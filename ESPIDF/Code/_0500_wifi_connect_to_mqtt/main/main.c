/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "main.h"
#define MAIN_TAG "MAIN_LOOP"
#define WIFI_DRV "WIFI_DRIVER_LOOP"

// AP info
#define WIFI_AP_SSID "Hello ESP32"
#define WIFI_AP_PASSWORD "123123123"

#define WIFI_RESET_TO_DEFAULT_BUTTON (25)


// STA info


// Wifi driver
#define WIFI_DRV_RECHECK_NETWORK_CYCLE    (5000) // check network connect after 5s


/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
// create led handle task
static int return_all_flag = 0; // <<- notify father task and all task using this flag is some thing wrong happened, clean and return to retart for safe


static uint8_t s_wifi_reset_factory_holding = 1; // pull up default, low is holding button

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
static int init_special_wifi_button();

// wifi factory reset
// click a button to set default wifi status :)
// no using interrupt low level, it not stable
static void reset_wifi_to_factory_handler(void* arg);


/**
 * **********************************************************
 * Drivers
 * **********************************************************
 */

// driver manage state wifi when start wifi driver
static void main_driver_control_wifi_apsta();

// driver manage http server when start wifi driver
static void main_driver_control_http_server();

// driver manage mqtt client

/**
 * **********************************************************
 * Main Codes
 * **********************************************************
 */

void app_main(void)
{
    esp_netif_t *sta_net_if = NULL;
    esp_netif_t *ap_net_if = NULL;
    uint8_t wifi_busy = WIFI_SETUP_COMMAND_REFUSED;

    // setup wifi driver
    if(wifi_setup_init_nvs_flash()) return;
    if(wifi_setup_create_default_event_loop()) return;
    if(wifi_setup_init_lightweight_ip_inform()) return;
    if(wifi_setup_create_interface_wifi_STA_link_LwIP(&sta_net_if)) return;
    if(wifi_setup_create_interface_wifi_AP_link_LwIP(&ap_net_if)) return;
    if(wifi_setup_init_wifi_driver()) return;

    ESP_LOGI(MAIN_TAG, "done init wifi driver");

    // regist event notify
    if(wifi_setup_regist_receive_event_task()) return;

    ESP_LOGI(MAIN_TAG, "done regiser wifi/IP eventasks");
    
    // set config STA+AP mode
    if(wifi_setup_set_wifi_mode(WIFI_MODE_APSTA)) return;
    if(wifi_setup_set_wifi_country()) return;

    ESP_LOGI(MAIN_TAG, "done setup mode/country");
    

    // continue program while wifi driver handle event

    // init button reset wifi
    if(init_special_wifi_button()) return;

    xTaskCreate(reset_wifi_to_factory_handler, "Wifi factory handler task", 2048, NULL, 5, NULL);
    

    while(!return_all_flag)
    {
        ESP_LOGW("Hello", "1s");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGW(MAIN_TAG, "END PROGRAM");
    return;
}



/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// init button
static int init_special_wifi_button()
{
    // install isr service for GPIO
    if(gpio_setup_install_isr_service_for_gpio_system()) return 1;

    if(gpio_setup_reset_pin_to_origin(WIFI_RESET_TO_DEFAULT_BUTTON)) return 1;

    // gpio in
    if(gpio_setup_io_direction(WIFI_RESET_TO_DEFAULT_BUTTON,GPIO_MODE_INPUT)) return 1;

    // set pull
    if(gpio_setup_pull_res(WIFI_RESET_TO_DEFAULT_BUTTON,GPIO_PULLUP_ONLY)) return 1;


    return 0;
}





// handle hold button and factory reset wifi
static void reset_wifi_to_factory_handler(void* arg)
{
    int count_temp = 5;

    while(!return_all_flag)
    {
        // first check
        if(gpio_setup_get_logic_level(WIFI_RESET_TO_DEFAULT_BUTTON, &s_wifi_reset_factory_holding)) 
        {
            return_all_flag = 1;
            break;
        }

        while(!s_wifi_reset_factory_holding) // pull up : if logic 0 -> holding, 1 is idle
        {
            ESP_LOGE(MAIN_TAG, "Prepare reset wifi to factory [%d]",count_temp);

            if(!count_temp) // ok start reset
            {
                // default sta inform
                // wifi_setup_set_wifi_sta_config(uint8_t ssid[32] , uint8_t password[64], uint8_t bssid[6]);

                // default ap inform
                wifi_setup_set_wifi_ap_config((uint8_t*)WIFI_AP_SSID,(uint8_t*) WIFI_AP_PASSWORD);   
            }

            // count down
            count_temp --;


            // hanging if reset done
            // reset done, release button
            if(count_temp < 0)
            {
                ESP_LOGE(MAIN_TAG, "Please release reset button");
                count_temp = -1;
            }

            // delay
            
            vTaskDelay(pdMS_TO_TICKS(1000));// 1s

            // recheck
            if(gpio_setup_get_logic_level(WIFI_RESET_TO_DEFAULT_BUTTON, &s_wifi_reset_factory_holding)) 
            {
                return_all_flag = 1;
                break;
            }
        }

        // reset count
        count_temp = 5;

        //delay
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    // exit
    vTaskDelete(NULL);
}






// driver control apsta mode
static void main_driver_control_wifi_apsta(void* arg)
{
    int wifi_busy = WIFI_SETUP_COMMAND_REFUSED;

    // Wifi driver start
    while(!return_all_flag)
    {
        ESP_LOGW(WIFI_DRV, "Checking wifi ...");

        // [apsta] try starting / check started wifi driver
        wifi_busy = WIFI_SETUP_COMMAND_EXECUTED;
        while(wifi_busy == WIFI_SETUP_COMMAND_EXECUTED)
        {
            if(wifi_setup_start_wifi_driver(&wifi_busy)) return;  // if wifi stopped -> try start it
        }

        ESP_LOGI(WIFI_DRV, "Starting/started APSTA driver");

        // [sta] try connecting / connected
        wifi_busy = WIFI_SETUP_COMMAND_EXECUTED;
        while(wifi_busy != WIFI_SETUP_COMMAND_EXECUTED) 
        {
            if(wifi_setup_connect_to_access_point(&wifi_busy)) return; // if wifi sta no access to AP -> try connect it
        }

        ESP_LOGI(WIFI_DRV, "Connecting/connected STA to AP");
        
        // sleep task
        vTaskDelay(pdMS_TO_TICKS(WIFI_DRV_RECHECK_NETWORK_CYCLE)); // sleep 2s then continue check
    }

    // Try stop wifi driver

    // 1. 

    // exit
    vTaskDelete(NULL);
}