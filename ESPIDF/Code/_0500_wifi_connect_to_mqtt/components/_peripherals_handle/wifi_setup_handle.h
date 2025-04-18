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
#include "esp_wifi_types.h"
#include "esp_event.h"

#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"


//user
#include "_peripherals_err.h"

// ap mode ip
#define WIFI_SETUP_AP_MODE_IP_ADDR         ( (192 << 24) | (168 << 16) | (1<<8)   | (1) )
#define WIFI_SETUP_AP_MODE_NET_MASK        ( (255 << 24) | (255 << 16) | (255<<8) | (0) ) // C class network by classful

// country set infor
#define WIFI_SETUP_SET_DEFAULT_COUNTRY_CODE    "01 " // world safe mode
                                                     // note: alway have third octet
                                                    // rule country code :
                                                    //
                                                    // Supported country codes are "01"(world safe mode) "AT","AU","BE","BG","BR", "CA","CH","CN","CY","CZ","DE","DK","EE","ES","FI","FR","GB","GR","HK","HR","HU", "IE","IN","IS","IT","JP","KR","LI","LT","LU","LV","MT","MX","NL","NO","NZ","PL","PT", "RO","SE","SI","SK","TW","US"
                                                    // When country code "01" (world safe mode) is set, SoftAP mode won't contain country IE.
                                                    // The default country is "CN" and ieee80211d_enabled is TRUE.
                                                    // The third octet of country code string is one of the following: ' ', 'O', 'I', 'X', otherwise it is considered as ' '.
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
#define WIFI_SETUP_WIFI_ACTIVE_SCAN_MAX_TIME_MS (200) // maximum time wait feedback. esp32 limit 1500ms
#define WIFI_SETUP_WIFI_SCAN_BLOCK     (false) // wifi scan stop scan when be requested


// struct save state of wifi
// when handle wifi, sometime race condition can be reach
// manage state of wifi by this struct more suitable than statemachine
// any query this state struct will accept by specicals function for sure not race condition
// this struct auto alloc when call 'wifi_setup_start_wifi_driver();' and init all field is 0
// and de alloc when call "wifi_setup_stop_wifi_driver();"

typedef struct
{
    // instance handler using register event task
    esp_event_handler_instance_t wifi_handler;
    esp_event_handler_instance_t ip_handler;

    // Mutex to protect state from race conditions,
    // since any event task can come at random time.
    // Helps ensure event handling is serialized.
    SemaphoreHandle_t wifi_state_mutex;  // sure that any event task call the wifi event handle will queuing

    // some harm action can avoid by flag:
    // + sta scan + sta connect
    // + sta scan + get list ap
    // + multiple calling function at the same time
    // + avoid call special function when wrong state:
        // no connect to ap when connecting or connected or scanning
        // no scan ap when scanning or scanned or connecting
        // no list ap when scanning or not scanned
        // no start ap driver when ap starting or ap started

    // state / using theses 'flags' instead of 'enum', because multiple states can occur simultaneously
    // can using a uint32_t and define macro to save RAM but write down like this easy to implement 
    
    // uint8_t sta_started;    // - Raised by WIFI_EVENT_STA_START, cleared by WIFI_EVENT_STA_STOP
                            // - When raised: clear sta_starting
                            // - When cleared: clear driver_stopping
                            
                            // + notify for upper layer - 'Application' - access point mode is start / stop 

    // uint8_t sta_starting;   // - Raise by : wifi_setup_start_wifi_driver() at step 3
                            // - Note: check sta_start before raise

                            // + check to avoid call wifi_setup_start_wifi_driver() multiple

    // uint8_t sta_connected;  // - Raised by WIFI_EVENT_STA_CONNECTED, cleared by WIFI_EVENT_STA_DISCONNECTED
                            // - When raised: clear sta_connecting

                            // + notify for upper layer - 'Application' - station mode is connected or not to access point 

    // uint8_t sta_connecting; // - Raise by wifi_setup_connect_to_access_point() step 4
                            // - Note: check sta_connecting, sta_connect and sta_scanning before raise                   

                            // + check to avoid call wifi_setup_connect_to_access_point() multiple
                            // + notify for ' wifi_setup_start_scan_wifi()' 
                            
    // uint8_t sta_disconnecting;
                            // - Raise by : wifi_setup_disconnect_wifi() at step 8
                            // - Note: check sta_disconnecting and sta_connected

                            // + check to avoide call wifi_setup_disconnect_wifi() multiple

    // uint8_t sta_scanning;   // - Raise by : wifi_setup_start_scan_wifi() at step 3.5
                            // - Note: check sta_connecting, sta_connecting and sta_scanned before

                            // + check to avoid call wifi_setup_start_scan_wifi() multiple
                            // + notify for 'wifi_setup_connect_to_access_point()' at step 4 
                            // + notify for ' wifi_setup_get_wifi_list_scanned() ' scanning, can't read

    
    // uint8_t sta_scanned;    // - Raise by WIFI_EVENT_SCAN_DONE
                            // - When raised: clear sta_scanning

                            // + notify for 'wifi_setup_start_scan_wifi()' should not start to avoid memory leak when not read
                            // + notify for 'wifi_setup_get_wifi_list_scanned()' at step 3,5 can read

                            // - Only clear by 'wifi_setup_get_wifi_list_scanned()' if can read
    
    // uint8_t sta_got_ip;     // - Raise when IP_EVENT_STA_GOT_IP and clear when IP_EVENT_STA_LOST_IP

                            // + notify for upper layer - 'Application' - IP is available or loss

    // uint8_t ap_started;     // - Raise when WIFI_EVENT_AP_START and clear when WIFI_EVENT_AP_STOP
                            // - When raised: clear ap_starting

                            // + notify for upper layer - 'Application' - access point mode is start / stop

    // uint8_t ap_starting;    // - Raise by : wifi_setup_start_wifi_driver() at step 3
                            // - Note: check sta_start before raise

                            // + check to avoid call wifi_setup_start_wifi_driver() multiple
    
    uint8_t ap_number_sta_connected;
                            // - This state count up when WIFI_EVENT_AP_STACONNECTED
                            // - count down when WIFI_EVENT_AP_STADISCONNECTED

                            // + notify for upper layer - 'Application' - some client is connected/ disconnect to access point
                            // + handle upper layer sockets

    // uint8_t driver_stopping;
                            // - Raise by : wifi_setup_stop_wifi_driver() at step 8
                            // - Note: check driver_stopping, sta_started before raise

                            // + check to avoid call wifi_setup_stop_wifi_driver() multiple
    
    // All flag behind package in one register 32 bit
    uint32_t wifi_ip_state;

    // struct pointer to data
    wifi_event_ap_staconnected_t* ap_list_sta_connected[WIFI_SETUP_WIFI_CONFIG_AP_MAX_CONN]; 
            // pointer to NULL if no connect, or pointer to
            // struct pointer to copy value feedback of WIFI_EVENT_AP_STACONNECTED event
            // if a pointer exist when WIFI_EVENT_AP_STADISCONNECTED raise, it will free and point to NULL
    wifi_event_sta_connected_t* sta_dest_ap_connected;
            // default NULL if not connect. 
            // copy value feedback of WIFI_EVENT_STA_CONNECTED event
            // it will free and point to NULL if WIFI_EVENT_STA_DISCONNECTED

    
} struct_wifi_state_t;

