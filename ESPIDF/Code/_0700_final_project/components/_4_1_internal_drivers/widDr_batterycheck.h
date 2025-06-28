/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


// unavailable battery
// pin low, full, fine
// voltage higher than measure

// This is a basic driver that can be called frequently to see battery capacity.

// using: widDr_display_performance.h

#ifndef _WIDDR_BATTERY_H_
#define _WIDDR_BATTERY_H_

// about hardware:
// battery -> 1n4007 (-0.7V) -> 
//                             |100k
//                             |------------> measure point (input 0-14V) == (output 0-2.45V)
//                             |22k
//                            GND

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

// esp32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// user define
#include "_peripherals_err.h"
#include "__task_sync.h"
#include "_priority.h"


#include "adc_74hc4067.h"

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

/**
 * @brief read 74hc4067 channel 0 then translate into real voltage
 * @retval return 0 is success
 * 
 * @param value address of 'int'
 * 
 * @note this function measure follow by hardware behind, voltage range value return form 0~14V
 */
uint8_t winDr_get_battery_mV(int* value);




#endif