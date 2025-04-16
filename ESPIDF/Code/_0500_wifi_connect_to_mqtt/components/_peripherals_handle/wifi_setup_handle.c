/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "wifi_setup_handle.h"

 /**
 * **********************************************************
 * Variables
 * **********************************************************
 */
static struct_wifi_state_t* s_wifi_state_table = NULL;

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
_peripherals_err_t wifi_setup_init_default_event_loop()
{
    esp_err_t ret = esp_event_loop_create_default();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_CREATE_EVENT_LOOP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_CREATE_EVENT_LOOP_FAILED;
    }

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

    // set up default IP
    // Config ip
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, (WIFI_SETUP_AP_MODE_IP_ADDR >> 24) & 0xFF,
                            (WIFI_SETUP_AP_MODE_IP_ADDR >> 16) & 0xFF,
                            (WIFI_SETUP_AP_MODE_IP_ADDR >> 8)  & 0xFF,
                            WIFI_SETUP_AP_MODE_IP_ADDR & 0xFF  );        // IP address
    IP4_ADDR(&ip_info.gw, (WIFI_SETUP_AP_MODE_IP_ADDR >> 24) & 0xFF,
                            (WIFI_SETUP_AP_MODE_IP_ADDR >> 16) & 0xFF,
                            (WIFI_SETUP_AP_MODE_IP_ADDR >> 8)  & 0xFF,
                            WIFI_SETUP_AP_MODE_IP_ADDR & 0xFF  );        // Gateway     
    IP4_ADDR(&ip_info.netmask, (WIFI_SETUP_AP_MODE_NET_MASK >> 24) & 0xFF,
                            (WIFI_SETUP_AP_MODE_NET_MASK >> 16) & 0xFF,
                            (WIFI_SETUP_AP_MODE_NET_MASK >> 8)  & 0xFF,
                            WIFI_SETUP_AP_MODE_NET_MASK & 0xFF  );       // Netmask

    // apply
    esp_err_t ret = esp_netif_set_ip_info(*ap_net_if, &ip_info);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_AP_CHANGE_IP_INFORM_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_AP_CHANGE_IP_INFORM_FAILED;
    }

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

// wifi even handler
_peripherals_err_t wifi_setup_wifi_event_handler
(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    // code logic state wifi



















    
}


// Register into event loop to check wifi status
_peripherals_err_t wifi_setup_regist_receive_event_task()
{
    esp_err_t ret = esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_setup_wifi_event_handler,
        NULL,
        NULL);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_REGIST_EVENT_TASK_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_REGIST_EVENT_TASK_FAILED;
    }

    return PERIPH_OK;
}





// =================================== 2.5 Config wifi inform      ===================================
// =================================== Warn: now using NVS flash   ===================================
// =================================== becareful frequency call thes function can harm flash   =======
// 0. choose mode for wifi driver
_peripherals_err_t wifi_setup_set_wifi_mode(wifi_mode_t mode)
{
    wifi_mode_t old_mode = WIFI_MODE_NULL;

    esp_err_t ret = esp_wifi_get_mode(&old_mode);
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_GET_WIFI_MODE_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_GET_WIFI_MODE_FAILED;
    }

    if(old_mode == mode)
    {
        return PERIPH_OK; // ok no write to nvs :) no thing changed
    }

    // debug
    #if CONFIG_DEBUG_ENABLE !=0
    fprintf(stderr, "\nStart write [mode] to nvs\n", (void*)old_country_code, old_country_code);
    #endif

    // if oldmode != expected mode
    ret = esp_wifi_set_mode(mode);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_SET_WIFI_MODE_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_SET_WIFI_MODE_FAILED;
    }

    return PERIPH_OK;
}

