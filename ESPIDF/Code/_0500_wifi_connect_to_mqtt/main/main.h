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
#include "esp_system.h"        //log ram

// user defined

// peripherals
#include "_peripherals_err.h"  // peripheral debug
#include "__task_sync.h"       // init sync tools
#include "i2c_master_handle.h" // i2c
#include "spi_master_handle.h" // spi
#include "gpio_setup_handle.h" // gpio
#include "ledc_setup_handle.h" // ledc pwm
#include "wifi_setup_handle.h" // wifi

// protocols
#include "mqtt_pubsub_handle.h" // mqtt client
#include "http_server_handle.h" // http server



// io device
#include "i2c_ssd1306.h"
#include "spi_sd_mount_to_fs.h"
#include "gpio_74hc165.h"



// define
#define SSD1306_ADDR (0x3C)


#endif