// API:
//   - start driver handle open 1 wav at a time
//   - APIs control force change wav file by write file name to buffer
//   - APIs control force delete file name then driver stop speaker I2S

/**
 * **********************************************************
 * Libs
 * **********************************************************
 */

#include "widDr_wav_player.h"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */

extern __task_sync_t* g_task_sync_tools; // get sync tools 

static SemaphoreHandle_t g_mutex = NULL;
static char g_requested_path[WIDDR_AUDIO_CMD_MAX_PATH] = {0}; // buffer file name wav

#define WIDDR_WAV_PLAYER "wav player"

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
// audio task
static void audio_player_task(void *param) ;


/**
 * @brief parse wav header
 * @note always open file success before use this function
 * 
 * @retval 0 ok, !0 fail
 *  */ 
static int wav_parse_header(FILE *fp, wav_header_info_t *info);


/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// parse wav header
static int wav_parse_header(FILE *fp, wav_header_info_t *info) 
{
    //check NULL
    if (!fp || !info) return 1;

    // set cursor at start file
    rewind(fp);
    uint8_t header[44];

    // check file header length
    if (fread(header, 1, 44, fp) != 44) 
        return 2;


    // check format RIFF and WAVE
    if (memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVE", 4) != 0)
        return 3;

    // get sample rate, bit per sample, number channel
    info->sample_rate = header[24] | (header[25] << 8) | (header[26] << 16) | (header[27] << 24);
    info->bits_per_sample = header[34] | (header[35] << 8);
    info->num_channels = header[22] | (header[23] << 8);

    return 0;
}



// ========== Internal Task ==========

static void audio_player_task(void *param)
{
    char local_path[WIDDR_AUDIO_CMD_MAX_PATH] = {0};

    uint8_t playing = 0; // set if start play audio

    uint8_t new_req = 0; // set if check new request path


    uint8_t ret = 0;

    uint8_t *pcm_buf = malloc(WIDDR_AUDIO_BUFFER_DISPLAY_SIZE); // 4096 << sdcard sometime fail read sector :(

    if (!pcm_buf)
    {
        // set global error
        g_task_sync_tools->err_flag = 1;
        vTaskDelete(NULL); // end task
        return;
    }

    // file handle 
    FILE* file_ptr = NULL; // Note when code reduce reopen file, it harm for sd card
                            // only close file when stop old wav
                            // only open file when start new wav
    const long pos = 44; // position cursor point to data in file

    long cur_pos = pos;

    long file_size = 0;

    while (!ret)
    {
        xSemaphoreTake(g_mutex, portMAX_DELAY);







//-----------------------------------------------------------
//--------------------------check new request-------------------------
//-----------------------------------------------------------

        // check path empty (no request)
        if (strlen(g_requested_path) == 0) 
        {
            ESP_LOGI(WIDDR_WAV_PLAYER,"new request: %d, is playing: %d", new_req, playing);
            // contine handle playback

        }
        else // new request
        {
            // new request link update, then clean request
            strncpy(local_path, g_requested_path, WIDDR_AUDIO_CMD_MAX_PATH - 1);
            g_requested_path[0]='\0';
            
            ESP_LOGI(WIDDR_WAV_PLAYER,"Request:[%s]", local_path);

            // set flag new_req then check to play another link below
            new_req = 1;
        }






//-----------------------------------------------------------
//--------------------------check file exist and config i2s output --------------
//-----------------------------------------------------------

        // handle audio play
        if (new_req) // handle playback if new request 
        {
            if(playing) //if new request  but playing old track
            {

                playing = 0; new_req = 0;

                // turn off speaker
                if(i2s_max98357_turn_off())
                {
                    ret = 1; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }

                // stop and clean old audio track
                i2s_master_output_deinit();
            }
            else //if new request  but not playing old track
            {
                new_req = 0;
            }







            // check file exist in file system
            take_cs_spi_mutex(); // take cs spi mutex
            if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0)) 
            {
                release_cs_spi_mutex();

                ESP_LOGE(WIDDR_WAV_PLAYER,"get cs spi failed, file might not close!!!");

                ret = 2; // end task
                g_task_sync_tools->err_flag = 1; // notify main stop
                goto delay_audio_task;
            }

            
            // check file exist
            struct stat st;
            if (stat(local_path, &st) != 0)
                goto delay_audio_task;// file not exist -> sleep

            
            if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
            {
                release_cs_spi_mutex();

                ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                ret = 3; // end task
                g_task_sync_tools->err_flag = 1; // notify main stop
                goto delay_audio_task;
            }
            // release cs spi mutex
            release_cs_spi_mutex();







            
            // check tail of file by strrchr (search last charactor)
            // return turn pointer to '.', NULL if fail
            const char *ext = strrchr(local_path, '.');
            // check NULL before check compare case-insensitive 
            if (!ext || strcasecmp(ext, ".wav") != 0)
            {
                // file invalid type
                goto delay_audio_task; // file type wrong -> sleep

            }







            // read file wav
            take_cs_spi_mutex(); // take cs spi mutex
            if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0)) 
            {
                release_cs_spi_mutex();

                ESP_LOGE(WIDDR_WAV_PLAYER,"get cs spi failed, file might not close!!!");

                ret = 4; // end task
                g_task_sync_tools->err_flag = 1; // notify main stop
                goto delay_audio_task;
            }

            if(file_ptr)
            {
                // if file_ptr not NULL, then close it
                fclose(file_ptr);
                // file_ptr = NULL; // fopen below will return new pointer
            }

            ESP_LOGI(WIDDR_WAV_PLAYER,"[local: %s]", local_path);

            file_ptr = fopen(local_path, "rb");
            if (!file_ptr) 
            {
                ret = 5; // end task
                g_task_sync_tools->err_flag = 1; // notify main stop
                goto delay_audio_task;
            };


            // parse wav file header
            wav_header_info_t wav_info;
            if (wav_parse_header(file_ptr, &wav_info) != 0) 
            {
                fclose(file_ptr);
                file_ptr = NULL;

                if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                {
                    release_cs_spi_mutex();

                    ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                    ret = 6; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop
                    goto delay_audio_task;
                }

                // release cs spi mutex
                release_cs_spi_mutex();

                goto delay_audio_task;
            }


            // check wav header info done
            if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
            {
                release_cs_spi_mutex();

                ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                ret = 7; // end task
                g_task_sync_tools->err_flag = 1; // notify main stop
                goto delay_audio_task;
            }
            // release cs spi mutex
            release_cs_spi_mutex();





            
            
            // config i2s output
            i2s_master_output_config_t i2s_cfg = 
            {
                .sample_rate = wav_info.sample_rate,
                .bits_per_sample = (i2s_bits_per_sample_t)wav_info.bits_per_sample,
                .channel_format = (wav_info.num_channels == 1) ? I2S_CHANNEL_FMT_ONLY_LEFT : I2S_CHANNEL_FMT_RIGHT_LEFT,
            };


            // init i2s output
            if(i2s_master_output_init(&i2s_cfg))
            {
                // close file before outtask
                take_cs_spi_mutex(); // take cs spi mutex
                if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0)) 
                {
                    release_cs_spi_mutex();

                    ESP_LOGE(WIDDR_WAV_PLAYER,"get cs spi failed, file might not close!!!");

                    ret = 8; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop
                    goto delay_audio_task;
                }


                fclose(file_ptr);
                file_ptr = NULL;

                if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                {
                    release_cs_spi_mutex();

                    ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                    ret = 9; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop
                    goto delay_audio_task;
                }
                // release cs spi mutex
                release_cs_spi_mutex();

                ret = 10; // end task
                g_task_sync_tools->err_flag = 1; // notify main stop
                goto delay_audio_task;
            }


        }
        else // no request
        {
            if(!playing)   // no request no play -> idle time      
            {
                ESP_LOGI(WIDDR_WAV_PLAYER,"no request, no play, idle time");

                // continue stop play
                vTaskDelay(WIDDR_AUDIO_TASK_IDLE_DELAY); // delay 500ms
                goto delay_audio_task; //no new request, no playing -> continue stop play
            }

            // else no request but playing old audio track
            // file ok continue go down and read continue data

        }








