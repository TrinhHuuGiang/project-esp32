#ifndef _MAIN_INIT_APIS_
#define _MAIN_INIT_APIS_

/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */
// --------------> C common libs
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


// --------------> C & POSIX file system
#include <stdio.h>    // file:   interract with file
#include <sys/stat.h> // folder: check folder info
                        // mkdir: make new folder
#include <unistd.h>   // folder: delete empty folder 
#include <cJSON.h>    // parse json file

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
#include "_priority.h"

// layer 2 (Peripheral connections)
// layer 2.1 - internal peripherals
#include "i2c_master_handle.h"  // i2c master : init, deinit
#include "spi_master_handle.h"  // spi master : init, deinit
#include "adc1_setup_handle.h"  // ADC : init, deinit
#include "gpio_setup_handle.h"  // gpio : register, unregister isr service
#include "ledc_setup_handle.h"  // ledc pwm : init, deinit
#include "mcpwm_setup_handle.h" // mcpwm : init, deinit
#include "i2s_master_output.h"  // i2s master output : init, deinit
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
#include "widDr_display_performance.h"
#include "widDr_net_mqtt_manager.h"

// layer 4.2 - external drivers
// #include ""


// layer top (Sequence deploy initialization)


/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

typedef enum
{
    MAIN_RET_OK = 0,

    // Layer 1
    MAIN_RET_TASKSYNC_INIT_FAIL,


    // Layer 2.1

    MAIN_RET_I2C_MASTER_INIT_FAIL,//i2c
    MAIN_RET_I2C_MASTER_CONF_FAIL,
    MAIN_RET_I2C_MASTER_INST_FAIL,
    
    MAIN_RET_SPI_MASTER_INIT_FAIL, // spi
    MAIN_RET_SPI_MASTER_INST_FAIL,

    MAIN_RET_ADC1_INIT_FAIL,//adc1

    MAIN_RET_WIFI_NVS_INIT_FAIL, // wifi
    MAIN_RET_WIFI_CREATE_EVENTLOOP_FAIL,
    MAIN_RET_WIFI_SETUP_LWIP_FAIL,
    MAIN_RET_WIFI_SETUP_CREATE_ITF_STA_FAIL,
    MAIN_RET_WIFI_SETUP_CREATE_ITF_AP_FAIL,
    MAIN_RET_WIFI_DRIVER_INIT_FAIL,
    MAIN_RET_WIFI_SET_COUNTRY_FAIL,


    // Layer 2.2
    MAIN_RET_74HC165_INIT_INPUT_FAIL,//165
    MAIN_RET_74HC595_INIT_OUTPUT_FAIL,//595
    MAIN_RET_98357_TOFF_FAIL,//max98357

    // layer 2.3
    MAIN_RET_SPI_SDCARD_MOUNT_FAIL,// micro sd card


    // layer 2.4
    MAIN_RET_HTTP_SERVER_START_FAIL, // http server

    MAIN_RET_MQTT_CLIENT_INIT_FAIL, // mqtt client
    MAIN_RET_MQTT_CLIENT_REGIST_EVENTLOOP_FAIL,
    MAIN_RET_MQTT_CLIENT_START_FAIL,

    // layer 4.1
    MAIN_RET_WID_DISPLAY_INIT_FAIL,// display

    MAIN_RET_WID_REALTIME_FAIL,// realtime

    MAIN_RET_WID_MQTT_MANAGER_FAIL,//mqtt manager


    // main
    MAIN_RET_FIX_MISSING_FILE_FOLDER_FAIL,//main
    MAIN_RET_FIX_UPDATE_WIFI_FROM_FILE_FAIL,
    MAIN_RET_START_WIFI_MANAGER_FAIL,
    MAIN_RET_TAKE_CS_MU_FAIL,
    MAIN_RET_RELEASE_CS_MU_FAIL
} main_ret_err_t;



// main loop
#define MAIN_LOOP_CHECK_CYCLE    2000 // check error flag each 2second
#define MAIN_LOOP_RESTART_COUNTDOWN_NUM      5    // 5 time count down before restart
#define MAIN_LOOP_RESTART_COUNTDOWN_DELAY    2000 // 2s count down delay

/**
 * **********************************************************
 * API
 * **********************************************************
 */
/**
 * @brief check file system described in README.md
 * @note check after mount with sd card system
 * @retval !0 is fail
 */ 
uint8_t main_fix_necess_system_file_folder();


/**
 * @brief read wf_cf.dat and update WiFi info (AP và STA)
 * 
 * @note wf_cf.dat should be update by "sysDr_http_update" from app
 * @note and should recover by "sysDr_rsFactory" apis
 * 
 * @retval 0 Thành công
 * @retval >0 Lỗi tương ứng từng bước xử lý
 */
uint8_t main_update_wifi_apsta_from_file();




/**
 * @brief Create task wifi apsta manager driver
 */
void wifi_apsta_mode_driver_manager(void* args);




/**
 * @brief Config mqtt client with user, pass, broker uri, pem certificate
 * 
 * @return !0 is fail
 * 
 */
uint8_t main_update_mqtt_config_from_file_and_init_mqtt_client();

#endif