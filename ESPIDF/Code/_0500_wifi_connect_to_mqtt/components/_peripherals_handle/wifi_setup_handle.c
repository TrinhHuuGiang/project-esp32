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
 * Prototype
 * **********************************************************
 */
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

/*

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


*/

// wifi even handler
void wifi_setup_wifi_event_handler
(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    // block all other task
    xSemaphoreTake(s_wifi_state_table->wifi_state_mutex, portMAX_DELAY);

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

                // get inform network
                s_wifi_state_table->sta_dest_ap_connected = calloc(1, sizeof(wifi_event_sta_connected_t));

                memcpy(s_wifi_state_table->sta_dest_ap_connected, event_data, sizeof(wifi_event_sta_connected_t));

                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                
                // + clear sta_connected
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED);

                // + clear sta_disconnecting
                CLR_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING);

                // clear net connected
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
    xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);


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

    // init default mutex key
    s_wifi_state_table->wifi_state_mutex = xSemaphoreCreateMutex();

    if(s_wifi_state_table->wifi_state_mutex == NULL)
    {
        free(s_wifi_state_table);
        s_wifi_state_table = NULL;

        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_CREATE_MUTEX_FAILED, __FILE__, __LINE__, "Wifi state mutex failed");
        #endif
        return WIFI_SETUP_WIFI_STATE_TABLE_CREATE_MUTEX_FAILED;
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
    xSemaphoreTake(s_wifi_state_table->wifi_state_mutex, portMAX_DELAY);   

    // if sta_started return  WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED
    // if sta_starting return WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING
    // if ap_started return   WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTED
    // if ap_starting return  WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTING
    if( GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED)  ||
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTING) ||
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTED)   ||
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTING) )
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

        xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
        return WIFI_SETUP_WIFI_START_FAILED;
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
    return PERIPH_OK;

}




// =================================== 3.5 Scan wifi               ===================================
// =================================== Only for STA in (STA/ STA+AP )  ===============================
// Start scan
_peripherals_err_t wifi_setup_start_scan_wifi(uint8_t* busy)
{
    // check state flags
    // block all other task
    xSemaphoreTake(s_wifi_state_table->wifi_state_mutex, portMAX_DELAY);   

    // if connecting return
    // if scanning return
    // if sta_scanned return
    if( GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING) ||
        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING)   ||
        GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNED)   )
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

        xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
        return WIFI_SETUP_WIFI_SCAN_FAILED;
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
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
    xSemaphoreTake(s_wifi_state_table->wifi_state_mutex, portMAX_DELAY);   

    // if scanning return
    // if not have any sta_scanned return

    if( (! GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNED) )  ||
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING))
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
    
            xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
            return WIFI_SETUP_WIFI_AP_RECORD_ALLOC_FAILED;

        }

        ret = esp_wifi_scan_get_ap_records(number, *ap_records);
    
        if(ret!=ESP_OK)
        {
            #if CONFIG_DEBUG_ENABLE !=0
            send_peripheral_err_location(WIFI_SETUP_GET_AP_LIST_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
            #endif
    
            xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
            return WIFI_SETUP_GET_AP_LIST_FAILED;
        }
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
    return PERIPH_OK;

}





// =================================== 4. Wifi connect phase       ===================================
// =================================== Only for STA in (STA/ STA+AP ) ================================
// 1. connect to wifi
_peripherals_err_t wifi_setup_connect_to_access_point(uint8_t* busy)
{
    // check state flags
    // block all other task
    xSemaphoreTake(s_wifi_state_table->wifi_state_mutex, portMAX_DELAY);   

    // if connected return
    // if connecting return
    // if scanning return

    if( GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED)   ||
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTING)   ||
        GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_SCANNING)   )
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

        xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
        return WIFI_SETUP_CONNECT_TO_WIFI_FAILED;
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
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
    xSemaphoreTake(s_wifi_state_table->wifi_state_mutex, portMAX_DELAY);   

    // if sta_disconnecting return
    // if no sta_connected return

    if( GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_DISCONNECTING)   ||
    (! GET_BIT(s_wifi_state_table->wifi_ip_state, WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_CONNECTED) ))
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

        xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
        return WIFI_SETUP_DISCONNECT_TO_WIFI_FAILED;
    }
    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
    return PERIPH_OK;
    
}



// Stop wifi driver
_peripherals_err_t wifi_setup_stop_wifi_driver(uint8_t* busy)
{
    // check state flags
    // block all other task
    xSemaphoreTake(s_wifi_state_table->wifi_state_mutex, portMAX_DELAY);   

    
    // if driver_stopping return
    // if no sta_started return
    // if no ap_started return
    if( GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_DRIVER_STOPPING)   ||
        (  (! GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_STA_STARTED))   &&
           (! GET_BIT(s_wifi_state_table->wifi_ip_state,WIFI_SETUP_WIFI_IP_FLAG_STATE_AP_STARTED))       )   )
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

        xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
        return WIFI_SETUP_STOP_WIFI_FAILED;
    }

    // command done
    * busy = WIFI_SETUP_COMMAND_EXECUTED;

    // ok
return_ok:

    xSemaphoreGive(s_wifi_state_table->wifi_state_mutex);
    return PERIPH_OK;
}



// unregister event loop
// clear struct table
_peripherals_err_t wifi_setup_unregister_event_task()
{
    // check syntax
    if(s_wifi_state_table == NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(WIFI_SETUP_WIFI_STATE_TABLE_IS_NULL, __FILE__, __LINE__, "can't free NULL table");
        #endif
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
        return WIFI_SETUP_UN_REGIST_EVENT_TASK_FAILED;
    }

    // free state struct

    vSemaphoreDelete(s_wifi_state_table->wifi_state_mutex); // delete mutex

    // delete destintion AP old information in STA mode if it not NULL
    if(s_wifi_state_table->sta_dest_ap_connected != NULL) free(s_wifi_state_table->sta_dest_ap_connected);

    ap_list_client_connected_t *element, *temp;

    // Duyệt và xóa từng phần tử
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