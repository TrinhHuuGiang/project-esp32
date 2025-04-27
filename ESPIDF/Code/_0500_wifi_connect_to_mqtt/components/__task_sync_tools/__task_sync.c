/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

#include "__task_sync.h"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
__task_sync_t* g_task_sync_tools = NULL;


/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// mutex:
// + xSemaphoreCreateMutex()
// +  xSemaphoreTake()
// + xSemaphoreGive()
// +  vSemaphoreDelete()


// init tools
uint8_t task_sync_tools_init()
{
    // wifi
    g_task_sync_tools->wifi_state_mutex = xSemaphoreCreateMutex();
    if(g_task_sync_tools->wifi_state_mutex == NULL) return 1;

    // mqtt
    g_task_sync_tools->mqtt_state_mutex = xSemaphoreCreateMutex();
    if(g_task_sync_tools->wifi_state_mutex == NULL) return 2;

    //ok
    return 0;
}