// some function join synchronous will be feedback by REFUSED or EXECUTED
// user can know state machine is busy to handle input command or can handle instantly
#define WIFI_SETUP_COMMAND_REFUSED  (0)
#define WIFI_SETUP_COMMAND_EXECUTED (1)


// reduce using RAM, using register 32 bit to save state flags
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED         (0)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING        (1)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED       (2)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING      (3)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING   (4)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING        (5)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNED         (6)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_GOT_IP          (7)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTED          (8)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTING         (9)
#define WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING     (10)





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
// =================================== 0. option                   ===================================
// =================================== NVS setup, event loop setup ===================================
// 0. Wifi start NVS memory before go to step 2
// check the box:
// Compiler config -> Wi-Fi -> WiFi NVS flash 
_peripherals_err_t wifi_setup_init_nvs_flash();



// 1. default event loop
// this function create default event loop for wifi
// no encourage stop event loop default when was created
// we no suply destroy event loop default function, it not safe :v 
// because the default loop can be used by lot of function, not only wifi
_peripherals_err_t wifi_setup_create_default_event_loop();


// =================================== 1. Init phase               ===================================
// =================================== =========================== ===================================
// 1. Init lightweight ip to handle TCP/IP stack
_peripherals_err_t wifi_setup_init_lightweight_ip_inform();


// 2. create instance Wifi interface connect to LwIP
// when using Station mode or STA+AP mode
_peripherals_err_t wifi_setup_create_interface_wifi_STA_link_LwIP(esp_netif_t** sta_net_if);

// when using AP mode or STA+AP mode , change ip, netmask
_peripherals_err_t wifi_setup_create_interface_wifi_AP_link_LwIP(esp_netif_t** ap_net_if);

// 3. init `wifi driver`
_peripherals_err_t wifi_setup_init_wifi_driver();





// =================================== 2. Configuration phase      ===================================
// =================================== Register wifi event handler ===================================

// Define a event handler, using react when even task call
// Register auto by 'wifi_setup_regist_receive_event_task()' below
void wifi_setup_wifi_event_handler
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
// config struct_wifi_state_t to synchronous
// from this time, function below will be synchronous by struct_wifi_state_t
_peripherals_err_t wifi_setup_start_wifi_driver(uint8_t *busy);

// Note:
// this function will alloc a struct to handle state of event feedback

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
_peripherals_err_t wifi_setup_start_scan_wifi(uint8_t *busy);

_peripherals_err_t wifi_setup_get_wifi_list_scanned(uint8_t* busy, uint16_t *number, wifi_ap_record_t *ap_records);





// =================================== 4. Wifi connect phase       ===================================
// =================================== Only for STA in (STA/ STA+AP ) ================================
// 1. connect to wifi
_peripherals_err_t wifi_setup_connect_to_access_point(uint8_t* busy);





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
_peripherals_err_t wifi_setup_disconnect_wifi(uint8_t* busy);



// Stop wifi driver
_peripherals_err_t wifi_setup_stop_wifi_driver(uint8_t* busy);


// unregister event loop
// clear struct table
_peripherals_err_t wifi_setup_unregister_event_task();

// de init wifi driver
_peripherals_err_t wifi_setup_de_init_wifi_driver();


// =================================== Extend                      ===================================
// =================================== ==========================  ===================================

// delete wifi interface
// clear LwIP

// this case can auto clear after reset chip



#endif