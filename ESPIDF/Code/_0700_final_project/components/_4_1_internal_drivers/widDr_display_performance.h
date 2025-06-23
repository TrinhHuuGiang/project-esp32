// total :  2 line perform (heap, bat), 
//      1 blank line  (reserve)
//          2 line time (day, time), 
//      1 blank line  (reserve)
//          2 line log (), 

// ssd1306 (128x64 pixel) has 128 seg x 8 page (eachpage 8 row == 1024 bit a page == 128 byte
// full screen == 1KB

// Note:
// - this library include:
//  + 1 API create driver handle display performance (6 line first)
//      (thread safe when only call 1 time)
//      update frequency 1second
//      
//      task auto delete after get error code, then set global g_task_sync_tools->err_flag = 1 ;
//      Warn: just say again only call 1 time :)
//  
//  + 1 API common write line 6 and 7 with content is
//      (thread safe by mutex at i2c at layer 2_1)
//      content: "Log: +11 word" 
//          and: "[  +14 word ]"
//      problem: Contents can be overwritten from multiple tasks.
//      solution: :) i will not sync here for reduce complexity 
//                      but we can sync and delay a second

#ifndef _WIDDR_BATTERY_H_
#define _WIDDR_BATTERY_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

//c
#include <stdlib.h> // Dynamic allocation
#include <stdint.h> // type int
#include <string.h>
#include <stdbool.h>

// esp32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

// user define
#include "_peripherals_err.h"
#include "__task_sync.h"
#include "_priority.h"

// define display
#include "i2c_ssd1306.h"
#include "i2c_ds3231.h"
#include "widDr_batterycheck.h"


#if CONFIG_WID_DISPLAY_SSD1306_ADDR_3C//default on my board :)
    #define WIDDR_DISPLAY_SSD1306_ADDR 0x3C
#else
    #define WIDDR_DISPLAY_SSD1306_ADDR 0x3D
#endif

#define WIDDR_DISPLAY_SSD1306_RENEW_TIME 1000 // 1s

typedef enum
{
    // page 0 - 7 of ssd1306
    WIDDR_LINE_0 = 0,  // HEAP size
    WIDDR_LINE_1,      // bat
    WIDDR_LINE_2,      // reserve
    WIDDR_LINE_3,      // date
    WIDDR_LINE_4,      // time
    WIDDR_LINE_5,      // reserve
    WIDDR_LINE_6,      // log topic
    WIDDR_LINE_7       // log content
} widdr_display_line_num_t ;

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

// public function: init display and run background task
// this function create driver handle display performance (6 line first)
// return 0 is ok
uint8_t widDr_display_init_and_run(void);



// update 2 last log lines (not sync)
uint8_t widDr_display_write_log_line(const char* topic, const char* content);



#endif