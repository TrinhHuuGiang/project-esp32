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

typedef struct 
{
    // Wifi tasks tools
    SemaphoreHandle_t wifi_state_mutex;
    SemaphoreHandle_t wifi_manager_apsta_mutex;

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


// :) no deinit, sync tool should live with program

#endif