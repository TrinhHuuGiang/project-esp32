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
