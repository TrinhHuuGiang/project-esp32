/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


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
    g_task_sync_tools = calloc(1, sizeof(__task_sync_t));
    if(g_task_sync_tools == NULL) return 1;

    // wifi
    g_task_sync_tools->wifi_state_mutex = xSemaphoreCreateMutex(); // wifi state
    if(g_task_sync_tools->wifi_state_mutex == NULL) return 2;

    g_task_sync_tools->wifi_manager_apsta_mutex = xSemaphoreCreateMutex(); // wifi apsta manager
    if(g_task_sync_tools->wifi_manager_apsta_mutex == NULL) return 2;

    // mqtt
    g_task_sync_tools->mqtt_state_mutex = xSemaphoreCreateMutex();  // mqtt state
    if(g_task_sync_tools->mqtt_state_mutex == NULL) return 3;

    //ok
    return 0;
}
