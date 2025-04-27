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
extern __task_sync_t* g_task_sync_tools; // get sync tools 

static struct_wifi_state_t* s_wifi_state_table = NULL;

 /**
 * **********************************************************
 * Prototype
 * **********************************************************
 */
// wifi even handler
static void wifi_setup_wifi_event_handler
(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

// compare client id, support event task handler check when disconnect with client
static int compare_client_id(ap_list_client_connected_t* a, wifi_event_ap_stadisconnected_t* b);

 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// =================================== 0. option                   ===================================
// =================================== NVS setup, event loop setup ===================================
// 0. Wifi start NVS memory before go to step 2
// check the box:
// Compiler config -> Wi-Fi -> WiFi NVS flash 
_peripherals_err_t wifi_setup_init_nvs_flash()
{
    esp_err_t ret = nvs_flash_init();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_INIT_NVS_FLASH_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_INIT_NVS_FLASH_FAILED;
    }
    
    return PERIPH_OK;
}

// 1. default event loop
// this function create default event loop for wifi
_peripherals_err_t wifi_setup_create_default_event_loop()
{
    esp_err_t ret = esp_event_loop_create_default();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_CREATE_DEFAULT_EVENT_LOOP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return WIFI_SETUP_CREATE_DEFAULT_EVENT_LOOP_FAILED;
    }

    return PERIPH_OK;
}





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


// 2. create instance Wifi interface connect to LwIP
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

    
    // create default ap interface
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
    esp_err_t ret1 = esp_netif_dhcps_stop(*ap_net_if);

    esp_err_t ret2 = esp_netif_set_ip_info(*ap_net_if, &ip_info);

    esp_err_t ret3 = esp_netif_dhcps_start(*ap_net_if);

    if(ret1!=ESP_OK   || ret2!=ESP_OK  || ret3!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_AP_CHANGE_IP_INFORM_FAILED, __FILE__, __LINE__, 
            esp_err_to_name(ret1!=ESP_OK ? ret1 : (ret2!=ESP_OK ? ret2 : ret3) ));
        #endif
        return WIFI_SETUP_AP_CHANGE_IP_INFORM_FAILED;
    }

    // ok
    return PERIPH_OK;
}



