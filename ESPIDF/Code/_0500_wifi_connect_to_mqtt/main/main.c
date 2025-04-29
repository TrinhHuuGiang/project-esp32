// connect to MQTT Cloud using MQTT and publish a message to a topic

// using linux, get the certificate:
// " openssl s_client -showcerts -connect [server url] 
// </dev/null 2>/dev/null|openssl x509  -outform PEM >mqtt_eclipse_org.pem "

// Note: 
// - server url example: mqtt.eclipseprojects.io:8883 if using service cloud of 
// mqtt.eclipseprojects.io

// - example using mqtts from HiveMQ server:
// "openssl s_client -showcerts -connect 7b92f852aa7b4f20a563529b35a8bd79.s1.eu.hivemq.cloud:8883
// </dev/null 2>/dev/null|openssl x509  -outform PEM >mqtt_eclipse_org.pem"

// -outform PEM > mqtt_eclipse_org.pem    // here we have certificate pem format

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "main.h"

#define MAIN_TAG "MAIN_LOOP"

#define MAINLOOP_SLEEP_CYCLE 3000 // 3s

// AP info
#define WIFI_AP_SSID "Hello ESP32"
#define WIFI_AP_PASSWORD "123123123"

// STA info
// get by scan list


#define HTTP_TAG "HTTP_LOG"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
// error state
static uint8_t s_err_notify = 0; // set if 1 funtion encounter an error

// wifi
static wifi_manager_apsta_mode_t* s_apsta_driver_manager = NULL;

// mqtt
static char* s_broker_cert = NULL;// copy cert got from broker


/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */

// copy cert got from broker
static int mqtt_client_broker_cert_init(void);


// http callback function
static esp_err_t wifi_page_handler(httpd_req_t *req);
static esp_err_t connect_handler(httpd_req_t *req);


/**
 * **********************************************************
 * Task
 * **********************************************************
 */

//  wifi driver manager task
static void wifi_apsta_mode_driver_manager(void* args);


/**
 * **********************************************************
 * Main Codes
 * **********************************************************
 */

void app_main(void)
{

    // init tasks sync tools
    if(task_sync_tools_init()) return;

    // wifi variable
    esp_netif_t *sta_net_if = NULL;
    esp_netif_t *ap_net_if = NULL;

    // setup wifi driver
    if(wifi_setup_init_nvs_flash()) return;
    if(wifi_setup_create_default_event_loop()) return;
    if(wifi_setup_init_lightweight_ip_inform()) return;
    if(wifi_setup_create_interface_wifi_STA_link_LwIP(&sta_net_if)) return;
    if(wifi_setup_create_interface_wifi_AP_link_LwIP(&ap_net_if)) return;
    if(wifi_setup_init_wifi_driver()) return;
    
    ESP_LOGI(MAIN_TAG, "done init wifi driver");

    // set nvs content if it changed
    if(wifi_setup_set_wifi_country()) return;
    if(wifi_setup_set_wifi_ap_config((uint8_t*)WIFI_AP_SSID, (uint8_t*)WIFI_AP_PASSWORD)) return;

    ESP_LOGI(MAIN_TAG, "done setup country + wifi ap config");

    // start manager apsta driver
    xTaskCreate(wifi_apsta_mode_driver_manager, "wifi apsta manager", 2048, NULL , 3 , NULL );

    // HTTP
    printf("FreeRTOS heap size: %u\n", esp_get_free_heap_size());
    printf("FreeRTOS minimum heap size: %u\n", esp_get_minimum_free_heap_size());

    // init http server
    // ESP_LOGW(MAIN_TAG, "wait 5 s");
    // vTaskDelay(pdMS_TO_TICKS(5000));  // wait 5 second for start wifi
    ESP_LOGW(HTTP_TAG, "HTTP starting ...");
    
    // 1. Bắt đầu server
    if (http_server_handle_start_server() != PERIPH_OK)
    {
        ESP_LOGE(HTTP_TAG, "Failed to start HTTP server");
        return;
    }

    // 2. Thêm URI cho trang config wifi
    http_server_handle_append_uri_struct_to_list(
        "/wifi",
        HTTP_GET,
        wifi_page_handler,
        NULL,
        0
    );

    // 3. Thêm URI cho xử lý kết nối
    http_server_handle_append_uri_struct_to_list(
        "/connect",
        HTTP_POST,
        connect_handler,
        NULL,
        0
    );

    // 4. Đăng ký URI vào server
    if (http_server_handle_regist_uri_list_to_handler() != PERIPH_OK)
    {
        ESP_LOGE(HTTP_TAG, "Failed to register URI list");
        return;
    }

    ESP_LOGI(HTTP_TAG, "HTTP server is up!");




    // main loop
    size_t total_DR = heap_caps_get_total_size(MALLOC_CAP_8BIT);

    uint32_t rest_DR = 0;

    double per_use = 0;
    uint8_t used_level = 0;

    while(!s_err_notify)
    {
        ESP_LOGI(MAIN_TAG,"Main loop - 1s");
        printf("0. _ Total   DRAM          : %u B\n", total_DR);
        printf("1. _ Current DRAM heap size: %u B\n", rest_DR = esp_get_free_heap_size());
        
        per_use = 1 - ((double)rest_DR/ total_DR);

        used_level = per_use * 10;

        printf("[");
        for(uint8_t i = 0; i < 10 ; i++)
        {
            if(i<=used_level) printf("#");
            else printf("_");
        }
        printf("] - %f%c \n", per_use*100,37);

        vTaskDelay(pdMS_TO_TICKS(MAINLOOP_SLEEP_CYCLE));
    }

    // exit
    ESP_LOGE(MAIN_TAG, "SYSTEM GET ERROR !!");

    return;
}



/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// copy cert got from broker
static int mqtt_client_broker_cert_init(void)
{
    int cert_len = strlen(MQTT_BROKER_CERTIFICATE);

    char* calloc_cert = calloc(cert_len + 1, sizeof(char));

    if(calloc_cert == NULL)
    {
        return 1; // failed to alloc cert
    }

    s_broker_cert = calloc_cert;

    return 0; // ok

}


//  wifi driver manager task
static void wifi_apsta_mode_driver_manager(void* args)
{
    if(wifi_setup_start_apsta_mode_manager_driver(&s_apsta_driver_manager)) s_err_notify = 1; // set err notify if return error

    // exit
    vTaskDelete(NULL);
}



// http

// Hàm handler cho trang web /wifi
static esp_err_t wifi_page_handler(httpd_req_t *req)
{
    const char html_response[] = 
        "<!DOCTYPE html>"
        "<html>"
        "<head><title>WiFi Config</title></head>"
        "<body>"
        "<h2>Enter WiFi Credentials</h2>"
        "<form action=\"/connect\" method=\"POST\">"
        "SSID: <input type=\"text\" name=\"ssid\"><br>"
        "Password: <input type=\"password\" name=\"password\"><br><br>"
        "<input type=\"submit\" value=\"Connect\">"
        "</form>"
        "</body>"
        "</html>";

    httpd_resp_send(req, html_response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Hàm handler để nhận SSID và Password gửi lên
static esp_err_t connect_handler(httpd_req_t *req)
{
    char buf[100];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[ret] = '\0'; // null-terminate chuỗi

    ESP_LOGI(HTTP_TAG, "Received POST data: %s", buf);

    httpd_resp_sendstr(req, "Connecting...");

    // Ở đây bạn có thể parse chuỗi buf để lấy ssid và password nhé

    return ESP_OK;
}