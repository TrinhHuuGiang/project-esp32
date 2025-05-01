// this example try send 1-> 9 to http server, tested on HIVEMQ cluster

// connect to MQTT Cloud using MQTT and publish a message to a topic

// using linux, get the certificate:
// " openssl s_client -showcerts -connect [server url] 
// </dev/null 2>/dev/null|openssl x509  -outform PEM >mqtt_eclipse_org.pem "

// Note: 
// - server url example: mqtt.eclipseprojects.io:8883 if using service cloud of 
// mqtt.eclipseprojects.io

// - example using mqtts from HiveMQ server:
// "openssl s_client -showcerts -connect 7b92f852aa7b4f20a563529b35a8bd79.s1.eu.hivemq.cloud:8883
// copy all cert you can see, and create a cert chain like macro below

// -outform PEM > mqtt_eclipse_org.pem    // here we have certificate pem format



// - example using EMQX server: 
// + download CA from server (type crt) (hmm this CA no enough) :) ignore it , should using HIVEmq 


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



// MQTT
#define MQTT_TAG "MQTT_LOG"

#define MQTT_BROKER_URI "mqtts://7b92f852aa7b4f20a563529b35a8bd79.s1.eu.hivemq.cloud:8883"

// #define MQTT_BROKER_URI "mqtt://f577ef27.ala.us-east-1.emqxsl.com:1883"

// #define MQTT_BROKER_URI "mqtt://mqtt.eclipseprojects.io"

#define MQTT_BROKER_USER "mqtt_esp32"
#define MQTT_BROKER_PASS "MQTT_esp32"

#define MQTT_BROKER_EMQX_CERTIFICATE "-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----"

