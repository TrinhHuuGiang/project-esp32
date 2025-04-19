#ifndef _HTTP_SERVER_HANDLE_H_
#define _HTTP_SERVER_HANDLE_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

//c
#include <stdlib.h> // Dynamic allocation
#include <stdint.h> // type int
#include <string.h>
#include <stdbool.h>

//esp32
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_http_server.h"


//user
#include "_peripherals_err.h"

// reference
#include "utlist.h"

typedef struct // struct to using linkedlist
{
    httpd_uri_t httpd_uri_inform;
    struct uri_list_http_uri* next;
} http_server_uri_list_t;

typedef struct // struct manage http server
{
    httpd_handle_t server_handle;              // manage http server lifecycle
    http_server_uri_list_t* uri_list_registed; // manage uri binding with funciton handler
} http_server_manager_t;

typedef esp_err_t (*http_uri_function_handler_t)(httpd_req_t *r); // function poniter type to callback uri handler of "httpd_uri_t"


/**
 * **********************************************************
 * APIs
 * **********************************************************
 */
// =================================== Basic html funtion          ===================================
// =================================== ==========================  ===================================

// http server start
_peripherals_err_t http_server_handle_start_server();

// http linked list (uri - function handler)
_peripherals_err_t http_server_handle_append_uri_struct_to_list(httpd_uri_t* uri_struct);

// http server register list uri to handler
_peripherals_err_t http_server_handle_regist_uri_list_to_handler();

// http server stop / auto free linked list
_peripherals_err_t http_server_handle_stop_server();


#endif