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

// Note: 
// + in menuconfig, go to: 
//      - HTTP_Server and change "Max HTTP Request Header Length"
//      - default 512 something quite small with website header now

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

#include "wifi_setup_handle.h"

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
// add your inform uri-method-function-context (data included)- context size by byte
_peripherals_err_t http_server_handle_append_uri_struct_to_list(const char* uri, httpd_method_t method, 
    http_uri_function_handler_t function_handler, void* user_ctx, int context_size_byte);

// http server register list uri to handler
_peripherals_err_t http_server_handle_regist_uri_list_to_handler();

// http server stop / auto free linked list
_peripherals_err_t http_server_handle_stop_server();


#endif