// 3. init `wifi driver`
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
static void wifi_setup_wifi_event_handler
(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    // block all other task
    xSemaphoreTake(g_task_sync_tools->wifi_state_mutex, portMAX_DELAY);

    // check manager null
    // dont care all event because wifi not start
    // because each event will callback this handler in new task
    // each task may be blocked and queuing by wifi mutex
    // if we call "wifi_setup_unregister_event_task" the driver , event access NULL pointer can crash program
    if(s_wifi_state_table == NULL)
    {
        xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
        return;
    }

    // event from wifi driver
    if(event_base == WIFI_EVENT)
    {

        // --------------- STA event 
        switch (event_id)
        {
            case WIFI_EVENT_STA_START: // done sta start

                // + raise sta_started
                SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED);

                // + clear sta_starting
                CLR_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING);

                break;
            
            case WIFI_EVENT_STA_STOP: // done sta stop
                
                // + clear sta_stared
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED);

                // + clear driver_stopping
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING);

                break;

            case WIFI_EVENT_STA_CONNECTED: // done connect

                // + raise sta_connected
                SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED);

                // + clear sta_connecting
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING);

                // get inform network (only use for diagnose network connected)
                s_wifi_state_table->sta_dest_ap_connected = calloc(1, sizeof(wifi_event_sta_connected_t));

                // if calloc failed, ignore it
                if(s_wifi_state_table->sta_dest_ap_connected == NULL)
                {
                    #if CONFIG_DEBUG_ENABLE != 0
                    send_peripheral_err_location(WIFI_SETUP_STA_ADD_AP_INF_FAILED, __FILE__, __LINE__, "calloc failed for sta_dest_ap_connected");
                    #endif
                    break;
                }

                memcpy(s_wifi_state_table->sta_dest_ap_connected, event_data, sizeof(wifi_event_sta_connected_t));

                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                
                // + clear sta_connected
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED);

                // + clear sta_connecting
                // when connect failed, WIFI_EVENT_STA_CONNECTED event will not comming
                // WIFI_EVENT_STA_DISCONNECTED must clear if want continue using STA connect function
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING);

                // + clear sta_disconnecting
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING);

                // clear net connected
                if(s_wifi_state_table->sta_dest_ap_connected == NULL)
                {
                    #if CONFIG_DEBUG_ENABLE != 0
                    send_peripheral_err_location(WIFI_SETUP_STA_REMOVE_AP_INF_FAILED, __FILE__, __LINE__, "No find access point inform");
                    #endif
                    break;
                }
                
                free(s_wifi_state_table->sta_dest_ap_connected);
                
                s_wifi_state_table->sta_dest_ap_connected = NULL;

                break;
                
            case WIFI_EVENT_SCAN_DONE:
                
                // + raise sta_scanned
                SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNED);

                // + clear sta_scanning
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING);

                break;
            

            // --------------- AP event 
            case WIFI_EVENT_AP_START:// done ap start

                // + raise sta_started 
                SET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTED);

                // + clear ap_starting
                CLR_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTING);

                break;

            case WIFI_EVENT_AP_STOP:
                
                // + clear sta_stared
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTED);

                // + clear driver_stopping
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING);

                break;
            
            case WIFI_EVENT_AP_STACONNECTED:

                // + count up number connected
                s_wifi_state_table->ap_number_sta_connected ++;

                // + add new ap to list
                ap_list_client_connected_t* new_client = calloc(1, sizeof(ap_list_client_connected_t));

                // if calloc failed, ignore it
                if(new_client == NULL)
                {
                    #if CONFIG_DEBUG_ENABLE != 0
                    send_peripheral_err_location(WIFI_SETUP_AP_ADD_CLIENT_INF_TO_LIST_FAILED, __FILE__, __LINE__, "calloc failed for new_client");
                    #endif
                    break;
                }

                memcpy(&(new_client->client_connected_inf), event_data, sizeof(wifi_event_ap_staconnected_t));

                LL_PREPEND(s_wifi_state_table->ap_list_client_connected,new_client);

                // printf notify new aid
                #if CONFIG_DEBUG_ENABLE !=0
                fprintf(stderr, "\nAp mode - new AID [%u]\n", ((wifi_event_ap_staconnected_t*)event_data)->aid);
                #endif

                break;
            
            case WIFI_EVENT_AP_STADISCONNECTED:
                // + count down number connected
                s_wifi_state_table->ap_number_sta_connected --;

                // + kick ap out of list
                ap_list_client_connected_t* rm_client = NULL;

                // find node to delete
                LL_SEARCH(s_wifi_state_table->ap_list_client_connected, rm_client, 
                    (wifi_event_ap_stadisconnected_t*)event_data, compare_client_id);
                
                // if remove expect no appear in client list so ignore
                if(rm_client == NULL)
                {
                    #if CONFIG_DEBUG_ENABLE != 0
                    send_peripheral_err_location(WIFI_SETUP_AP_REMOVE_CLIENT_INF_TO_LIST_FAILED, __FILE__, __LINE__, "client disappeared form connected list");
                    #endif
                    break;
                }

                // delete node
                LL_DELETE(s_wifi_state_table->ap_list_client_connected, rm_client);

                free(rm_client);
                
                // printf notify delete aid
                #if CONFIG_DEBUG_ENABLE !=0
                fprintf(stderr, "\nAP mode - free AID [%u]\n", ((wifi_event_ap_stadisconnected_t*)event_data)->aid);
                #endif

                break;

            default:
                break;
        }

    }

    // event from LwIP
    else if(event_base == IP_EVENT)
    {
        switch (event_id)
        {
            case IP_EVENT_STA_GOT_IP:
                
                // got IP
                SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_GOT_IP);

                break;
    
            case IP_EVENT_STA_LOST_IP:

                // lost IP
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_GOT_IP);

                break;
        
            default:
                break;
        }
    }

    // else don't care

    // unlock after done task
    xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);


}


