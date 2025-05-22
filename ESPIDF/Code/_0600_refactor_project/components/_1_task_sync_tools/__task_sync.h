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
// - To avoid deadlock at layer `Internal Pheriph`, mutex of these peripheral
// have been design working in API and release when out API.
// - Any tasks call 1 (the same) API will block untill first talk run API success.
// - Nerver reach case: call 2 `Internal Pheriph mutex` at a time <= thread safe

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