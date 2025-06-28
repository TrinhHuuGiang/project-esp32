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
#include "widDr_time_realtime.h"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
extern __task_sync_t* g_task_sync_tools; // get sync tools 

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
// driver check sntp untill success then update into DS3231 clock
static void widDr_realtime_update(void* arg);



/**
 * **********************************************************
 * Codes
 * **********************************************************
 */
/**
 * @brief API create a task running parallel with main task
 * @note  only call this api 1 time to avoid many tasks being 
 * @note created but unable to finish due to no internet connection
 * 
 * @retval no retval, check "err_flag" maybe have error (or not :v it is global flag)
 * @retval " extern __task_sync_t* g_task_sync_tools; "
 * @retval but driver task will auto end when reconnect and get new local time
 */
uint8_t widDr_update_rtc_to_DS3231()
{
    BaseType_t ret = xTaskCreate(
        widDr_realtime_update,
        WIDDR_TASK_NAME,
        WIDDR_TASK_STACK_SIZE,
        NULL,
        WIDDR_TASK_PRIORITY,
        NULL
    );

    return (ret == pdPASS) ? 0 : 1;
}


// driver check sntp untill success then update into DS3231 clock
static void widDr_realtime_update(void* arg)
{
    // 1: Init sntp
    sntp_time_sync_init(WIDDR_REALTIME_TIMEZONE, WIDDR_REALTIME_SERVER);

    // 2: wait until success
    bool synced = false;
    while (!synced)
    {
        fprintf(stderr,"sntp checking\n");
        if (sntp_time_sync_wait_500ms())
        {
            fprintf(stderr,"rtc syncing...");
            synced = true;
            break;
        }
        // if fail
        vTaskDelay(pdMS_TO_TICKS(WIDDR_TASK_RETRY_AFTER));
    }

    // 3: get system time
    struct tm now_tm;
    if (!sntp_time_sync_get(&now_tm))
    {
        #if CONFIG_DEBUG_ENABLE !=0
            send_peripheral_err_location(PERIPH_UNKNOWN, 
                __FILE__, __LINE__, "rtc sync fail" );
        #endif

        g_task_sync_tools->err_flag = 1; // error get sync time (stop for debug)
        vTaskDelete(NULL);
        return;
    }

    // 4: write to DS3231
    ds3231_reg_t* reg = NULL;
    if (i2c_ds3231_init_reg_table((ds3231_reg_t*)&reg) != 0 || reg == NULL)
    {

        #if CONFIG_DEBUG_ENABLE !=0
            send_peripheral_err_location(PERIPH_UNKNOWN, 
                __FILE__, __LINE__, "ds3231 init failed" );
        #endif

        g_task_sync_tools->err_flag = 1; // error update time to table ds3231 (stop for debug)
        vTaskDelete(NULL);
        return;
    }

    i2c_ds3231_set_time_to_reg_table(reg, I2C_DS3231_REG_SEC,  now_tm.tm_sec);
    i2c_ds3231_set_time_to_reg_table(reg, I2C_DS3231_REG_MIN,  now_tm.tm_min);
    i2c_ds3231_set_time_to_reg_table(reg, I2C_DS3231_REG_HOUR, now_tm.tm_hour);
    i2c_ds3231_set_time_to_reg_table(reg, I2C_DS3231_REG_DAY,  now_tm.tm_wday == 0 ? 7 : now_tm.tm_wday); // tm_wday: 0=Sun -> 7
    i2c_ds3231_set_time_to_reg_table(reg, I2C_DS3231_REG_DATE, now_tm.tm_mday);
    i2c_ds3231_set_time_to_reg_table(reg, I2C_DS3231_REG_MONTH, now_tm.tm_mon + 1); // tm_mon: 0-11
    i2c_ds3231_set_time_to_reg_table(reg, I2C_DS3231_REG_YEAR, now_tm.tm_year + 1900);

    if (i2c_ds3231_write_full_reg(reg) != 0)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_UNKNOWN, 
            __FILE__, __LINE__, "ds3231 sync fail" );
        #endif

        g_task_sync_tools->err_flag = 1 ; // error update time to ds3231 (stop for debug)
    }

    i2c_ds3231_clear_reg_table((ds3231_reg_t*)&reg);

    // stop SNTP
    sntp_time_sync_stop();

    // delete task
    vTaskDelete(NULL);
}