// Register into event loop to check wifi status
_peripherals_err_t wifi_setup_regist_receive_event_task()
{
    // init state struct 
    // check state table

    if(s_wifi_state_table!=NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_NOT_NULL, __FILE__, __LINE__, "wifi state table not NULL");
        #endif
        return WIFI_SETUP_WIFI_STATE_TABLE_NOT_NULL;
    }

    // create state table
    s_wifi_state_table = (struct_wifi_state_t*)calloc(1, sizeof(struct_wifi_state_t)); // << must be calloc, lot of value need init 0 (NULL), do not use malloc

    if(s_wifi_state_table == NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_INIT_FAILED, __FILE__, __LINE__, "wifi state table not NULL");
        #endif
        return WIFI_SETUP_WIFI_STATE_TABLE_INIT_FAILED;
    }

    // regist state handler with default event loop
    esp_err_t ret1 = esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_setup_wifi_event_handler,
        NULL,
        &(s_wifi_state_table->wifi_handler));

    esp_err_t ret2 = esp_event_handler_instance_register(
        IP_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_setup_wifi_event_handler,
        NULL,
        &(s_wifi_state_table->ip_handler));
    
    if((ret1!=ESP_OK)||(ret2!=ESP_OK))
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_REGIST_EVENT_TASK_FAILED, __FILE__, __LINE__, esp_err_to_name(ret1 != ESP_OK ? ret1 : ret2));
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
        // debug
        #if CONFIG_DEBUG_ENABLE !=0
        fprintf(stderr, "\nInput mode similar with old mode in nvs, no update\n");
        #endif

        return PERIPH_OK; // ok no write to nvs :) no thing changed
    }

    // debug
    #if CONFIG_DEBUG_ENABLE !=0
    fprintf(stderr, "\nStart write [mode] to nvs\n");
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
        // debug
        #if CONFIG_DEBUG_ENABLE !=0
        fprintf(stderr, "\nInput country similar with old country in nvs, no update\n");
        #endif
        return PERIPH_OK;
    }

    // debug
    #if CONFIG_DEBUG_ENABLE !=0
    fprintf(stderr, "\nStart write [country] to nvs\n");
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
    conf.ap.ssid[sizeof(conf.ap.ssid) - 1] = '\0';

    conf.ap.ssid_len = strlen((const char *)conf.ap.ssid);

    strncpy((char*)conf.ap.password, (const char*)password, sizeof(conf.ap.password) - 1);
    conf.ap.password[sizeof(conf.ap.password) - 1] = '\0';

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
_peripherals_err_t wifi_setup_start_wifi_driver(uint8_t* busy)
{
    // check state flags
    // block all other task
    xSemaphoreTake(g_task_sync_tools->wifi_state_mutex, portMAX_DELAY);   

    // check state
    if( GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED)     ||  // sta driver stared << this still check if wifi sta no stopped before
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING)    ||  // sta driver starting

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTED)      ||  // ap driver started  << this still check if wifi sta no stopped before
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTING)     ||  // ap driver starting

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING))    // driver is stopping
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "Wifi STA/AP  starting/started");
        #endif

        * busy = WIFI_SETUP_COMMAND_REFUSED;

        goto return_ok;
    }

    // set start flag
    SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING);
    SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTING);

    // wifi start
    esp_err_t ret = esp_wifi_start();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_START_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif

        xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
        return WIFI_SETUP_WIFI_START_FAILED;
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
    return PERIPH_OK;

}




// =================================== 3.5 Scan wifi               ===================================
// =================================== Only for STA in (STA/ STA+AP )  ===============================
// Start scan
_peripherals_err_t wifi_setup_start_scan_wifi(uint8_t* busy)
{
    // check state flags
    // block all other task
    xSemaphoreTake(g_task_sync_tools->wifi_state_mutex, portMAX_DELAY);   

    // if connecting return
    // if scanning return
    // if sta_scanned return
    if( (! GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED))  || // no start driver -> no scan
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING)     || // starting -> ...
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING)  || // stoping -> ...

        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING)    || // connecting -> ...
        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING) || // disconnecting -> ...

        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING)      || // scanning -> no recall
        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNED)   )      // scanned  -> no rescan
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "Wifi STA connecting/scanning/scanned");
        #endif

        * busy = WIFI_SETUP_COMMAND_REFUSED;

        goto return_ok;
    }

    // else start scan, set scanning state
    SET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING);

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

        xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
        return WIFI_SETUP_WIFI_SCAN_FAILED;
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
    return PERIPH_OK;

}