#define MQTT_BROKER_HIVEMQ_CERTIFICATE \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFPDCCBCSgAwIBAgISBlvO6+BhAYoAodu3Tizj+vEDMA0GCSqGSIb3DQEBCwUA\n" \
"MDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQwwCgYDVQQD\n" \
"EwNSMTEwHhcNMjUwNDIyMjA1OTMwWhcNMjUwNzIxMjA1OTI5WjAfMR0wGwYDVQQD\n" \
"DBQqLnMxLmV1LmhpdmVtcS5jbG91ZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n" \
"AQoCggEBAKVuz2sMPmxx2w/f81/YAEKTbNZMJPk2+ooLFg5hxXvReF+AwIT4XvZ+\n" \
"MLhSKvFxmghJF+BB9WyhqrcJLGDCP4s6SOLWTYixEoTcaLUviqqn+06kYqDJ6E83\n" \
"NGsc7T42DlPnzqcZZjPRed9rt4CP3RgeZlWyYZgiD8FoJG9gie8ytihF/FkGZT8T\n" \
"N4Vkl2vQa3mfBWeeKrcuhcLPxqIWDz/30iYfLtEe5JYYScoCKTXcP9SUStjpR8pD\n" \
"vfOWdvasOAuBy7yBbx01/4lcQt50hfbhTR/K14/D4rNkuuvU7ktSQnoxVXC8YDwG\n" \
"zkny10DFt65mVYLNZcBQtOLHHOZGV30CAwEAAaOCAlwwggJYMA4GA1UdDwEB/wQE\n" \
"AwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIw\n" \
"ADAdBgNVHQ4EFgQUgsEjDU35+EWJKBsFxJ0lM0PXMi4wHwYDVR0jBBgwFoAUxc9G\n" \
"pOr0w8B6bJXELbBeki8m47kwVwYIKwYBBQUHAQEESzBJMCIGCCsGAQUFBzABhhZo\n" \
"dHRwOi8vcjExLm8ubGVuY3Iub3JnMCMGCCsGAQUFBzAChhdodHRwOi8vcjExLmku\n" \
"bGVuY3Iub3JnLzAzBgNVHREELDAqghQqLnMxLmV1LmhpdmVtcS5jbG91ZIISczEu\n" \
"ZXUuaGl2ZW1xLmNsb3VkMBMGA1UdIAQMMAowCAYGZ4EMAQIBMC0GA1UdHwQmMCQw\n" \
"IqAgoB6GHGh0dHA6Ly9yMTEuYy5sZW5jci5vcmcvNC5jcmwwggEFBgorBgEEAdZ5\n" \
"AgQCBIH2BIHzAPEAdgAN4fIwK9MNwUBiEgnqVS78R3R8sdfpMO8OQh60fk6qNAAA\n" \
"AZZfgg0JAAAEAwBHMEUCIQCENUD4FWITFwnyxsOr4D54wR+LUgZyEjwMd+GwHiha\n" \
"agIgOdeXyofPYtzl2DajwNvR+6XbCikAbbQvZTZ4Eahu2coAdwDM+w9qhXEJZf6V\n" \
"m1PO6bJ8IumFXA2XjbapflTA/kwNsAAAAZZfghU/AAAEAwBIMEYCIQDu8/zVPYFl\n" \
"bmd1vt5Fqk0sXJLV+MEFhQH75Kn6jlvtFgIhAOA8DAE1QBWXxmYSyFXw9UvC4EvH\n" \
"4+VR1cA8merS5vl4MA0GCSqGSIb3DQEBCwUAA4IBAQBVET3hPDZX/protLVPy/vX\n" \
"4i41k5J3teGokrEMu/TdMN6i/W7555Vsgl1zXj5a1f+4FsQ2Nfh1sDMuz/Djzgxp\n" \
"M8HMifB5HJTX+slAuElLzlQFCxMVNn3+b4BgpxvwA3srrXGudF3cya0qztg5lNju\n" \
"y6zAjYfxMQA0uHtCSuxKk033uFkeBv1ui3XWC1JcISbsoF47RVBp/a5O3kBr+j18\n" \
"k5qL7dWcKWr2S9JctGCH4ezYNmAG9W6w/KoTHH3HJCWrTzziJutY48Rwt4gJcS1s\n" \
"OSV8OT5pGKVpVnKSSOz4ItIaqis6fdetTiba38lUyzjDNklYL72Ye4Ck+qvjyc33\n" \
"-----END CERTIFICATE-----\n" \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFBjCCAu6gAwIBAgIRAIp9PhPWLzDvI4a9KQdrNPgwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw\n" \
"WhcNMjcwMzEyMjM1OTU5WjAzMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n" \
"RW5jcnlwdDEMMAoGA1UEAxMDUjExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n" \
"CgKCAQEAuoe8XBsAOcvKCs3UZxD5ATylTqVhyybKUvsVAbe5KPUoHu0nsyQYOWcJ\n" \
"DAjs4DqwO3cOvfPlOVRBDE6uQdaZdN5R2+97/1i9qLcT9t4x1fJyyXJqC4N0lZxG\n" \
"AGQUmfOx2SLZzaiSqhwmej/+71gFewiVgdtxD4774zEJuwm+UE1fj5F2PVqdnoPy\n" \
"6cRms+EGZkNIGIBloDcYmpuEMpexsr3E+BUAnSeI++JjF5ZsmydnS8TbKF5pwnnw\n" \
"SVzgJFDhxLyhBax7QG0AtMJBP6dYuC/FXJuluwme8f7rsIU5/agK70XEeOtlKsLP\n" \
"Xzze41xNG/cLJyuqC0J3U095ah2H2QIDAQABo4H4MIH1MA4GA1UdDwEB/wQEAwIB\n" \
"hjAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwEgYDVR0TAQH/BAgwBgEB\n" \
"/wIBADAdBgNVHQ4EFgQUxc9GpOr0w8B6bJXELbBeki8m47kwHwYDVR0jBBgwFoAU\n" \
"ebRZ5nu25eQBc4AIiMgaWPbpm24wMgYIKwYBBQUHAQEEJjAkMCIGCCsGAQUFBzAC\n" \
"hhZodHRwOi8veDEuaS5sZW5jci5vcmcvMBMGA1UdIAQMMAowCAYGZ4EMAQIBMCcG\n" \
"A1UdHwQgMB4wHKAaoBiGFmh0dHA6Ly94MS5jLmxlbmNyLm9yZy8wDQYJKoZIhvcN\n" \
"AQELBQADggIBAE7iiV0KAxyQOND1H/lxXPjDj7I3iHpvsCUf7b632IYGjukJhM1y\n" \
"v4Hz/MrPU0jtvfZpQtSlET41yBOykh0FX+ou1Nj4ScOt9ZmWnO8m2OG0JAtIIE38\n" \
"01S0qcYhyOE2G/93ZCkXufBL713qzXnQv5C/viOykNpKqUgxdKlEC+Hi9i2DcaR1\n" \
"e9KUwQUZRhy5j/PEdEglKg3l9dtD4tuTm7kZtB8v32oOjzHTYw+7KdzdZiw/sBtn\n" \
"UfhBPORNuay4pJxmY/WrhSMdzFO2q3Gu3MUBcdo27goYKjL9CTF8j/Zz55yctUoV\n" \
"aneCWs/ajUX+HypkBTA+c8LGDLnWO2NKq0YD/pnARkAnYGPfUDoHR9gVSp/qRx+Z\n" \
"WghiDLZsMwhN1zjtSC0uBWiugF3vTNzYIEFfaPG7Ws3jDrAMMYebQ95JQ+HIBD/R\n" \
"PBuHRTBpqKlyDnkSHDHYPiNX3adPoPAcgdF3H2/W0rmoswMWgTlLn1Wu0mrks7/q\n" \
"pdWfS6PJ1jty80r2VKsM/Dj3YIDfbjXKdaFU5C+8bhfJGqU3taKauuz0wHVGT3eo\n" \
"6FlWkWYtbt4pgdamlwVeZEW+LM7qZEJEsMNPrfC03APKmZsJgpWCDWOKZvkZcvjV\n" \
"uYkQ4omYCTX5ohy+knMjdOmdH9c7SpqEWBDC86fiNex+O0XOMEZSa8DA\n" \
"-----END CERTIFICATE-----"


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


