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

#include "rtc_sntp.h"


static const char* TAG = "SNTP";

void sntp_time_sync_init(const char* tz_str, const char* server)
{
    ESP_LOGI(TAG, "Start SNTP...");

    // Setup timezone
    if (tz_str) {
        setenv("TZ", tz_str, 1);
        tzset();
    }

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, server);  // ex: "pool.ntp.org"
    sntp_init();
}

bool sntp_time_sync_wait_500ms()
{
    const uint32_t check_interval_ms = 500;

    vTaskDelay(pdMS_TO_TICKS(check_interval_ms));

    return sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED;
}

bool sntp_time_sync_get(struct tm* out_tm)
{
    if (!out_tm) return false;

    time_t now = 0;
    time(&now);

    if (now < 1000000000UL) // before 2001, not yet synchronized
        return false;

    localtime_r(&now, out_tm);
    return true;
}

void sntp_time_sync_stop(void)
{
    sntp_stop();
}