// get list scanned
_peripherals_err_t wifi_setup_get_wifi_list_scanned(uint8_t* busy, uint16_t *number, wifi_ap_record_t **ap_records)
{
    if(*ap_records != NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_AP_RECORD_NOT_NULL, __FILE__, __LINE__, "value of record pointer not NULL");
        #endif
        return WIFI_SETUP_WIFI_AP_RECORD_NOT_NULL;
    }

    // check state flags
    // block all other task
    xSemaphoreTake(g_task_sync_tools->wifi_state_mutex, portMAX_DELAY);   

    // if scanning return
    // if not have any sta_scanned return

    if( (!GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED))    ||    // no start driver -> no list
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING)      ||    // starting driver -> no list
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING)   ||    // stoping -> ...
        
        (! GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNED) )  ||    // no scanned -> ...
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING)   )        // scanning -> ... 


    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "Wifi STA scanning/no instance scanned");
        #endif

        * busy = WIFI_SETUP_COMMAND_REFUSED;

        goto return_ok;
    }

    // else read scan, clear old scanned state
    CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNED);

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
    if(*number > 0)
    {
        *ap_records = calloc(*number, sizeof(wifi_ap_record_t));

        if(*ap_records == NULL)
        {
            #if CONFIG_DEBUG_ENABLE !=0
            send_peripheral_err_location(WIFI_SETUP_WIFI_AP_RECORD_ALLOC_FAILED, __FILE__, __LINE__, "Failed alloc memory for ap records");
            #endif
    
            xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
            return WIFI_SETUP_WIFI_AP_RECORD_ALLOC_FAILED;

        }

        ret = esp_wifi_scan_get_ap_records(number, *ap_records);
    
        if(ret!=ESP_OK)
        {
            #if CONFIG_DEBUG_ENABLE !=0
            send_peripheral_err_location(WIFI_SETUP_GET_AP_LIST_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
            #endif
    
            xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
            return WIFI_SETUP_GET_AP_LIST_FAILED;
        }
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
    return PERIPH_OK;

}





// =================================== 4. Wifi connect phase       ===================================
// =================================== Only for STA in (STA/ STA+AP ) ================================
// 1. connect to wifi
_peripherals_err_t wifi_setup_connect_to_access_point(uint8_t* busy)
{
    // check state flags
    // block all other task
    xSemaphoreTake(g_task_sync_tools->wifi_state_mutex, portMAX_DELAY);   

    // if connected return
    // if connecting return
    // if scanning return

    if( !(GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED))  ||  // no start -> no connect 
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING)    ||  // starting -> no connect

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING) ||  // stopping driver -> no connect

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED)    || // connected -> no connect
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING)   || // connecting -> no recall
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING)|| // disconnecting -> no connect

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING)   )    // scanning -> no nonnect
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "Wifi STA connected/connecting/scanning");
        #endif

        * busy = WIFI_SETUP_COMMAND_REFUSED;

        goto return_ok;
    }


    // else start connect
    SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING);

    // connect
    esp_err_t ret = esp_wifi_connect();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_CONNECT_TO_WIFI_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif

        xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
        return WIFI_SETUP_CONNECT_TO_WIFI_FAILED;
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
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
_peripherals_err_t wifi_setup_disconnect_wifi(uint8_t* busy)
{
    // check state flags
    // block all other task
    xSemaphoreTake(g_task_sync_tools->wifi_state_mutex, portMAX_DELAY);   

    // if sta_disconnecting return
    // if no sta_connected return

    if( (!GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED))  ||      // no start driver -> no disconnect
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING)    ||      // starting -> ...

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING)     ||  // stopping -> ...

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING)      ||  // connecting -> no disconnect
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING)   ||  // disconnecting -> no recall
        (! GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED) )  ||  // no connected -> no disconnect

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING))           // scanning -> no disnonnect
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "Wifi STA no connect/disconnecting");
        #endif

        * busy = WIFI_SETUP_COMMAND_REFUSED;

        goto return_ok;
    }

    // else start disconnect
    SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING);


    esp_err_t ret = esp_wifi_disconnect();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_DISCONNECT_TO_WIFI_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif

        // something wrong:
        // Because we know befor was connected so we just call esp_wifi_disconnect();
        // And we sure controlling all state, so the error maybe external Wifi missing connect
        // No need ROLL BACK, just continue stop wifi or reconnect.

        * busy = WIFI_SETUP_COMMAND_EXECUTED;

        xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
        return WIFI_SETUP_DISCONNECT_TO_WIFI_FAILED;
    }
    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
    return PERIPH_OK;
    
}



