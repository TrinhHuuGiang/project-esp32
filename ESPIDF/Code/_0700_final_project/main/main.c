/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */
#include "main_init_api.h"
#define main_tag "MAIN_LOG"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */ 
extern __task_sync_t* g_task_sync_tools; // get sync tools 


// wifi status
static esp_netif_t *sta_net_if = NULL;
static esp_netif_t *ap_net_if = NULL;

// spi sdcard
static sdmmc_card_t* out_card = NULL;


/**
 * **********************************************************
 * Main
 * **********************************************************
 */
void app_main(void)
{
/** ==========================================================================
 * Init Layer (See README.md)
--------------------------------------------------------------------------- */

    // return err code when init Layer 1-> 4
    main_ret_err_t ret  = MAIN_RET_OK;

                                /** =========================================
                                * LAYER 1: System architecture infrastructure
                                ------------------------------------------- */

    if(task_sync_tools_init()) 
    {
        ret = MAIN_RET_TASKSYNC_INIT_FAIL;
        goto main_log_restart; // << init g_task_sync_tools
    }
    
    ESP_LOGI(main_tag,"sync pass");

                                /** =========================================
                                * LAYER 2: System resource
                                ------------------------------------------- */
    
    /** ======================
    * LAYER 2.1: Internal peripherals
    ------------------------ */
    
    // i2c init block
    {
        i2c_config_t* master_conf=NULL;

        if(i2c_master_init_config((i2c_config_t*)(&master_conf))) { ret = MAIN_RET_I2C_MASTER_INIT_FAIL; goto main_log_restart;}

        if(i2c_master_setup_hardware(master_conf)) { ret = MAIN_RET_I2C_MASTER_CONF_FAIL; goto main_log_restart;}

        if(i2c_master_install_driver(master_conf)) { ret = MAIN_RET_I2C_MASTER_INST_FAIL; goto main_log_restart;}
        
        i2c_master_free_config((i2c_config_t*)(&master_conf));
    }

    ESP_LOGI(main_tag,"i2c pass");

    // spi init block
    {
        spi_bus_config_t* spi_master_bus_conf = NULL;
        
        if(spi_master_init_bus_config((spi_bus_config_t*)(&spi_master_bus_conf))) { ret = MAIN_RET_SPI_MASTER_INIT_FAIL; goto main_log_restart;}

        if(spi_master_install_bus_config(spi_master_bus_conf)) { ret = MAIN_RET_SPI_MASTER_INST_FAIL; goto main_log_restart;}
        
        spi_master_free_bus_config((spi_bus_config_t*)(&spi_master_bus_conf));
    }

    ESP_LOGI(main_tag,"spi pass");

    // adc1 init
    if(adc1_setup_init_atten11dB()) { ret = MAIN_RET_ADC1_INIT_FAIL; goto main_log_restart;}

    ESP_LOGI(main_tag,"adc pass");

    // gpio
    // comming soon infuture when we need sleep mode or interrupt
    // otherwhise any setup input, output don't need config here, it work when use peripheral
    


    // ledc
    // config it when coding logic block or driver :)


    // mcpwm
    // config it when coding logic block or driver :)


    // i2s
    // config it when call in driver "widDr_wav_player"


    // start wifi driver
    {
        // setup wifi driver
        if(wifi_setup_init_nvs_flash()) {ret = MAIN_RET_WIFI_NVS_INIT_FAIL; goto main_log_restart;}
        if(wifi_setup_create_default_event_loop()) {ret = MAIN_RET_WIFI_CREATE_EVENTLOOP_FAIL; goto main_log_restart;}
        if(wifi_setup_init_lightweight_ip_inform()) {ret = MAIN_RET_WIFI_SETUP_LWIP_FAIL; goto main_log_restart;}
        if(wifi_setup_create_interface_wifi_STA_link_LwIP(&sta_net_if)) {ret = MAIN_RET_WIFI_SETUP_CREATE_ITF_STA_FAIL; goto main_log_restart;}
        if(wifi_setup_create_interface_wifi_AP_link_LwIP(&ap_net_if)) {ret = MAIN_RET_WIFI_SETUP_CREATE_ITF_AP_FAIL; goto main_log_restart;}
        if(wifi_setup_init_wifi_driver()) {ret = MAIN_RET_WIFI_DRIVER_INIT_FAIL; goto main_log_restart;}

        // set nvs default content if it not available
        if(wifi_setup_set_wifi_country()) {ret = MAIN_RET_WIFI_SET_COUNTRY_FAIL; goto main_log_restart;} 
            
            // scroll down 2.3 to get more update wifi info
            // then set up wifi ap , sta( pass, ssd, bssid)
            // example:
            // if(wifi_setup_set_wifi_ap_config((uint8_t*)WIFI_AP_SSID, (uint8_t*)WIFI_AP_PASSWORD)) return;
            // if(wifi_setup_set_wifi_sta_config((uint8_t*)WIFI_STA_SSID, (uint8_t*)WIFI_STA_PASSWORD, (uint8_t*)BSSID)) return;
            // check new wifi inform and set after config sdcard module
            // otherwise return it similar with old config

            // start wifi after config SSID and PASSWORD <- scroll down to layer 2.3
            // find : main_update_wifi_apsta_from_file()
    }

    ESP_LOGI(main_tag,"conf wifi");




    /** ======================
    * LAYER 2.2: External peripherals
    ------------------------ */

    // 165 input series x32
    if(gpio_74HC165_init_input_gate()){ret = MAIN_RET_74HC165_INIT_INPUT_FAIL; goto main_log_restart;}
    
    ESP_LOGI(main_tag,"165 pass");

    // 595 output series x32
    if(gpio_74HC595_init_output_gate()){ret = MAIN_RET_74HC595_INIT_OUTPUT_FAIL; goto main_log_restart;}
    // set output default (0xFFFFFFFF) for spi chip select conflict avoidance
    // all port output is high at start

    ESP_LOGI(main_tag,"595 pass");

    // 4067 analog input x16
    // don't need init


    // max98357
    if(i2s_max98357_turn_off()){ret = MAIN_RET_98357_TOFF_FAIL; goto main_log_restart;}

    ESP_LOGI(main_tag,"98357 pass");



    /** ======================
    * LAYER 2.3: External peripherals
    ------------------------ */

    // i2c_ssd1306
    // i2c device only need right address and init i2c driver was done behind
    // it will init and control by "winDr_display_ferformance"


    // ds3231
    // it only 1 address, and apis can call by some 'time handle': widDr_time_realtime, widDr_time_schedule


    // microsd spi module
    if(spi_sd_card_mount_vfs_with_fatfs_on_card(&out_card)){ret = MAIN_RET_98357_TOFF_FAIL; goto main_log_restart;}
        
    ESP_LOGI(main_tag,"mounted sd");


        // fix file system, update new info: wifi ap,sta
        {
            // take control chip select 
            take_cs_spi_mutex();
            if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0))
            {ret = MAIN_RET_TAKE_CS_MU_FAIL; goto main_log_restart;}



            // SPI_SD_CARD_ROOT_PATH_STRING  "/sd_card0"
            // main check missing file and folder
            if(main_fix_necess_system_file_folder()){ret = MAIN_RET_FIX_MISSING_FILE_FOLDER_FAIL; goto main_log_restart;}
        
            // check file update password, ssid for wifi ap + pass, ssid,bssid sta info
            if(main_update_wifi_apsta_from_file()){ret = MAIN_RET_FIX_UPDATE_WIFI_FROM_FILE_FAIL; goto main_log_restart;}
        


            // release chip select bus
            if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
            {ret = MAIN_RET_RELEASE_CS_MU_FAIL; goto main_log_restart;}

            release_cs_spi_mutex();
        }


        
        // now start wifi in parallel task
        // start manager apsta driver
        {
            BaseType_t check_task = xTaskCreate(wifi_apsta_mode_driver_manager, "wifi apsta manager", 
                TASK_STACK_SIZE_EXTREM, NULL , TASK_PRIO_IMPORTANT , NULL );
            if (check_task != pdPASS){ret = MAIN_RET_START_WIFI_MANAGER_FAIL; goto main_log_restart;}
        }
 

        ESP_LOGI(main_tag,"wifi pass");

    /** ======================
    * LAYER 2.4: Network application protocol
    ------------------------ */

    // http server 
    if (http_server_handle_start_server()){ret = MAIN_RET_HTTP_SERVER_START_FAIL; goto main_log_restart;}

    ESP_LOGI(main_tag,"httpsv pass");


    // mqtt client check and init
    {
        // take control chip select 
        take_cs_spi_mutex();
        if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0))
        {ret = MAIN_RET_TAKE_CS_MU_FAIL; goto main_log_restart;}


        if(main_update_mqtt_config_from_file_and_init_mqtt_client()) {ret = MAIN_RET_MQTT_CLIENT_INIT_FAIL; goto main_log_restart;};
    
        
        // release chip select bus
        if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
        {ret = MAIN_RET_RELEASE_CS_MU_FAIL; goto main_log_restart;}

        release_cs_spi_mutex();
    
    }
    // mqtt regist event loop, start client
    {
        // regist event loop
        if(mqtt_client_handle_regist_receive_event_task())
            {ret = MAIN_RET_MQTT_CLIENT_REGIST_EVENTLOOP_FAIL; goto main_log_restart;}
        
        // start client
        mqtt_client_command_state_t busy;
        if(mqtt_client_handle_client_start(&busy))
            {ret = MAIN_RET_MQTT_CLIENT_START_FAIL; goto main_log_restart;}
    
    }

    ESP_LOGI(main_tag,"mqttstart pass");





                                /** =========================================
                                * LAYER 3: External devices
                                ------------------------------------------- */







                                /** =========================================
                                * LAYER 4: Controller and logic
                                ------------------------------------------- */

    /** ======================
    * LAYER 4.1: System driver
    ------------------------ */
    // Widget driver display performance
    if(widDr_display_init_and_run()) {ret = MAIN_RET_WID_DISPLAY_INIT_FAIL; goto main_log_restart;}

    ESP_LOGI(main_tag,"w_dis pass");

    // Widget driver time realtime
    if(widDr_update_rtc_to_DS3231()) {ret = MAIN_RET_WID_REALTIME_FAIL; goto main_log_restart;}

    ESP_LOGI(main_tag,"w_rtc pass");


    // sysDr_http_update_new_config (update systemfile by app)



    // sysDr_net_mqtt_manager (start driver, connect to broker, wait register topic form others logic blocks)
    if(widDr_net_mqtt_manager_start_task()) {ret = MAIN_RET_WID_MQTT_MANAGER_FAIL; goto main_log_restart;}

    ESP_LOGI(main_tag,"w_mqtt_man pass");

    /** ======================
    * LAYER 4.2: User/logic block init driver
    ------------------------ */
    





    

