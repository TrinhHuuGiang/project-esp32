/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "wifi_setup_handle.h"


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// =================================== 1. Init phase               ===================================
// =================================== =========================== ===================================
// 1. Init lightweight ip to handle TCP/IP stack
_peripherals_err_t wifi_setup_init_lightweight_ip_inform()
{
    esp_err_t ret = esp_netif_init();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_INIT_NETIF_LWIP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_INIT_NETIF_LWIP_FAILED;
    }

    return PERIPH_OK;

}


// 2. init event loop for `event task`
_peripherals_err_t wifi_setup_init_event_loop(esp_event_loop_handle_t* even_task_loop_handle)
{
    if(*even_task_loop_handle != NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_INPUT_EVENT_HANDLE_NOT_NULL, __FILE__, __LINE__, "even task handle input not NULL");
        #endif
        return WIFI_SETUP_INPUT_EVENT_HANDLE_NOT_NULL;
    }

    esp_event_loop_args_t event_loop_args =
    {
        .task_name       = WIFI_SETUP_EVENT_TASK_LOOP_NAME,
        .queue_size      = WIFI_SETUP_EVENT_TASK_QUEUE_SIZE,
        .task_stack_size = WIFI_SETUP_EVENT_TASK_STACK_SIZE,
        .task_priority   = WIFI_SETUP_EVENT_TASK_PRIORITY,
        .task_core_id    = WIFI_SETUP_EVENT_CORE_BIND_ID
    };
    esp_event_loop_create_default();
    
    esp_event_loop_handle_t even_task_loop_temp = NULL;

    esp_err_t ret = esp_event_loop_create(&event_loop_args, &even_task_loop_temp);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_CREATE_EVENT_LOOP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_CREATE_EVENT_LOOP_FAILED;
    }

    *even_task_loop_handle = even_task_loop_temp;

    return PERIPH_OK;
}


// 3. create instance Wifi interface connect to LwIP
// when using Station mode or STA+AP mode
_peripherals_err_t wifi_setup_create_interface_wifi_STA_link_LwIP(esp_netif_t** sta_net_if)
{
    if(*sta_net_if != NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_STA_NETIF_INPUT_NOT_NULL, __FILE__, __LINE__, "STA create net interface failed");
        #endif
        return WIFI_SETUP_STA_NETIF_INPUT_NOT_NULL;
    }

    *sta_net_if = esp_netif_create_default_wifi_sta(); // this API will call abort to stop program if error

    return PERIPH_OK;
}


// when using AP mode or STA+AP mode
_peripherals_err_t wifi_setup_create_interface_wifi_AP_link_LwIP(esp_netif_t** ap_net_if)
{
    if(*ap_net_if != NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_AP_NETIF_INPUT_NOT_NULL, __FILE__, __LINE__, "AP create net interface failed");
        #endif
        return WIFI_SETUP_AP_NETIF_INPUT_NOT_NULL;
    }

    *ap_net_if = esp_netif_create_default_wifi_ap(); // this API will call abort to stop program if error

    return PERIPH_OK;
}



// 4. init `wifi driver`
_peripherals_err_t wifi_setup_init_wifi_driver()
{
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

    esp_err_t ret = esp_wifi_init(&wifi_init_config);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_INIT_WIFI_DRIVER_CONFIG_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_INIT_WIFI_DRIVER_CONFIG_FAILED;
    }

    return PERIPH_OK;
}





// =================================== 2. Configuration phase      ===================================
// =================================== =========================== ===================================

// Register into event loop to check wifi status
_peripherals_err_t wifi_setup_regist_receive_event_task()
{

}





// =================================== 2.5 Config wifi inform      ===================================
// =================================== ==========================  ===================================
// 0. choose mode for wifi driver
_peripherals_err_t wifi_setup_set_wifi_mode(wifi_mode_t mode)
{
    esp_err_t ret = esp_wifi_set_mode(mode);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_CHOOSE_WIFI_MODE_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_CHOOSE_WIFI_MODE_FAILED;
    }

    return PERIPH_OK;
}

// 1. Wifi country
_peripherals_err_t wifi_setup_set_wifi_country()
{
    // esp_wifi_set_country(); // this function not encourage because it not check valid infor input
    // using esp_wifi_set_country_code() instead
    esp_err_t ret = esp_wifi_set_country_code(WIFI_SETUP_SET_DEFAULT_COUNTRY_CODE,
        WIFI_SETUP_CHOOSE_AUTO_FIX_COUNTRY); // do not support code VN == vietnam, so we can use "01"(world safe mode) 

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_SET_COUNTRY_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_SET_COUNTRY_FAILED;
    }

    return PERIPH_OK;
}

// 2. Wifi config

