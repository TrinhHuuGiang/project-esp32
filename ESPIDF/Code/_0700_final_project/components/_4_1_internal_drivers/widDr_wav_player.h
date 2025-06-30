/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


// API:
//   - start driver handle open 1 wav at a time
//   - APIs control force change wav file by write file name to buffer



// Test on SDcard class 4:
//      - maximum : 1MHz SPI -> test at 4MHz sometime can't fread, must fclose then reopen -> very harm for card
//      - buffer size : 2048 -> at 4096 some time got " diskio_sdmmc: sdmmc_read_blocks failed (263) "
//      - test at wav 8khz ok but at 12khz the sound is delay, and glitch


// [Ideal condition for stable WAV playback using I2S + SPI (1 MHz)]
//
// + SPI speed: 1 MHz = 1,000,000 bps = 125,000 Bps
// + Buffer size: 2048 bytes
//    + SPI read time per buffer = 2048 / 125000 ≈ 16.38 ms
//
// + WAV format:
//     Sample rate: 8 kHz
//     Bit depth: 16 bit
//     Channels: 1 (mono)
//     + Bitrate = 8000 × 16 × 1 = 128,000 bps = 16,000 Bps
//
// + Buffer playback duration = 2048 / 16000 = 0.128 s = 128 ms
//     + Need to update buffer ~8 times per second
//     + CPU has ≈ 128 - 16.38 ≈ 111.6 ms idle per cycle
//     + Read time ≈ 12.8% of each cycle
//
// => Rule: must feed I2S every ≤128 ms
// => Safe zone: complete fread + prepare next buffer in < 110 ms
// => CPU consummed: ~13%


// 


/**
 * **********************************************************
 * Libs
 * **********************************************************
 */

#ifndef _WIDDR_AUDIO_PLAYER_H_
#define _WIDDR_AUDIO_PLAYER_H_



//c
#include <stdlib.h> // Dynamic allocation
#include <stdint.h> // type int
#include <string.h>
#include <stdbool.h>

// system file
#include <sys/stat.h> // check existing file

// esp32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// user define
#include "_peripherals_err.h"
#include "__task_sync.h"

#include "_priority.h"


// necess
#include "i2s_max98357.h"
#include "i2s_master_output.h"

#include "gpio_74hc595.h"
#include "spi_sd_mount_to_fs.h"

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

// task info
#define WIDDR_AUDIO_TASK_NAME        "widDr_audio_task"
#define WIDDR_AUDIO_TASK_STACK_SIZE  TASK_STACK_SIZE_EXTREM    // << stack over flow at TASK_STACK_SIZE_HIGH
#define WIDDR_AUDIO_TASK_PRIORITY    TASK_PRIO_NORMAL

// nodelay : if u wanna test, uncomment below macro and xTaskDelay at the end of widDr_audio_player_init()
// other wise : player will playback thoughout when have request then use default delay IDLE MODE below
#define WIDDR_AUDIO_TASK_DELAY       10  // should >=10ms. otherwise remove pdMS_TO_TICKS by tick
                            // 10ms is limit of vTaskDelay at setting configTICK_RATE_HZ 100
                            // if play too gleak, use tick instead of pdMS_TO_TICKS


#define WIDDR_AUDIO_TASK_IDLE_DELAY  pdMS_TO_TICKS(500) // delay when no request, no play
                                                    // addition delay to avoid high cpu usage

#define WIDDR_AUDIO_BUFFER_DISPLAY_SIZE      2048   
                                // test at 4096, 8192, 16384 B both is bug: 
                                //          " diskio_sdmmc: sdmmc_read_blocks failed (263) "
                                // on type C4 class mycro SD card

#define WIDDR_AUDIO_RETRY_FREAD_TIME_IF_FAIL  5

#define WIDDR_AUDIO_CMD_MAX_PATH     128


typedef struct {
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint16_t num_channels;
} wav_header_info_t;


/**
 * **********************************************************
 * APIs
 * **********************************************************
 */


/**
 * @brief Init and start driver control i2s sound speaker (I2S + MAX98357 + .wav)
 * 
 * Only call 1 time at restart
 * 
 * @return 0 if success, !0 if failed
 */
uint8_t widDr_audio_player_init(void);

/**
 * @brief Change Wav file
 * 
 * @param wav_file_path absolute link to file in file system
 * 
 * @note if file not exist -> no change
 * 
 * @return 0 if OK, otherwise failed
 */
uint8_t widDr_audio_player_set_file(const char* wav_file_path);

/**
 * @brief Stop sound current wav file
 * @note this function not available if u have a file name is "0"
 * @note this function try write "0" as a no exist file then driver stop play audio
 */
void widDr_audio_player_stop(void);


#endif