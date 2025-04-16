/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "main.h"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */


/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
// wifi sta + ap handle
static void wifi_ap_sta_mode_event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data);

// wifi factory reset
// then set flag.
// a function handle reset will wait all done and reset chip ( using semaphore )

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

void app_main(void)
{
    esp_netif_t *sta_net_if = NULL;
    esp_netif_t *ap_net_if = NULL;

    // setup wifi driver
    if(wifi_setup_init_lightweight_ip_inform()) return;
    if(wifi_setup_init_default_event_loop()) return;
    if(wifi_setup_create_interface_wifi_STA_link_LwIP(&sta_net_if)) return;
    if(wifi_setup_create_interface_wifi_AP_link_LwIP(&ap_net_if)) return;
    if(wifi_setup_init_wifi_driver()) return;

    // regist event notify
    if(wifi_setup_regist_receive_event_task(wifi_ap_sta_mode_event_handler)) return;
    
    // set config STA+AP mode
    if(wifi_setup_set_wifi_mode(WIFI_MODE_APSTA)) return;
    if(wifi_setup_set_wifi_country()) return;

    // Wifi driver start
    if(wifi_setup_start_wifi_driver()) return;
    if(wifi_setup_connect_to_access_point()) return;

    // continue program while wifi driver handle event

    return;
}

// wifi sta + ap handle
static void wifi_ap_sta_mode_event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    // WIFI_EVENT_WIFI_READY          ?  (ignore) wifi driver ready never generate :v see espidf document
    // WIFI_EVENT_SCAN_DONE           // after scanf done
    // WIFI_EVENT_STA_START           // after `esp_wifi_start()` done
    // WIFI_EVENT_STA_STOP            // after `esp_wifi_stop()` done
    // WIFI_EVENT_STA_CONNECTED       // after `esp_wifi_connect()` success
    // WIFI_EVENT_STA_DISCONNECTED    // after got an exception failed or disconnect by 
                                // esp_wifi_disconnect(), esp_wifi_stop(), esp_wifi_deinit() 
    // IP_EVENT_STA_GOT_IP            // after WIFI_EVENT_STA_CONNECTED, wifi driver auto send DHCP to get new IP
    // IP_EVENT_GOT_IP6               // (ignore) got ipv6 (:v ignore, no use)
    // IP_EVENT_STA_LOST_IP           // (ignore) when the IPV4 address become invalid. wait to renew IP
    // WIFI_EVENT_AP_START            // after `esp_wifi_start()` done
    // WIFI_EVENT_AP_STOP             // after `esp_wifi_stop()` done
    // WIFI_EVENT_AP_STACONNECTED     // when an client connected to wifi
    // WIFI_EVENT_AP_STADISCONNECTED  // when client no feedback after 5 minute
                                    //  esp_wifi_disconnect() or  esp_wifi_deauth_sta()
    // WIFI_EVENT_AP_PROBEREQRECVED   ?  (ignore) when  AP receives a probe request.
    // WIFI_EVENT_STA_BEACON_TIMEOUT  ?  (ignore) connect time out to accesspoint

    // sử dụng mutex cho hàm handler và viết riêng nó ra.
    // sử dụng 

    // send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "");
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        /* code */
        break;
    
    default:
        break;
    }

}