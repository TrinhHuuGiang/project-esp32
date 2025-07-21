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


// library providing api to call driver running parallel with main task
// automatically check network time and update to ds3231 then finish
// Fatal failures can be reported via the "__task_sync_t->err_flag"
//      found in "__task_sync.h"
//      handle it in mainloop
#ifndef _WIDDR_REALTIME_H_
#define _WIDDR_REALTIME_H_
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
#include "freertos/semphr.h"

// user define


#include "_peripherals_err.h"
#include "__task_sync.h"

#include "_priority.h"

#include "rtc_sntp.h"
#include "i2c_ds3231.h"



// time zone
#define WIDDR_REALTIME_TIMEZONE  CONFIG_WID_REALTIME_TIMEZONE
#define WIDDR_REALTIME_SERVER    CONFIG_WID_REALTIME_SERVER

// task size
#define WIDDR_TASK_NAME             "widDr_rtc_updater"
#define WIDDR_TASK_STACK_SIZE       TASK_STACK_SIZE_HIGH // << (overflow at) TASK_STACK_SIZE MEDIUM
                                        // LOG ERR: Guru Meditation Error: Core  0 panic'ed (StoreProhibited). Exception was unhandled.

#define WIDDR_TASK_PRIORITY         TASK_PRIO_LOW

#define WIDDR_TASK_RETRY_AFTER      1000 // 1s

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

/**
 * @brief API create a task running parallel with main task
 * @note  only call this api 1 time to avoid many tasks being 
 * @note created but unable to finish due to no internet connection
 * 
 * @retval !0 if failed
 * @retval main should check "err_flag" maybe have error (or not :v it is global flag)
 * @retval " extern __task_sync_t* g_task_sync_tools; "
 * @retval but driver task will auto end when reconnect and get new local time
 */
uint8_t widDr_update_rtc_to_DS3231();




#endif