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
// => but still declare 2 api get and release, it will reduce code generate by compiler because
//    each time we only call function pointer (call function). It help reduce flash for code.
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
    
    // Internal pheriph is lowest layer of "Project Layered Architecture", see README
    // so only these mutex is enough for safe and reduce wasted ram

    //  mutex Layer 2.1 "internal pheriph"
    SemaphoreHandle_t adc_setup_handle_mutex;
    SemaphoreHandle_t i2c_master_handle_mutex;
    SemaphoreHandle_t spi_master_handle_mutex;
    SemaphoreHandle_t gpio_setup_handle_mutex;
    SemaphoreHandle_t ledc_setup_handle_mutex;
    SemaphoreHandle_t mcpwm_setup_handle_mutex;
    SemaphoreHandle_t i2s_master_output_mutex;
    // Wifi tasks tools
    SemaphoreHandle_t wifi_state_mutex;
    SemaphoreHandle_t wifi_manager_apsta_mutex;

    // mutex Layer 2.4 "network protocols" 
    // MQTT tasks tools
    SemaphoreHandle_t mqtt_state_mutex;
    
} __task_sync_t;


typedef enum
{
    TSYNC_OK,
    TSYNC_ALLOC_STRUCT_FAILED,

    // layer 2.1
    TSYNC_MU_ADC_FAILED,
    TSYNC_MU_I2C_M_FAILED,
    TSYNC_MU_SPI_M_FAILED,
    TSYNC_MU_GPIO_FAILED,
    TSYNC_MU_LEDC_FAILED,
    TSYNC_MU_MCPWM_FAILED,
    TSYNC_MU_I2S_MO_FAILED,
    // wifi
    TSYNC_MU_WIFI_STATE_FAILED,
    TSYNC_MU_WIFI_MAN_APSTA_FAILED,

    // layer 2.4
    TSYNC_MU_MQTT_STATE_FAILED
} init_tasksync_t;

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