//-----------------------------------------------------------
//-------------------------- get data to audio -------------------------
//-----------------------------------------------------------

        // get continue data from file and push out
        take_cs_spi_mutex(); // take cs spi mutex

        if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0)) 
        {
            release_cs_spi_mutex();

            ESP_LOGE(WIDDR_WAV_PLAYER,"get cs spi failed, file might not close!!!");

            ret = 11; // end task
            g_task_sync_tools->err_flag = 1; // notify main stop

            goto delay_audio_task;
        }



        // If playing but no new request state -> continue old file
        if(playing) // continue
        {   
            ESP_LOGI(WIDDR_WAV_PLAYER,"cur_pos: %ld/ %ld", cur_pos, file_size);

            if(cur_pos >= file_size )   // eof reached
            {
                playing = 0; // stop playing
                ESP_LOGW(WIDDR_WAV_PLAYER,"End of file reached, stop playback");

                fclose(file_ptr); // close file
                file_ptr = NULL;

                if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                {
                    release_cs_spi_mutex();

                    ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                    ret = 10; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }

                // release cs spi mutex
                release_cs_spi_mutex();

                // turn off speaker
                if(i2s_max98357_turn_off())
                {
                    ret = 12; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }
                // stop and clean old audio track
                i2s_master_output_deinit();

                goto delay_audio_task;
            }


            // read data from file
            clearerr(file_ptr); // clear before error
            fread(pcm_buf,1,WIDDR_AUDIO_BUFFER_DISPLAY_SIZE,file_ptr); // read data  
            
            {
                uint8_t retry_read = WIDDR_AUDIO_RETRY_FREAD_TIME_IF_FAIL;  // << i don't know why but when i try fread at first time
                                        // it always get error
                                        // sometime when i retry read, this problen reach
                // try check error
                if(ferror(file_ptr))
                {
                    while(retry_read)
                    {
                        clearerr(file_ptr);
                        ESP_LOGW(WIDDR_WAV_PLAYER,"Retry fread %hhu",retry_read);
                        fseek(file_ptr,cur_pos,SEEK_SET);
                        fread(pcm_buf,1,WIDDR_AUDIO_BUFFER_DISPLAY_SIZE,file_ptr); // read data 

                        if(ferror(file_ptr))
                        {
                            retry_read--;
                        }
                        else break;
                    } 
                }
            }

            
            // check error last time, if fail then stop audio
            if(ferror(file_ptr)) // read file failed
            {
                fclose(file_ptr);
                file_ptr = NULL;

                if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                {
                    release_cs_spi_mutex();

                    ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                    ret = 12; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }
                // release cs spi mutex
                release_cs_spi_mutex();

                playing = 0;

                // turn off speaker
                if(i2s_max98357_turn_off())
                {
                    ret = 13; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }

                // stop and clean old audio track
                i2s_master_output_deinit();
                
                ESP_LOGE(WIDDR_WAV_PLAYER,"Read file failed");

            }
            // if no error then playback
            else
            {
                if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                {
                    release_cs_spi_mutex();

                    ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                    ret = 17; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }

                // update cursor position
                cur_pos += WIDDR_AUDIO_BUFFER_DISPLAY_SIZE;

                // release cs spi mutex
                release_cs_spi_mutex();


                // send playback to DMA
                if(i2s_master_output_write(pcm_buf, WIDDR_AUDIO_BUFFER_DISPLAY_SIZE,
                                    I2S_MASTER_DMA_SUPPLY_TIME_MAX_RECOMMEND))
                {

                    //close file before outtask
                    take_cs_spi_mutex(); // take cs spi mutex
                    if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0)) 
                    {
                        release_cs_spi_mutex();

                        ESP_LOGE(WIDDR_WAV_PLAYER,"get cs spi failed, file might not close!!!");

                        ret = 14; // end task
                        g_task_sync_tools->err_flag = 1; // notify main stop

                        goto delay_audio_task;
                    }


                    fclose(file_ptr); // close file before outtask
                    file_ptr = NULL;

                    if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                    {
                        release_cs_spi_mutex();

                        ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                        ret = 14; // end task
                        g_task_sync_tools->err_flag = 1; // notify main stop

                        goto delay_audio_task;
                    }
                    // release cs spi mutex
                    release_cs_spi_mutex();

                    ret = 15; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop
                    goto delay_audio_task;
                }

            }

            // ok go to sleep

        }



        
        // if new request play
        else // prepare play
        {
            // get data from file
            // set cursor then read data
            if(fseek(file_ptr, pos, SEEK_SET) || 
                (fread(pcm_buf,1,WIDDR_AUDIO_BUFFER_DISPLAY_SIZE,file_ptr)  <= 0))
            {
                fclose(file_ptr); // read file failed
                file_ptr = NULL;

                if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                {
                    release_cs_spi_mutex();

                    ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                    ret = 16; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }

                // release cs spi mutex
                release_cs_spi_mutex();

            }
            // start sound
            // playback i2s
            else
            {
                fseek(file_ptr, 0, SEEK_END); // set cursor to end file
                file_size = ftell(file_ptr); // get file size

                if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                {
                    release_cs_spi_mutex();

                    ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                    ret = 17; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }

                // release cs spi mutex
                release_cs_spi_mutex(); 
                
                // update cursor position
                cur_pos = pos + WIDDR_AUDIO_BUFFER_DISPLAY_SIZE;
                
                // turn on speaker
                if(i2s_max98357_turn_on())
                {
                    take_cs_spi_mutex(); // take cs spi mutex
                    if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0)) 
                    {
                        release_cs_spi_mutex();

                        ESP_LOGE(WIDDR_WAV_PLAYER,"get cs spi failed, file might not close!!!");

                        ret = 18; // end task
                        g_task_sync_tools->err_flag = 1; // notify main stop

                        goto delay_audio_task;
                    }

                    fclose(file_ptr); // close file before outtask
                    file_ptr = NULL;

                    if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                    {
                        release_cs_spi_mutex();

                        ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                        ret = 17; // end task
                        g_task_sync_tools->err_flag = 1; // notify main stop

                        goto delay_audio_task;
                    }

                    // release cs spi mutex
                    release_cs_spi_mutex();

                    ret = 18; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop

                    goto delay_audio_task;
                }


                // send playback to DMA
                if(i2s_master_output_write(pcm_buf, WIDDR_AUDIO_BUFFER_DISPLAY_SIZE,
                                    I2S_MASTER_DMA_SUPPLY_TIME_MAX_RECOMMEND))
                {
                    take_cs_spi_mutex(); // take cs spi mutex
                    if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0)) 
                    {
                        release_cs_spi_mutex();

                        ESP_LOGE(WIDDR_WAV_PLAYER,"get cs spi failed, file might not close!!!");

                        ret = 19; // end task
                        g_task_sync_tools->err_flag = 1; // notify main stop

                        goto delay_audio_task;
                    }

                    fclose(file_ptr); // close file before outtask
                    file_ptr = NULL;

                    if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
                    {
                        release_cs_spi_mutex();

                        ESP_LOGE(WIDDR_WAV_PLAYER,"release cs spi failed, file might not close!!!");

                        ret = 19; // end task
                        g_task_sync_tools->err_flag = 1; // notify main stop

                        goto delay_audio_task;
                    }

                    // release cs spi mutex
                    release_cs_spi_mutex();

                    ret = 20; // end task
                    g_task_sync_tools->err_flag = 1; // notify main stop
                    goto delay_audio_task;
                }


                // set playing
                playing = 1;
            }


        }