// 1. Wifi country
_peripherals_err_t wifi_setup_set_wifi_country()
{
    char old_country_code[4] = {0};

    // debug
    #if CONFIG_DEBUG_ENABLE !=0
    fprintf(stderr, "\nCheck country string - old address = [%p] | [%s] \n", (void*)old_country_code, old_country_code);
    #endif

    esp_err_t ret = esp_wifi_get_country_code(old_country_code);

    // debug
    
    #if CONFIG_DEBUG_ENABLE !=0
    fprintf(stderr, "\nCheck country string - new address = [%p] | [%s] \n", (void*)old_country_code, old_country_code);
    #endif

    if(! strcmp(old_country_code, WIFI_SETUP_SET_DEFAULT_COUNTRY_CODE))
    {
        return PERIPH_OK;
    }

    // debug
    #if CONFIG_DEBUG_ENABLE !=0
    fprintf(stderr, "\nStart write [country] to nvs\n", (void*)old_country_code, old_country_code);
    #endif

    // esp_wifi_set_country(); // this function not encourage because it not check valid infor input
    // using esp_wifi_set_country_code() instead
    ret = esp_wifi_set_country_code(WIFI_SETUP_SET_DEFAULT_COUNTRY_CODE,
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

_peripherals_err_t wifi_setup_set_wifi_sta_config(uint8_t ssid[32] , uint8_t password[64], uint8_t bssid[6])
{

    // init
    wifi_config_t conf = {0};  // full clean init wifi config 0

    strncpy((char*)conf.sta.ssid, (const char*)ssid, sizeof(conf.sta.ssid) - 1);
    conf.sta.ssid[sizeof(conf.sta.ssid) - 1] = '\0';
    
    strncpy((char*)conf.sta.password, (const char*)password, sizeof(conf.sta.password) - 1);
    conf.sta.password[sizeof(conf.sta.password) - 1] = '\0';
    
    memcpy(conf.sta.bssid, bssid, sizeof(conf.sta.bssid));

    conf.sta.bssid_set = true; // using bssid cho connect with ssid and password

    conf.sta.threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD; // this config affect on scan wifi, all wifi have less
    // sercure than default authentication setup in menuconfig will be ignore
    // it still be using choose wifi destination
    
    // config
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
    // init
    wifi_config_t conf = {0};

    strncpy((char*)conf.ap.ssid, (const char*)ssid, sizeof(conf.ap.ssid) - 1);
    conf.ap.ssid[sizeof(conf.sta.ssid) - 1] = '\0';

    conf.ap.ssid_len = strlen(conf.ap.ssid);

    strncpy((char*)conf.ap.password, (const char*)password, sizeof(conf.ap.password) - 1);
    conf.ap.password[sizeof(conf.sta.password) - 1] = '\0';

    conf.ap.channel = WIFI_SETUP_WIFI_CONFIG_AP_CHANNEL_DEFAULT;
    conf.ap.max_connection = WIFI_SETUP_WIFI_CONFIG_AP_MAX_CONN;
    conf.ap.authmode = WIFI_SETUP_WIFI_CONFIG_AP_AUTHEN_MODE;

    // config
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
// Note:
// this function will alloc a struct to handle state of event feed back
_peripherals_err_t wifi_setup_start_wifi_driver()
{
    // check state table

    if(s_wifi_state_table!=NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_NOT_NULL, __FILE__, __LINE__, "wifi state table not NULL");
        #endif
        return WIFI_SETUP_WIFI_STATE_TABLE_NOT_NULL;
    }

    // create state table
    s_wifi_state_table = (struct_wifi_state_t*)calloc(1, sizeof(struct_wifi_state_t));

    if(s_wifi_state_table == NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_INIT_FAILED, __FILE__, __LINE__, "wifi state table not NULL");
        #endif
        return WIFI_SETUP_WIFI_STATE_TABLE_INIT_FAILED;
    }

    // init default mutex key
    s_wifi_state_table->wifi_state_mutex = xSemaphoreCreateMutex();
    s_wifi_state_table->sta_collision_conn_scan_mutex = xSemaphoreCreateMutex();

    if( (s_wifi_state_table->wifi_state_mutex == NULL) || 
    s_wifi_state_table->sta_collision_conn_scan_mutex == NULL)
    {
        free(s_wifi_state_table);
        s_wifi_state_table = NULL;

        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_CREATE_MUTEX_FAILED, __FILE__, __LINE__, "wifi state table not NULL");
        #endif
        return WIFI_SETUP_WIFI_STATE_TABLE_CREATE_MUTEX_FAILED;
    }

    // wifi start
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
        .bssid = WIFI_SETUP_WIFI_SCAN_BSSID,
        .ssid  = WIFI_SETUP_WIFI_SCAN_SSID,
        .channel = WIFI_SETUP_WIFI_SCAN_HIDEN_WF,
        .scan_type = WIFI_SETUP_WIFI_SCAN_TYPE,
        .scan_time.active=
        {
            .min = WIFI_SETUP_WIFI_ACTIVE_SCAN_MIN_TIME_MS,
            .max = WIFI_SETUP_WIFI_ACTIVE_SCAN_MAX_TIME_MS
        }
    };

    // set scan authen mode

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

    // free state struct
    if(s_wifi_state_table == NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_IS_NULL, __FILE__, __LINE__, "can't free NULL table");
        #endif
        return WIFI_SETUP_WIFI_STATE_TABLE_IS_NULL;
    }

    if(s_wifi_state_table->sta_dest_ap_connected != NULL) free(s_wifi_state_table->sta_dest_ap_connected);

    for(int i = 0; i < WIFI_SETUP_WIFI_CONFIG_AP_MAX_CONN; i++)
    {
        if(s_wifi_state_table->ap_list_sta_connected[i] != NULL) free(s_wifi_state_table->ap_list_sta_connected[i]);
    }

    free(s_wifi_state_table);
    s_wifi_state_table = NULL;

    // ok
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
// =================================== Delele/free below case if u want but keep be mind before do ===

// delete wifi interface if need

// free loop / warn that default loop using for vary perpose, not only wifi/ip

// clear LwIP if need

// these behind extend case can auto clear after reset chip

