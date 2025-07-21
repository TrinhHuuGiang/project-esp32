/*
 * Copyright (C) 2025 Giang Trinh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */



// update logic block will be chosen by app: get JSON data, update to file: lgb_init.dat
// update wifi apsta config : get JSON data, handle then update to file: wf_cf.dat
// update mqtt cert, config : get JSON data, handle then update to file: mqttcert.pem, mqtt_cf.dat

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "sysDr_http_update.h"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */
#define LOG_TAG "Http update"

static SemaphoreHandle_t s_http_busy_mutex = NULL; 

static uint8_t s_http_busy = 0 ; // 1 is busy

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
// handshake http get board type 
static esp_err_t get_board_type_handler(httpd_req_t *req);


// http get post json config logic block
static esp_err_t get_post_json_config_lgb_handler(httpd_req_t *req);


// http get post json config mqtt info
static esp_err_t get_post_json_config_mqtt_handler(httpd_req_t *req);


// http get post json config wifi ap
static esp_err_t get_post_json_config_wifi_ap_handler(httpd_req_t *req);


// http get post json config wifi sta
static esp_err_t get_post_json_config_wifi_sta_handler(httpd_req_t *req);

/**
 * **********************************************************
 * Code
 * **********************************************************
 */
/**
 * @brief http path, function handler to receive callback when new http request
 * 
 * - update pair http (uri, method, 
 * 
 * @retval return 0 is OK
 */
uint8_t sysDr_http_update_uri_and_function_handler_with_http_server()
{

    // Create mutex for only 1 request allowed
    s_http_busy_mutex = xSemaphoreCreateMutex();
    if (!s_http_busy_mutex) return 1;



    // HTTP GET board type
    if(http_server_handle_append_uri_struct_to_list(
        HTTP_PATH_GET_BOARD_TYPE,
        HTTP_GET,
        get_board_type_handler,
        NULL,
        0
    )) return 2;

    // HTTP POST  get json config logic block
    if(http_server_handle_append_uri_struct_to_list(
        HTTP_PATH_POST_JSON_CONFIG_LOGIC_BLOCK,
        HTTP_POST,
        get_post_json_config_lgb_handler,
        NULL,
        0
    )) return 3;

    // HTTP POST  get json config config mqtt info
    if(http_server_handle_append_uri_struct_to_list(
        HTTP_PATH_POST_JSON_CONFIG_MQTT,
        HTTP_POST,
        get_post_json_config_mqtt_handler,
        NULL,
        0
    )) return 4;


    // HTTP POST  get json config logic block
    if(http_server_handle_append_uri_struct_to_list(
        HTTP_PATH_POST_JSON_CONFIG_WIFI_AP,
        HTTP_POST,
        get_post_json_config_wifi_ap_handler,
        NULL,
        0
    )) return 5;



    // HTTP POST  get json config logic block
    if(http_server_handle_append_uri_struct_to_list(
        HTTP_PATH_POST_JSON_CONFIG_WIFI_STA,
        HTTP_POST,
        get_post_json_config_wifi_sta_handler,
        NULL,
        0
    )) return 6;



    // regist uri list to handler
    if(http_server_handle_regist_uri_list_to_handler()) return 7;


    // ok
    return 0;
}







// handshake http get board type 
static esp_err_t get_board_type_handler(httpd_req_t *req)
{
    // Get serving
    xSemaphoreTake(s_http_busy_mutex, portMAX_DELAY);

    if(s_http_busy)// check if http is serving a handler
    {
        xSemaphoreGive(s_http_busy_mutex);
        return ESP_FAIL; // notify http server refuse HTTP GET
    }
    else // set busy flag
    {
        s_http_busy = 1;
        xSemaphoreGive(s_http_busy_mutex);
    }

    
    
    // Try send board code
    
    
    
    ESP_LOGI(LOG_TAG, "Sending board code [%s]", DEV_BOARD_UNIQUE_ID);



}