/** ==========================================================================
 * Main loop check err flag
--------------------------------------------------------------------------- */

    // main loop until an error feedback by err_flag (set by some widget, driver, or user code)
    while(!g_task_sync_tools->err_flag)
    {
        // delay
        vTaskDelay(pdMS_TO_TICKS(MAIN_LOOP_CHECK_CYCLE)); // check system feedback

        // log before check flag
        ESP_LOGI(main_tag, "main check");
    }



/** ----------------------------------------------------------------------------
 * Restart
 *  2 possible cases:
 *      - init failed -> ret != 0
 *      - global flag set -> unexpected error(from driver, user code)
==============================================================================*/
main_log_restart:
    if(ret)
    {
        ESP_LOGW(main_tag, "Err code: %d" ,ret);
    }
    else if(g_task_sync_tools->err_flag) // by widget or somewhere 
    {
        ESP_LOGW(main_tag, "SYSTEM ERR FLAG SET !!!");
    }

    // restart in 10s + log
    for(int i = MAIN_LOOP_RESTART_COUNTDOWN_NUM; i>0;i--)
    {
        ESP_LOGW(main_tag, "Restart in: %d" ,i);
        vTaskDelay(pdMS_TO_TICKS(MAIN_LOOP_RESTART_COUNTDOWN_DELAY));
    }

    // reset chip
    esp_restart();

    // ok
    return;
}
