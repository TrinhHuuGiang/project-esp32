#ifndef _TASK_SYNC_TOOLS_
#define _TASK_SYNC_TOOLS_



/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include <stdio.h>

#include "freertos/semphr.h"

#include "stdint.h"

typedef struct 
{
    // Wifi tasks tools
    SemaphoreHandle_t wifi_state_mutex;

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