// mqtt
static void mqtt_task(void* args);
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
    xTaskCreate(wifi_apsta_mode_driver_manager, "wifi apsta manager", 4096, NULL , 3 , NULL );

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


    
    // MQTT init

    // vTaskDelay(pdMS_TO_TICKS(10000)); // wait 10s

    // ESP_LOGI(MQTT_TAG,"PEM: \n%s", MQTT_BROKER_EMQX_CERTIFICATE);
    // if(mqtt_client_handle_client_init(MQTT_BROKER_URI, NULL, 0, MQTT_BROKER_USER, MQTT_BROKER_PASS)) return;

    ESP_LOGI(MQTT_TAG,"PEM: \n%s", MQTT_BROKER_HIVEMQ_CERTIFICATE);
    if(mqtt_client_handle_client_init(MQTT_BROKER_URI, MQTT_BROKER_HIVEMQ_CERTIFICATE, 0, MQTT_BROKER_USER, MQTT_BROKER_PASS)) return;
    // if(mqtt_client_handle_client_init(MQTT_BROKER_URI, NULL, 0, MQTT_BROKER_USER, MQTT_BROKER_PASS)) return;

    // ESP_LOGI(MQTT_TAG,"mqtt://mqtt.eclipseprojects.io");
    // if(mqtt_client_handle_client_init(MQTT_BROKER_URI, NULL, 0, NULL, NULL)) return;

    
    // regist event task
    if(mqtt_client_handle_regist_receive_event_task()) return;

    // start send
    xTaskCreate(mqtt_task, "mqtt send hello", 2048, NULL , 3 , NULL );


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
        vTaskDelay(pdMS_TO_TICKS(2000));
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



// MQTT TASK
static void mqtt_task(void* args)
{
    mqtt_client_command_state_t busy = MQTT_CLIENT_COMMAND_EXECUTED;


    if(mqtt_client_handle_client_start(&busy)) goto go_exit_mqtt_task;
    
    int ret = 1;

    while(ret)
    {
        // retry subcribe topic
        ESP_LOGW(MQTT_TAG, "Try subcribe");

        ret = mqtt_client_handle_client_subcribe_topic("hello esp32", MQTT_PUBSUB_QOS0);

        fprintf(stderr,"\n subcribe ret: [%d]\n", ret);
        
        // try reconnect
        if(mqtt_client_handle_client_reconnect(&busy)) goto go_exit_mqtt_task;


        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    
    // try send hello value
    char num_send[10] = {0}; // <<< number int need at list 6 value to save

    for(int i = 0; i< 10; i++)
    {
        ret = 1;

        snprintf(num_send, sizeof(num_send), "%d", (int8_t)i); // auto add \0

        ESP_LOGW(MQTT_TAG, "Try publish");

        while(ret)
        {
            // retry subcribe topic
            ret = mqtt_client_handle_client_publish_data("hello esp32", MQTT_PUBSUB_QOS0, (const char*)num_send, 0, MQTT_PUBSUB_NO_RETAIN_MSG);

            // try reconnect
            if(mqtt_client_handle_client_reconnect(&busy)) goto go_exit_mqtt_task;

            vTaskDelay(pdMS_TO_TICKS(2000));
        }
        
        ESP_LOGI(MQTT_TAG,"SUCCESS send hello esp32 = [%s]", num_send);

        if(i < 10) ESP_LOGI(MQTT_TAG, "New message in 10s");

        vTaskDelay(pdMS_TO_TICKS(10000));

    }
    

    // exit

go_exit_mqtt_task:
    ESP_LOGW(MQTT_TAG, "Mqtt task ended !!!");
    vTaskDelete(NULL);
}
