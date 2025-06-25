// config master mode
// using I2S_NUM_0
// apply for MAX98357 -> do not reuse this library 

// control only by "widDr_wav_player.h" otherwise not thread safe



#ifndef _I2S_MASTER_OUTPUT_H_
#define _I2S_MASTER_OUTPUT_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************/
//c
#include <stdlib.h> // dynamic alloction
#include <stdio.h>  // print serial
#include <stdint.h> // type int

//esp32
#include "driver/i2s.h"
#include "esp_err.h"        // const char *esp_err_to_name(esp_err_t code);

//user
#include "_peripherals_err.h"
#include "__task_sync.h"


// setup i2s
#define I2S_MASTER_OUTPUT_PORT            I2S_NUM_0

#define I2S_MASTER_DMA_BUF_COUNT          8
#define I2S_MASTER_DMA_BUF_LEN            512


#define I2S_MASTER_BCK_PIN                CONFIG_I2S_MASTER_BCK_PIN   
#define I2S_MASTER_WS_PIN                 CONFIG_I2S_MASTER_WS_PIN
#define I2S_MASTER_DO_PIN                 CONFIG_I2S_MASTER_DO_PIN


//  Configurable through these enums 
typedef struct {
    uint32_t sample_rate;                   // 8000, 44100,...
    i2s_bits_per_sample_t bits_per_sample; // bit per sample 8 16 24 32
    i2s_channel_fmt_t channel_format;      // go to definiton: right left, onlyright ,...
} i2s_master_output_config_t;





/**
 * **********************************************************
 * Sync 
 * (never use these mutex if call any APIs below, it included)
 * (only use when other device is conflicted with this peripheral)
 * *********************************************************
 * */

// get periph mutex
void take_i2s_mutex();

// release periph mutex
void release_i2s_mutex();



/**
 * **********************************************************
 * APIs
 * **********************************************************/

/**
 *  Initialize I2S driver
 * Warn: alway deinit before init
 * */
_peripherals_err_t i2s_master_output_init(const i2s_master_output_config_t *cfg);

// Write PCM buffer (blocking)
_peripherals_err_t i2s_master_output_write(const void *pcm_data, size_t size, TickType_t timeout);

// Stop and uninstall I2S
void i2s_master_output_deinit(void);




#endif