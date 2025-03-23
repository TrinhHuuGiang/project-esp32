#ifndef _MAIN_H_
#define _MAIN_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
// common libs
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// esp libs
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// user defined
#include "i2c_ssd1306.h"
#include "i2c_ds3231.h"


// debug
#if CONFIG_DEBUG_ENABLE !=0

#define CONFIG_ESP_ERR_TO_NAME_LOOKUP

#endif

// define
#define SSD1306_ADDR (0x3C)



#endif