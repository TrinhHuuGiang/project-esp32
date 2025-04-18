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
    uint8_t wifi_busy = WIFI_SETUP_COMMAND_EXECUTED;

    // setup wifi driver
    if(wifi_setup_create_default_event_loop()) return;
    if(wifi_setup_init_lightweight_ip_inform()) return;
    if(wifi_setup_create_interface_wifi_STA_link_LwIP(&sta_net_if)) return;
    if(wifi_setup_create_interface_wifi_AP_link_LwIP(&ap_net_if)) return;
    if(wifi_setup_init_wifi_driver()) return;

    // regist event notify
    if(wifi_setup_regist_receive_event_task()) return;
    
    // set config STA+AP mode
    if(wifi_setup_set_wifi_mode(WIFI_MODE_APSTA)) return;
    if(wifi_setup_set_wifi_country()) return;

    // Wifi driver start
    if(wifi_setup_start_wifi_driver()) return;
    if(wifi_setup_connect_to_access_point(&wifi_busy)) return;

    // continue program while wifi driver handle event

    

    return;
}