delay_audio_task:

        xSemaphoreGive(g_mutex);
        // vTaskDelay(pdMS_TO_TICKS(WIDDR_AUDIO_TASK_DELAY)); // 10ms
    }

    // end task
    free(pcm_buf);

    ESP_LOGE(WIDDR_WAV_PLAYER,"Audio player task end with error code: %d", ret);

    vTaskDelete(NULL);
}



// ========== Public API ==========

uint8_t widDr_audio_player_init(void)
{
    // init mutex
    g_mutex = xSemaphoreCreateMutex();
    if (!g_mutex) return 1;

    // tạo task
    BaseType_t ok = xTaskCreate(
        audio_player_task, WIDDR_AUDIO_TASK_NAME,
        WIDDR_AUDIO_TASK_STACK_SIZE, NULL,
        WIDDR_AUDIO_TASK_PRIORITY, NULL
    );
    if (ok != pdPASS) return 2;

    return 0;
}


// request file
uint8_t widDr_audio_player_set_file(const char *wav_file_path)
{
    if (!wav_file_path) return 1;

    xSemaphoreTake(g_mutex, portMAX_DELAY);

    strncpy(g_requested_path, wav_file_path, WIDDR_AUDIO_CMD_MAX_PATH - 1);

    xSemaphoreGive(g_mutex);

    return 0;
}



// clear audio file and stop
void widDr_audio_player_stop(void)
{
    xSemaphoreTake(g_mutex, portMAX_DELAY);

    g_requested_path[0] = '\0';
    
    xSemaphoreGive(g_mutex);
}