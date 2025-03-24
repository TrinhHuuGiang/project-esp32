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
#include "_peripherals_err.h"  // peripheral debug
#include "i2c_master_handle.h" // i2c
#include "spi_master_handle.h" // spi

#include "i2c_ssd1306.h"
#include "i2c_ds3231.h"

#include "spi_mcp41010.h"


// debug
#if CONFIG_DEBUG_ENABLE !=0

#define CONFIG_ESP_ERR_TO_NAME_LOOKUP

#endif

// define
#define SSD1306_ADDR (0x3C)

#define MCP41010_CS_PIN (17)  // GPIO17


#endif