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


#ifndef _SNTP_RTC_H_
#define _SNTP_RTC_H_

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "esp_log.h"
#include "esp_sntp.h"        // sntp wrapper handle synchronous 
#include "lwip/apps/sntp.h"  // sntp service init , stop

/**
 * @brief Start SNTP import server and timezone
 * 
 * @param tz_str   String of timezone (ex: "ICT-7" == Việt Nam)
 * @param server   domain or IP of SNTP server (ex: "pool.ntp.org")
 */
void sntp_time_sync_init(const char* tz_str, const char* server);

/**
 * @brief Wait sync time 500ms
 * 
 * @return true sync success, false if timeout and still syncing
 * */
bool sntp_time_sync_wait_500ms();

/**
 * @brief Get system time after sync
 * 
 * @param out_tm pointer to struct tm to get time
 * @return true if succes, false if time not available ( < year 2001)
 */
bool sntp_time_sync_get(struct tm* out_tm);

/**
 * @brief stop SNTP service
 */
void sntp_time_sync_stop(void);

#endif