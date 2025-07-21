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

#ifndef _TASK_SYNC_TOOLS_
#define _TASK_SYNC_TOOLS_



/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include "freertos/semphr.h"

#include "stdint.h"

// Handle risk of deadlock at layer `internal peripherals`:
// - Solution 1: To avoid deadlock at layer `Internal Pheriph`, mutex of these peripheral
// have been design working in API and release when out API.
//      + Any tasks call 1 (the same) API will block untill first talk run API success.
//      + Nerver reach case: call 2 `Internal Pheriph mutex` at a time <= thread safe

// - Solution 2: 1 API get + 1 API release mutex -> reduce Reduced complexity of 
// synchronization management but more frequency call get/ release
//
// => chose solution 1, just design mutex in function handle, user no need 
//    to worry about when and which function should use mutex
//
// Other case: 
// - Avoid design 'nested mutex locking' by order mutex between task is the same form
// - Avoid design 'recursive mutex locking' by only call 1 mutex in a function
// - ...

typedef struct 
{
    // ====================== Internal Pheriph ==========================
    // These mutex ensure that only 1 API acts on 1 internal peripheral type 
    // at a time to avoid conflicts

    // i2c
    SemaphoreHandle_t i2c_master_handle_mutex;
    SemaphoreHandle_t spi_master_handle_mutex;
    SemaphoreHandle_t gpio_setup_handle_mutex;
    SemaphoreHandle_t ledc_setup_handle_mutex;
    SemaphoreHandle_t mcpwm_setup_handle_mutex;
    SemaphoreHandle_t i2s_master_handle_mutex;

    // Wifi tasks tools
    SemaphoreHandle_t wifi_state_mutex;
    SemaphoreHandle_t wifi_manager_apsta_mutex;




    // ====================== Protocol ==========================

    // MQTT tasks tools
    SemaphoreHandle_t mqtt_state_mutex;
    
} __task_sync_t;


/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

// init tools
uint8_t task_sync_tools_init();


// :v no deinit, sync tool should live with program
// reset chip when update something new

#endif