/*
 * Copyright (C) 2025 Giang Trinh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

#include "i2s_master_output.h"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
extern __task_sync_t* g_task_sync_tools;

// i2s config
static i2s_master_output_config_t* i2s_cfg_ptr = NULL;


/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
// get periph mutex
void take_i2s_mutex()
{
    xSemaphoreTake(g_task_sync_tools->i2s_master_output_mutex, portMAX_DELAY);
}

// get periph mutex
void release_i2s_mutex()
{
    xSemaphoreGive(g_task_sync_tools->i2s_master_output_mutex);
}





_peripherals_err_t i2s_master_output_init(const i2s_master_output_config_t *cfg)
{
	//take mutex
	take_i2s_mutex();

    if (!cfg)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2S_MASTER_OUTPUT_PARAM_NULL, __FILE__, __LINE__, "Input null ptr");
        #endif
        
        
        //give mutex
        release_i2s_mutex();
        return I2S_MASTER_OUTPUT_PARAM_NULL;
    }


    if (i2s_cfg_ptr) 
    {
        // de init before
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2S_MASTER_OUTPUT_ERR_ALREADY_INIT, __FILE__, __LINE__, "i2s already init");
        #endif
        

        
        //give mutex
        release_i2s_mutex();
        return I2S_MASTER_OUTPUT_ERR_ALREADY_INIT;
    }

    // Allocate memory
    i2s_cfg_ptr = (i2s_master_output_config_t*) malloc(sizeof(i2s_master_output_config_t));
    if (!i2s_cfg_ptr)
    {
        // de init before
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2S_MASTER_OUTPUT_ALOC_FAILED, __FILE__, __LINE__, "i2s alloc failed");
        #endif
        
        
        //give mutex
        release_i2s_mutex();
        return I2S_MASTER_OUTPUT_ALOC_FAILED;
    }

    *i2s_cfg_ptr = *cfg;  // copy config

    i2s_config_t i2s_cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = cfg->sample_rate,
        .bits_per_sample = cfg->bits_per_sample,
        .channel_format = cfg->channel_format,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = I2S_MASTER_DMA_BUF_COUNT,
        .dma_buf_len = I2S_MASTER_DMA_BUF_LEN,
        .use_apll = false,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .tx_desc_auto_clear = true,
    };

    i2s_pin_config_t pin_cfg = {
        .bck_io_num = I2S_MASTER_BCK_PIN,
        .ws_io_num = I2S_MASTER_WS_PIN,
        .data_out_num = I2S_MASTER_DO_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE,
    };

    esp_err_t err = i2s_driver_install(I2S_MASTER_OUTPUT_PORT, &i2s_cfg, 0, NULL);
    if (err != ESP_OK)
    {
        free(i2s_cfg_ptr);
        i2s_cfg_ptr = NULL;

        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2S_MASTER_OUTPUT_DRIVER_INSTALL_FAILED, __FILE__, __LINE__,  esp_err_to_name(err) );
        #endif

                
        //give mutex
        release_i2s_mutex();
        return I2S_MASTER_OUTPUT_DRIVER_INSTALL_FAILED;
    }

    err = i2s_set_pin(I2S_MASTER_OUTPUT_PORT, &pin_cfg);
    if (err != ESP_OK) {
        i2s_driver_uninstall(I2S_MASTER_OUTPUT_PORT);
        free(i2s_cfg_ptr); 
        i2s_cfg_ptr = NULL;

        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2S_MASTER_OUTPUT_DRIVER_INIT_PIN_FAILED, __FILE__, __LINE__,  esp_err_to_name(err) );
        #endif

                
        //give mutex
        release_i2s_mutex();
        return I2S_MASTER_OUTPUT_DRIVER_INIT_PIN_FAILED;
    }

    i2s_zero_dma_buffer(I2S_MASTER_OUTPUT_PORT);
    
    //give mutex
    release_i2s_mutex();
    return PERIPH_OK;
}


_peripherals_err_t i2s_master_output_write(const void *pcm_data, size_t size, TickType_t timeout)
{
	//take mutex
	take_i2s_mutex();

    if (!pcm_data || size == 0) 
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2S_MASTER_OUTPUT_PARAM_NULL, __FILE__, __LINE__, "Input null ptr");
        #endif
        
                
        //give mutex
        release_i2s_mutex();
        return I2S_MASTER_OUTPUT_PARAM_NULL;
    }

    if (!i2s_cfg_ptr) 
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2S_MASTER_OUTPUT_CONFIG_NULL, __FILE__, __LINE__, "Miss i2s config");
        #endif
        
        
        //give mutex
        release_i2s_mutex();
        return I2S_MASTER_OUTPUT_CONFIG_NULL;
    }

    size_t bytes_written = 0;
    esp_err_t err = i2s_write(I2S_MASTER_OUTPUT_PORT, pcm_data, size, &bytes_written, timeout);

    if (err != ESP_OK || bytes_written != size)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2S_MASTER_OUTPUT_WRITE_FAILED, __FILE__, __LINE__, "i2s write failed");
        #endif
        
        
        //give mutex
        release_i2s_mutex();
        return I2S_MASTER_OUTPUT_WRITE_FAILED;
    }

    
    //give mutex
    release_i2s_mutex();
    return PERIPH_OK;
}


void i2s_master_output_deinit(void)
{
	//take mutex
	take_i2s_mutex();

    if (!i2s_cfg_ptr) 
    {   
        //give mutex
        release_i2s_mutex();
        return;
    }

    i2s_driver_uninstall(I2S_MASTER_OUTPUT_PORT);
    free(i2s_cfg_ptr);
    i2s_cfg_ptr = NULL;

    //give mutex
    release_i2s_mutex();
    return;
}