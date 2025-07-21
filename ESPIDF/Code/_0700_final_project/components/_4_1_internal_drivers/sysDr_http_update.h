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


// This library supple an API handle register: 
///         http uri, function handler with http server
// update logic block will be chosen by app: get JSON data, update to file: lgb_init.dat
// update wifi apsta config : get JSON data, handle then update to file: wf_cf.dat
// update mqtt cert, config : get JSON data, handle then update to file: mqttcert.pem, mqtt_cf.dat

#ifndef _SYSDIR_HTTP_UPDATE_H_
#define _SYSDIR_HTTP_UPDATE_H_
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

// esp32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"

// user define
#include "_peripherals_err.h"
#include "__task_sync.h"
#include "_priority.h"
#include "__board_inform.h"

#include "http_server_handle.h"
#include "sysDr_http_update_path.h"

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

/**
 * @brief http path, function handler to receive callback when new http request
 * 
 * - update pair http (uri, method, ...)
 * 
 * @retval return 0 is OK
 */
uint8_t sysDr_http_update_uri_and_function_handler_with_http_server();

 


#endif