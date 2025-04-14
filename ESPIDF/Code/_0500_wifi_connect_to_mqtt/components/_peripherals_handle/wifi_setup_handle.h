// Note: 
// + go to Compiler config -> Wi-Fi -> WiFi NVS flash , then active NVS flash to save SSID and password inform
// + this library will init LwIP and event loop for event task but not have a function to free them.
// If necessary, please find out more at: https://docs.espressif.com/projects/esp-idf/en/v4.4.8/esp32/api-guides/wifi.html

// This library: handle general scenarios of AP, STA, STA+AP mode step by step
//

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

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

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

// 1. choose mode for wifi driver
_peripherals_err_t wifi_setup_set_wifi_mode();




// =================================== 2.5 Config wifi inform      ===================================
// =================================== ==========================  ===================================
// 1. Wifi country
_peripherals_err_t wifi_setup_set_wifi_country();

// 2. Wifi config
_peripherals_err_t wifi_setup_set_wifi_config();




// =================================== 3 Start wifi driver         ===================================
// =================================== ==========================  ===================================

// 1. after config all inform for wifi, start wifi
_peripherals_err_t wifi_setup_start_wifi_driver();




// =================================== 3.5 Scan wifi               ===================================
// =================================== ==========================  ===================================
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




// Stop wifi driver
_peripherals_err_t wifi_setup_de_init_wifi_driver();


// =================================== Extend                      ===================================
// =================================== ==========================  ===================================

// delete wifi interface
// free loop
// clear LwIP

// this case can auto clear after reset chip

#endif