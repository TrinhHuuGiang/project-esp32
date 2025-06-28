/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// Function
// - driver runtime and check time schedule container
//      - browse and check timer containter
//      - set time flag when timer time out
//
// - API handle config schedule container
//      - regiter a node timer and get pointer to timer node
//      - order time and set up timer
//      - check timeout
//      - set flag timeout timer if user want stop timer check
//
// - Note: not supply a API delete timer, container only get
//      bigger when register new timer
// - Advantage: Prevent heap fragmentation when destroys timer
// - Disadvantage: 
//      The container can get big and slow down because it uses linked list traversal.
//      When there is a new update from the system, you have to reset to reload from the beginning.
//
// - Note: To simplify the system I will not provide api to modify the file system
//      So after reset all timer state will clear




#ifndef _WIDDR_TIME_SCHEDULER_H_
#define _WIDDR_TIME_SCHEDULER_H_



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
#include <time.h>


// esp32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"


// user define
#include "_peripherals_err.h"
#include "__task_sync.h"
#include "_priority.h"

#include "container.h"

#include "i2c_ds3231.h"

// refer
#include "utlist.h"


#define WIDDR_TIME_TASK_STACK  TASK_STACK_SIZE_MEDIUM // 
#define WIDDR_TIME_TASK_PRIO   TASK_PRIO_NORMAL       //  
#define WIDDR_TIME_TASK_SLEEP  1000 // ms  -> check time each second



/**
 * **********************************************************
 * APIs
 * **********************************************************
 */


// Start the time scheduler task, only call 1 time
uint8_t widDr_time_scheduler_start_task(void);




// Register a timer, returns pointer to timer slot
node_time_schedule_t* widDr_time_scheduler_register_a_timer_slot();





/**
 * @brief set time to timer then start schedule
 * @retval !0 if slot not available
 */
uint8_t widDr_time_scheduler_set_time_from_components(
    node_time_schedule_t* slot,
    int year, int month, int day,
    int hour, int min, int sec);



/** Check if timer timeout flag is set (does not clear it), return false if not time out or input not
available slot*/
bool widDr_time_scheduler_check_timeout(node_time_schedule_t* slot);




// Set the timeout flag manually
void widDr_time_scheduler_turnoff_timer(node_time_schedule_t* slot);







#endif // _WIDDR_TIME_SCHEDULER_H_
