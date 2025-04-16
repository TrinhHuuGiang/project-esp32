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
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
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

// ap mode ip
#define WIFI_SETUP_AP_MODE_IP_ADDR         ( (192 << 24) | (168 << 16) | (1<<8)   | (1) )
#define WIFI_SETUP_AP_MODE_NET_MASK        ( (255 << 24) | (255 << 16) | (255<<8) | (0) ) // C class network by classful

// country set infor
#define WIFI_SETUP_SET_DEFAULT_COUNTRY_CODE    "01" // world safe mode
#define WIFI_SETUP_CHOOSE_AUTO_FIX_COUNTRY     (1)


// wifi authen for STA mode, go to menuconfig
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


// struct save state of wifi
// when handle wifi, sometime race condition can be reach
// manage state of wifi by this struct more suitable than statemachine
// any query this state struct will accept by specicals function for sure not race condition
// this struct auto alloc when call 'wifi_setup_start_wifi_driver();' and init all field is 0
// and de alloc when call "wifi_setup_stop_wifi_driver();"

typedef struct
{
    // handle mutex, protect state no affect by race condition
    SemaphoreHandle_t wifi_state_mutex;  // sure that any event task call the wifi event handle will queuing

    // state
    uint8_t sta_start;
    uint8_t sta_connect;
    uint8_t sta_got_ip;
    uint8_t ap_start;
    uint8_t ap_stop;
    uint8_t ap_number_sta_connected;

    // struct pointer to data
    wifi_event_ap_staconnected_t* ap_list_sta_connected[WIFI_SETUP_WIFI_CONFIG_AP_MAX_CONN]; 
            // pointer to NULL if no connect, or pointer to
            // struct pointer to copy value feed back of WIFI_EVENT_AP_STACONNECTED event
            // if a pointer exist when WIFI_EVENT_AP_STADISCONNECTED raise, it will free and point to NULL
    wifi_event_sta_connected_t* sta_dest_ap_connected;
            // default NULL if not connect. 
            // copy value feed back of WIFI_EVENT_STA_CONNECTED event
            //  it will free and point to NULL if WIFI_EVENT_STA_DISCONNECTED

    // sta scan/connect collision
    SemaphoreHandle_t sta_collision_conn_scan_mutex;
        // before get this mutex, give 'wifi_state_mutex'
        // after get mutex, take 'wifi_state_mutex'

} struct_wifi_state_t;

// mutex:
// + xSemaphoreCreateMutex()
// +  xSemaphoreTake()
// + xSemaphoreGive()
// +  vSemaphoreDelete()

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
// Only call 1 time to create default loop
_peripherals_err_t wifi_setup_init_default_event_loop();

// 3. create instance Wifi interface connect to LwIP
// when using Station mode or STA+AP mode
_peripherals_err_t wifi_setup_create_interface_wifi_STA_link_LwIP(esp_netif_t** sta_net_if);

// when using AP mode or STA+AP mode , change ip, netmask
_peripherals_err_t wifi_setup_create_interface_wifi_AP_link_LwIP(esp_netif_t** ap_net_if);

// 4. init `wifi driver`
_peripherals_err_t wifi_setup_init_wifi_driver();





// =================================== 2. Configuration phase      ===================================
// =================================== Register wifi event handler ===================================

// Define a event handler, using react when even task call
// Register auto by 'wifi_setup_regist_receive_event_task()' below
_peripherals_err_t wifi_setup_wifi_event_handler
(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

// Register 'wifi_setup_wifi_event_handler()' into event loop to check wifi status. 
_peripherals_err_t wifi_setup_regist_receive_event_task();

// Note: 
// + register ALL in one event. You write a handle function how to do when a event reached 
// event:
    // WIFI_EVENT_WIFI_READY          ?      (ignore) wifi driver ready never generate :v see espidf document
    // WIFI_EVENT_SCAN_DONE           // after scanf done
    // WIFI_EVENT_STA_START           // after `esp_wifi_start()` done
    // WIFI_EVENT_STA_STOP            // after `esp_wifi_stop()` done
    // WIFI_EVENT_STA_CONNECTED       //   [keep value] after `esp_wifi_connect()` success
    // WIFI_EVENT_STA_DISCONNECTED    //   [keep value] after got an exception failed or disconnect by 
                                // esp_wifi_disconnect(), esp_wifi_stop(), esp_wifi_deinit() 
    // IP_EVENT_STA_GOT_IP            //   [keep value] after WIFI_EVENT_STA_CONNECTED, wifi driver auto send DHCP to get new IP
    // IP_EVENT_GOT_IP6               //     (ignore) got ipv6 (:v ignore, no use)
    // IP_EVENT_STA_LOST_IP           // when the IPV4 address become invalid. wait to renew IP
    // WIFI_EVENT_AP_START            // after `esp_wifi_start()` done
    // WIFI_EVENT_AP_STOP             // after `esp_wifi_stop()` done
    // WIFI_EVENT_AP_STACONNECTED     //   [keep value] when an client connected to wifi
    // WIFI_EVENT_AP_STADISCONNECTED  //   [keep value] when client no feedback after 5 minute
                                    //  esp_wifi_disconnect() or  esp_wifi_deauth_sta()
    // WIFI_EVENT_AP_PROBEREQRECVED   ?      (ignore) when  AP receives a probe request.
    // WIFI_EVENT_STA_BEACON_TIMEOUT  ?      (ignore) connect time out to accesspoint



// =================================== 2.5 Config wifi inform      ===================================
// =================================== these functions (option)    ===================================
// =================================== only call when update NVS data ================================
// =================================== esp_wifi_set_xxx APIs        ==================================
// =================================== auto save in NVS flash       ==================================

// Note: these function may be harm if call frequency. because it write to nvs

// + choose mode:  compare old value mode with new mode, if different -> write to nvs , else return
// + wifi country: compare old country with new country, if different -> write to nvs , else return

// + wifi config:  [warn] these function do not compare, only write to nvs


// 0. choose mode for wifi driver
_peripherals_err_t wifi_setup_set_wifi_mode(wifi_mode_t mode);


// 1. Wifi country
_peripherals_err_t wifi_setup_set_wifi_country();


// 2 Wifi config
// Independent configuration for STA or AP mode
// note string input size
_peripherals_err_t wifi_setup_set_wifi_sta_config(uint8_t ssid[32] , uint8_t password[64], uint8_t bssid[6]);

_peripherals_err_t wifi_setup_set_wifi_ap_config(uint8_t ssid[32], uint8_t password[64]);





// =================================== 3 Start wifi driver         ===================================
// =================================== ==========================  ===================================

// 1. after config all inform for wifi, start wifi
_peripherals_err_t wifi_setup_start_wifi_driver();

// Note:
// this function will alloc a struct to handle state of event feed back

// Suggest: 
// + if wifi infor not set (no pass, no ssid, ...) because flash chip
// + you should write a function like 'factory reset'
// + and config default inform for wifi station and ap mode
// + then using a button , hold it 1 , 2 ,3 ... second like you want then go `factory reset`
// + `factory reset` with 4 function in step 2.5 



// =================================== 3.5 Scan wifi               ===================================
// =================================== Only for STA in (STA/ STA+AP )  ===============================
// Start scan
// note: do not scan while connect (step 4) , connect function will abort scan and return error code
_peripherals_err_t wifi_setup_start_scan_wifi();


_peripherals_err_t wifi_setup_get_wifi_list_scanned(uint16_t *number, wifi_ap_record_t *ap_records);





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