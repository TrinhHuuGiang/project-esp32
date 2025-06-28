/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef _CONTAINER_UNITS_
#define _CONTAINER_UNITS_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

#include <stdint.h>
#include <time.h>

// Purpose: A node is where driver communicates with logic block (4.1 with 4.2)
// These containers may need sync tools (mutex) because they can be accessed by both driver and logic block

// Add sync at "__task_sync.h" to ensure any mutex created first after restart




// 1 structure for use in linked list for scheduling timer
// API need return pointer for logic block to find fastest
typedef enum
{
    TIME_SCHEDULER_SET_IMMEDIATE,
    TIME_SCHEDULER_SET_COUNT_DOWN
} time_type_set_t;

typedef struct node_time_schedule_t
{
    struct tm schedule_time;
    uint8_t   timeout_f;        
        // timeout flag: sey by scheduler after timeout
        // clear by logic block if adding a "schedule_time"

    struct node_time_schedule_t* next;
} node_time_schedule_t;




// 1 structure for use in linked list for saving topic mqtt
typedef struct node_mqtt_send_mes_t
{
    char* topic;
    char* message;
    uint8_t sent_f;
        // sent flag: set by logic block after update
        // cleared by driver after sending

    struct node_mqtt_send_mes_t* next;
} node_mqtt_send_mes_t;

typedef struct node_mqtt_recv_mes_t
{
    char* topic;
    char* message;
    uint8_t read_f;    
        // read flag: set by MQTT driver
        // cleared by logic block

    struct node_mqtt_recv_mes_t* next;
} node_mqtt_recv_mes_t;

#endif // _CONTAINER_UNITS_
