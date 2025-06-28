/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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