// Stop wifi driver
_peripherals_err_t wifi_setup_stop_wifi_driver(uint8_t* busy)
{
    // check state flags
    // block all other task
    xSemaphoreTake(g_task_sync_tools->wifi_state_mutex, portMAX_DELAY);   

    
    // if driver_stopping return
    // if no sta_started return
    // if no ap_started return
    if( (! GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED))   || // no start -> no stop
        (! GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTED))    || // no start -> no stop
        
        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING)      || // starting -> no stop
        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTING)       || // starting -> no stop
        
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING)|| // disconnecting -> ...
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING)   || // connecting -> ...
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED)    || // connected -> ...

        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING)     || // scanning -> no stop
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNED)      || // scanned -> no stop <- require read data scanned before stop

        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING))     // stopping -> no recall
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "Wifi driver stopping/sta no start/ap no start");
        #endif

        * busy = WIFI_SETUP_COMMAND_REFUSED;

        goto return_ok;
    }

    // else start disconnect
    SET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING);


    esp_err_t ret = esp_wifi_stop();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_STOP_WIFI_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif

        xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
        return WIFI_SETUP_STOP_WIFI_FAILED;
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
    return PERIPH_OK;
}



// unregister event loop
// clear struct table
_peripherals_err_t wifi_setup_unregister_event_task()
{
    // block all other task
    // warn: after call this funtion, event handler tasks call back by event task maybe queuing by mutex
    // the mutex do not be deleted , it should alive with program
    xSemaphoreTake(g_task_sync_tools->wifi_state_mutex, portMAX_DELAY);

    // check syntax
    if(s_wifi_state_table == NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_IS_NULL, __FILE__, __LINE__, "can't free NULL table");
        #endif

        xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
        return WIFI_SETUP_WIFI_STATE_TABLE_IS_NULL;
    }
    
    // un register event task
    esp_err_t ret1 = esp_event_handler_instance_unregister(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        s_wifi_state_table->wifi_handler);

    esp_err_t ret2 = esp_event_handler_instance_unregister(
        IP_EVENT,
        ESP_EVENT_ANY_ID,
        s_wifi_state_table->ip_handler);
    
    if((ret1!=ESP_OK)||(ret2!=ESP_OK))
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_REGIST_EVENT_TASK_FAILED, __FILE__, __LINE__, esp_err_to_name(ret1 != ESP_OK ? ret1 : ret2));
        #endif

        xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
        return WIFI_SETUP_UN_REGIST_EVENT_TASK_FAILED;
    }

    // free state struct

    // delete destintion AP old information in STA mode if it not NULL
    if(s_wifi_state_table->sta_dest_ap_connected != NULL) free(s_wifi_state_table->sta_dest_ap_connected);

    ap_list_client_connected_t *element, *temp;

    // fereach and delete all element in list
    LL_FOREACH_SAFE(s_wifi_state_table->ap_list_client_connected, element, temp)  // copy element->next and save in 'temp'
                                                              // then using temp to continue browse next element
    {
        LL_DELETE(s_wifi_state_table->ap_list_client_connected, element);      // delete element
        free(element);                                     // Free
        // element = NULL
    }

    free(s_wifi_state_table);
    s_wifi_state_table = NULL;

    // ok
    xSemaphoreGive(g_task_sync_tools->wifi_state_mutex);
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

// clear LwIP if need

// these behind extend case can auto clear after reset chip










// =================================== SUB FUNCTION                ===================================

// support function
// compare client id, support event task handler check when disconnect with client
static int compare_client_id(ap_list_client_connected_t* a, wifi_event_ap_stadisconnected_t* b) 
{
    return (a->client_connected_inf).aid - b->aid;
}