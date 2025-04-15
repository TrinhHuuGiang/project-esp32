// Note: 
// + go to Compiler config -> Wi-Fi -> WiFi NVS flash , then active NVS flash to save SSID and password inform
// + this library will init LwIP and event loop for event task but not have a function to free them.
// If necessary, please find out more at: https://docs.espressif.com/projects/esp-idf/en/v4.4.8/esp32/api-guides/wifi.html
// + BSSID (Basic Service Set Identifier) == MAC address và SSID (Service Set Identifier) là tên wifi
// + Wifi channel: when in STA+AP mode, channel of AP will auto modify depend on channel STA mode connected

// This library: handle general scenarios of AP, STA, STA+AP mode step by step


#ifndef _WIFI_SETUP_HANDLE_H_
#define _WIFI_SETUP_HANDLE_H_

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

//esp32
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"


//user
#include "_peripherals_err.h"

// wifi default inititial refer from esp_wifi.h
#define WIFI_SETUP_EVENT_TASK_LOOP_NAME    "wifi event task"
#define WIFI_SETUP_EVENT_TASK_QUEUE_SIZE   CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE
#define WIFI_SETUP_EVENT_TASK_STACK_SIZE   ESP_TASKD_EVENT_STACK
#define WIFI_SETUP_EVENT_TASK_PRIORITY     ESP_TASKD_EVENT_PRIO
#define WIFI_SETUP_EVENT_CORE_BIND_ID      0

// country set infor
#define WIFI_SETUP_SET_DEFAULT_COUNTRY_CODE    "01" // world safe mode
#define WIFI_SETUP_CHOOSE_AUTO_FIX_COUNTRY     (1)


// wifi authen for STA mode
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

// default wifi config for AP mode
#define WIFI_SETUP_WIFI_CONFIG_AP_MAX_CONN           (4) // each client connect take alot of RAM about 50KB, max limit connect 10 by espidf
#define WIFI_SETUP_WIFI_CONFIG_AP_CHANNEL_DEFAULT    (1) // channel 1 - 13 with option "01" world safe mode behind
#define WIFI_SETUP_WIFI_CONFIG_AP_AUTHEN_MODE        WIFI_AUTH_WPA_WPA2_PSK


// default wifi scan infor
#define WIFI_SETUP_WIFI_SCAN_SSID      (NULL) // scan all ssid << change if want scan specific wifi ssid
#define WIFI_SETUP_WIFI_SCAN_BSSID     (NULL) // scan all MAC with same ssid 
#define WIFI_SETUP_WIFI_SCAN_HIDEN_WF  (0)    // do not scan hiden wifi << change if want scan hiden wifi
#define WIFI_SETUP_WIFI_SCAN_TYPE               WIFI_SCAN_TYPE_ACTIVE
#define WIFI_SETUP_WIFI_ACTIVE_SCAN_MIN_TIME_MS (100) // limit speed send probe
#define WIFI_SETUP_WIFI_ACTIVE_SCAN_MAX_TIME_MS (200) // maximum time wait feed back. esp32 limit 1500ms
#define WIFI_SETUP_WIFI_SCAN_BLOCK     (false) // wifi scan stop scan when be requested




/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

// 0 Wifi start NVS memory before go to step 2
// check the box:
// Compiler config -> Wi-Fi -> WiFi NVS flash 

// =================================== 1. Init phase               ===================================
// =================================== =========================== ===================================
// 1. Init lightweight ip to handle TCP/IP stack
_peripherals_err_t wifi_setup_init_lightweight_ip_inform();

// 2. init event loop for `event task`
_peripherals_err_t wifi_setup_init_event_loop();

// 3. create instance Wifi interface connect to LwIP
// when using Station mode or STA+AP mode
_peripherals_err_t wifi_setup_create_interface_wifi_STA_link_LwIP();
// when using AP mode or STA+AP mode
_peripherals_err_t wifi_setup_create_interface_wifi_AP_link_LwIP();

// 4. init `wifi driver`
_peripherals_err_t wifi_setup_init_wifi_driver();





// =================================== 2. Configuration phase      ===================================
// =================================== =========================== ===================================

// Register into event loop to check wifi status
_peripherals_err_t wifi_setup_regist_receive_event_task();





// =================================== 2.5 Config wifi inform      ===================================
// =================================== these functions (option)    ===================================
// =================================== only call when update NVS data ================================
// =================================== esp_wifi_set_xxx APIs        ==================================
// =================================== auto save in NVS flash       ==================================

// 0. choose mode for wifi driver
_peripherals_err_t wifi_setup_set_wifi_mode();

// 1. Wifi country
_peripherals_err_t wifi_setup_set_wifi_country();


// 2 Wifi config
// Independent configuration for STA or AP mode
_peripherals_err_t wifi_setup_set_wifi_sta_config();

_peripherals_err_t wifi_setup_set_wifi_ap_config();




// =================================== 3 Start wifi driver         ===================================
// =================================== ==========================  ===================================

// 1. after config all inform for wifi, start wifi
_peripherals_err_t wifi_setup_start_wifi_driver();




// =================================== 3.5 Scan wifi               ===================================
// =================================== Only for STA in (STA/ STA+AP )  ===============================
// Start scan
_peripherals_err_t wifi_setup_start_scan_wifi();


_peripherals_err_t wifi_setup_get_wifi_list_scanned();





// =================================== 4. Wifi connect phase       ===================================
// =================================== Only for STA in (STA/ STA+AP ) ================================
// 1. connect to wifi
_peripherals_err_t wifi_setup_connect_to_access_point();





// =================================== 5. Wifi got IP phase        ===================================
// =================================== Notify for STA in (STA/ STA+AP )  =============================
// got ip
// notify by event task
// handle by hand





// =================================== 6. Disconnect feedback      ===================================
// =================================== Notify for STA in (STA/ STA+AP )  =============================
// got ip
// notify by event task
// handle by hand




// =================================== 7. Wifi IP change phase     ===================================
// =================================== Notify for STA in (STA/ STA+AP )  =============================
// changed ip
// notify by event task
// handle by hand


// =================================== 8. Deinit wifi              ===================================
// =================================== ==========================  ===================================

// disconnect only for STA in (STA/ STA+AP ) 
_peripherals_err_t wifi_setup_disconnect_wifi();



// Stop wifi driver
_peripherals_err_t wifi_setup_stop_wifi_driver();




// de init wifi driver
_peripherals_err_t wifi_setup_de_init_wifi_driver();


// =================================== Extend                      ===================================
// =================================== ==========================  ===================================

// delete wifi interface
// free loop
// clear LwIP

// this case can auto clear after reset chip



#endif