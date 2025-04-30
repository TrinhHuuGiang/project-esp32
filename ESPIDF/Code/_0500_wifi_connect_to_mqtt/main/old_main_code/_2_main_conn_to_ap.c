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

#define MAINLOOP_SLEEP_CYCLE 10000 // 10s

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

static char* s_dest_ap_connect_ssid = NULL;
static char* s_dest_ap_connect_password = NULL;

static wifi_ap_record_t* s_wifi_record_lists = NULL;
static uint16_t s_wifi_record_num = 0;

// mqtt
static char* s_broker_cert = NULL;// copy cert got from broker


/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */

// copy cert got from broker
static int mqtt_client_broker_cert_init(void);


// http function
static esp_err_t wifi_page_handler(httpd_req_t *req);
static esp_err_t connect_handler(httpd_req_t *req);

static void url_decode(char *dst, const char *src);


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

    wifi_setup_command_state_t busy = WIFI_SETUP_COMMAND_EXECUTED;

    while(!s_err_notify)
    {
        ESP_LOGI(MAIN_TAG,"||||||||||||||||||||\nMain loop - 10s");
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
        

        // delay
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



static esp_err_t wifi_page_handler(httpd_req_t *req)
{
    // Scan và lấy danh sách WiFi
    wifi_setup_command_state_t busy = WIFI_SETUP_COMMAND_EXECUTED;
    
    // Scan ít nhất 1 lần thành công
    while (busy == WIFI_SETUP_COMMAND_EXECUTED) {
        if (wifi_setup_start_scan_wifi(&busy)) return ESP_FAIL;
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    // Lấy danh sách
    busy = WIFI_SETUP_COMMAND_REFUSED;
    while (busy == WIFI_SETUP_COMMAND_REFUSED) {
        if (wifi_setup_get_wifi_list_scanned(&busy, &s_wifi_record_num, &s_wifi_record_lists)) return ESP_FAIL;
    }

    // Bắt đầu HTML response
    httpd_resp_sendstr_chunk(req,
        "<!DOCTYPE html><html><head><title>WiFi Config</title></head><body>"
        "<h2>Select WiFi and Enter Password</h2>"
        "<form method=\"POST\" action=\"/connect\">"
        "WiFi List: <select name=\"index\">");

    // Gửi từng option tương ứng với mạng WiFi
    for (int i = 0; i < s_wifi_record_num; ++i) {
        char option[128];
        snprintf(option, sizeof(option),
            "<option value=\"%d\">%s (RSSI: %d)</option>",
            i, (char*)s_wifi_record_lists[i].ssid, s_wifi_record_lists[i].rssi);
        httpd_resp_sendstr_chunk(req, option);
    }

    // Gửi phần còn lại của form
    httpd_resp_sendstr_chunk(req,
        "</select><br><br>"
        "Password: <input type=\"password\" name=\"password\" required><br><br>"
        "<input type=\"submit\" value=\"Connect\">"
        "</form></body></html>");

    httpd_resp_sendstr_chunk(req, NULL); // Kết thúc
    return ESP_OK;
}



// Hàm handler để nhận SSID và Password gửi lên
static esp_err_t connect_handler(httpd_req_t *req)
{
    char buf[256];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[ret] = '\0';
    ESP_LOGI(HTTP_TAG, "Received POST data: %s", buf);

    // Biến tạm
    int index = -1;
    char password[64] = {0};

    // Parse form-urlencoded: index=2&password=abc123
    char *token = strtok(buf, "&");
    while (token != NULL) {
        char *equal = strchr(token, '=');
        if (equal) {
            *equal = '\0';
            char *key = token;
            char *value = equal + 1;

            // Giải mã URL
            char decoded[128];
            url_decode(decoded, value);

            if (strcmp(key, "index") == 0) {
                index = atoi(decoded);
            } else if (strcmp(key, "password") == 0) {
                strncpy(password, decoded, sizeof(password) - 1);
            }
        }
        token = strtok(NULL, "&");
    }

    // Kiểm tra index hợp lệ
    if (index < 0 || index >= s_wifi_record_num) {
        httpd_resp_sendstr(req, "Invalid WiFi index.");
        return ESP_FAIL;
    }

    // Lấy thông tin mạng đã chọn
    uint8_t *ssid = (uint8_t *)s_wifi_record_lists[index].ssid;
    uint8_t *bssid = s_wifi_record_lists[index].bssid;


    // free list for next scan
    free(s_wifi_record_lists);
    s_wifi_record_lists = NULL;

    // Có thể in ra để kiểm tra
    ESP_LOGI(HTTP_TAG, "Selected SSID: %s", ssid);
    ESP_LOGI(HTTP_TAG, "Password: %s", password);

    // Gọi cấu hình WiFi
    if (wifi_setup_set_wifi_sta_config(ssid, (uint8_t *)password, bssid)) {
        httpd_resp_sendstr(req, "Failed to connect to selected WiFi.");
    } else {
        httpd_resp_sendstr(req, "WiFi configuration updated successfully.");
    }

    return ESP_OK;
}




// url decoder 
// Hàm giải mã URL-encoded (%XX)
static void url_decode(char *dst, const char *src)
{
    char a, b;
    while (*src) {
        if (*src == '%')
        {
            a = src[1] ; b = src[2] ;
            if(isxdigit(a) && isxdigit(b))
            {
                a = (a >= 'a') ? a - 'a' + 10 : (a >= 'A') ? a - 'A' + 10 : a - '0';
                b = (b >= 'a') ? b - 'a' + 10 : (b >= 'A') ? b - 'A' + 10 : b - '0';
                *dst++ = 16 * a + b;
                src += 3;
            }
        }
        else if (*src == '+')
        {
            *dst++ = ' '; // Dấu + là space
            src++;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}