_peripherals_err_t wifi_setup_set_wifi_sta_config(uint8_t ssid[32] , uint8_t password[64], uint8_t bssid[6], bool bssid_enable)
{ 
    wifi_config_t conf = {
        .sta = { // maximum str_len 31 for ssid, and 63 for password
            .ssid = ssid,                                   // ssid     << string , because if be array, 
                                                            // system will handle whole 32 byte
                                                            // and sure that read overlength -> error
            .password = password,                           // password << string 
            .bssid = bssid,                                 // BSSID (MAC router) << array
            .bssid_set = bssid_enable,                      // enable check BSSID
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD       // select authentication mode
        }
    };

    esp_err_t ret = esp_wifi_set_config(WIFI_IF_STA , &conf);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_SET_WIFI_STA_CONFIG_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_SET_WIFI_STA_CONFIG_FAILED;
    }

    return PERIPH_OK;

}

_peripherals_err_t wifi_setup_set_wifi_ap_config(uint8_t ssid[32], uint8_t password[64])
{
    wifi_config_t conf = {
        .ap = {
            .ssid = ssid,
            .ssid_len = strlen(ssid),
            .channel = WIFI_SETUP_WIFI_CONFIG_AP_CHANNEL_DEFAULT,
            .password = password,
            .max_connection = WIFI_SETUP_WIFI_CONFIG_AP_MAX_CONN,
            .authmode = WIFI_SETUP_WIFI_CONFIG_AP_AUTHEN_MODE
        },
    };

    esp_err_t ret = esp_wifi_set_config(WIFI_IF_AP , &conf);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_SET_WIFI_AP_CONFIG_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_SET_WIFI_AP_CONFIG_FAILED;
    }

    return PERIPH_OK;
}



// =================================== 3 Start wifi driver         ===================================
// =================================== ==========================  ===================================

// 1. after config all inform for wifi, start wifi
_peripherals_err_t wifi_setup_start_wifi_driver()
{
    
    esp_err_t ret = esp_wifi_start();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_START_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_WIFI_START_FAILED;
    }

    return PERIPH_OK;
}




// =================================== 3.5 Scan wifi               ===================================
// =================================== Only for STA in (STA/ STA+AP )  ===============================
// Start scan
_peripherals_err_t wifi_setup_start_scan_wifi()
{
    wifi_scan_config_t wifi_scan_config =
    {
        .bssid = WIFI_SETUP_WIFI_SCAN_SSID,
        .ssid  = WIFI_SETUP_WIFI_SCAN_BSSID,
        .channel = WIFI_SETUP_WIFI_SCAN_HIDEN_WF,
        .scan_type = WIFI_SETUP_WIFI_SCAN_TYPE,
        .scan_time.active=
        {
            .min = WIFI_SETUP_WIFI_ACTIVE_SCAN_MIN_TIME_MS,
            .max = WIFI_SETUP_WIFI_ACTIVE_SCAN_MAX_TIME_MS
        }
    };

    esp_err_t ret = esp_wifi_scan_start(&wifi_scan_config , WIFI_SETUP_WIFI_SCAN_BLOCK);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_SCAN_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_WIFI_SCAN_FAILED;
    }

    return PERIPH_OK;
}


_peripherals_err_t wifi_setup_get_wifi_list_scanned(uint16_t *number, wifi_ap_record_t *ap_records)
{
    // get ap list size

    esp_err_t ret = esp_wifi_scan_get_ap_num(number);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_GET_NUMBER_AP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_GET_NUMBER_AP_FAILED;
    }

    // get ap list
    ret = esp_wifi_scan_get_ap_records(number, ap_records);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_GET_AP_LIST_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_GET_AP_LIST_FAILED;
    }

    return PERIPH_OK;

}





// =================================== 4. Wifi connect phase       ===================================
// =================================== Only for STA in (STA/ STA+AP ) ================================
// 1. connect to wifi
_peripherals_err_t wifi_setup_connect_to_access_point()
{
    
    esp_err_t ret = esp_wifi_connect();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_CONNECT_TO_WIFI_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_CONNECT_TO_WIFI_FAILED;
    }

    return PERIPH_OK;

}





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
_peripherals_err_t wifi_setup_disconnect_wifi()
{
    esp_err_t ret = esp_wifi_disconnect();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_DISCONNECT_TO_WIFI_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_DISCONNECT_TO_WIFI_FAILED;
    }

    return PERIPH_OK;
    
}



// Stop wifi driver
_peripherals_err_t wifi_setup_stop_wifi_driver()
{

    esp_err_t ret = esp_wifi_stop();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_STOP_WIFI_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_STOP_WIFI_FAILED;
    }

    return PERIPH_OK;
}




// de init wifi driver
_peripherals_err_t wifi_setup_de_init_wifi_driver()
{

    esp_err_t ret = esp_wifi_deinit();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_DE_INIT_WIFI_DRIVER_CONFIG_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_DE_INIT_WIFI_DRIVER_CONFIG_FAILED;
    }

    return PERIPH_OK;
}


// =================================== Extend                      ===================================
// =================================== ==========================  ===================================

// delete wifi interface
// free loop
// clear LwIP

// this case can auto clear after reset chip

