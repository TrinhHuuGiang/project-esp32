#ifndef _MAIN_INIT_APIS_
#define _MAIN_INIT_APIS_

/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */
// --------------> common libs
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// --------------> esp libs
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"  // reset

// --------------> Architecture libs

// layer 1 (System architecture infrastructure)
// layer 1 - Container structures
#include "container.h"
// layer 1 - Reference libs
#include "utlist.h"
// layer 1 - Task sync tools
#include "__task_sync.h"


// layer 2 (Peripheral connections)
// layer 2.1 - internal peripherals
#include "adc1_setup_handle.h"   // ADC : init, deinit
#include "gpio_setup_handle.h"  // gpio : register, unregister isr service
#include "i2c_master_handle.h"  // i2c master : init, deinit
#include "i2s_master_output.h"  // i2s master output : init, deinit
#include "ledc_setup_handle.h"  // ledc pwm : init, deinit
#include "mcpwm_setup_handle.h" // mcpwm : init, deinit
#include "spi_master_handle.h"  // spi master : init, deinit
#include "wifi_setup_handle.h"  // wifi setup : init, deinit
// layer 2.2 - external peripherals
#include "gpio_74hc165.h"       // PISO x32 chan
#include "gpio_74hc595.h"       // POSI x32 chan (4 for spi, 4 for adc input, 24 for general output)
#include "adc_74hc4067.h"       // ADC MUX DEMUX x16 chan
#include "i2s_max98357.h"       // speaker amplifier
// layer 2.3 - internal devices
#include "i2c_ssd1306.h"        // system log
#include "spi_sd_mount_to_fs.h" // files system
#include "i2c_ds3231.h"         // clock system
// layer 2.4 - network protocols
#include "http_server_handle.h" // http server
#include "mqtt_pubsub_handle.h" // mqtt client
#include "rtc_sntp.h"           // stnp update time

// layer 3 (User define IO devices)
// #include ""


// layer 4 (Controller drivers and User define drivers)
// layer 4.1 - internal drivers


#include "widDr_batterycheck.h"
#include "widDr_time_realtime.h"


// layer 4.2 - external drivers
// #include ""


// layer top (Sequence deploy initialization)


/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */




/**
 * **********************************************************
 * API
 * **********************************************************
 */




/*
Layer 1: 
    - Initialize container structure
    - Initialize synchorous tools
Layer 2: System resource
    - Initialize internal peripherals
    - Initialize external peripherals
    - Initialize internal IO device
    - Initialize network (OSI model: Physic (wifi), Datalink(Mac), Network(IP), Transport(TCP))
Layer 3: User define devices
    - Initialize external IO device
Layer 4: System drivers and user drivers
    - Initialize internal control driver
        - System driver:
            - Maintain system driver ( update, sleep, reset factory )
        - Widget driver:
            - Timer driver ( scheduling with clock device )
            - Network driver (implement protocol - Application layer in TCP/IP model)
            - Speaker player driver
                - Check `Read/Write file driver` before run file
                - Ensures synchronized audio `file` playback through a single device
                - Limit only 1 audio `file` playback ( reduce file must open for File system )
                - *Note:*
                    - File play back must be existed
                    - SPI_SD_CARD_MAX_FILE_OPEN limit number file can open at the same time
            - Read/Write file driver
                - Check `speaker driver` before write to file (fail if file be using)
                - Only read/write at the same time because SPI_SD_CARD_MAX_FILE_OPEN limit number file can open at the same time
            - Display driver
                - Display performance ( RAM / log %, bar )
                - Print new debug (time / error log)
    - Initialize external control driver
        - User define

- Loop waiting / blocked until end signal from special internal system driver then restart
*/



#endif