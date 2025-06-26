/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

#include "widDr_time_schedule.h"

static const char* TAG = "time_sched";

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */

extern __task_sync_t* g_task_sync_tools; // get sync tools 


static node_time_schedule_t* g_time_container = NULL;
static SemaphoreHandle_t g_time_mutex = NULL;


/**
 * **********************************************************
 * Prototype
 * **********************************************************
 */

// task scheduler
static void widDr_time_scheduler_task(void* arg);


/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// public: task scheduler start task
uint8_t widDr_time_scheduler_start_task(void)
{
    g_time_mutex = xSemaphoreCreateMutex();

    if (!g_time_mutex) return 1;



    BaseType_t ret = xTaskCreate(
        widDr_time_scheduler_task,
        "timeSchedulerTask",
        WIDDR_TIME_TASK_STACK,
        NULL,
        WIDDR_TIME_TASK_PRIO,
        NULL
    );
    

    return (ret == pdPASS) ? 0 : 2;
}







// register a timer slots
node_time_schedule_t* widDr_time_scheduler_register_a_timer_slot()
{
    node_time_schedule_t* node = calloc(1, sizeof(node_time_schedule_t));
    if (!node) return NULL;

    node->timeout_f = 1; // time out -> scheduler dont care it


    xSemaphoreTake(g_time_mutex, portMAX_DELAY);
    LL_APPEND(g_time_container, node);
    xSemaphoreGive(g_time_mutex);

    
    return node;
}






/**
 * @brief set time to timer then start schedule
 */
uint8_t widDr_time_scheduler_set_time_from_components(
    node_time_schedule_t* slot,
    int year, int month, int day,
int hour, int min, int sec)
{
    if (!slot) return 1; // lỗi: slot null

    xSemaphoreTake(g_time_mutex, portMAX_DELAY);

    slot->schedule_time.tm_year = year - 1900; // struct tm: year since 1900
    slot->schedule_time.tm_mon  = month - 1;   // struct tm: month 0-11
    slot->schedule_time.tm_mday = day;
    slot->schedule_time.tm_hour = hour;
    slot->schedule_time.tm_min  = min;
    slot->schedule_time.tm_sec  = sec;
    slot->schedule_time.tm_isdst = -1;

    slot->timeout_f = 0; // reset flag

    xSemaphoreGive(g_time_mutex);

    return 0; // OK
}






// check time out if driver check ds3231 seem has handled time out
bool widDr_time_scheduler_check_timeout(node_time_schedule_t* slot)
{
    if (!slot) return false;

    xSemaphoreTake(g_time_mutex, portMAX_DELAY);

    bool flag = (slot->timeout_f != 0);
    
    xSemaphoreGive(g_time_mutex);

    return flag;
}





// clear timer, some time user not wanna schedule, set timeout it then driver have free time for sleep
void widDr_time_scheduler_turnoff_timer(node_time_schedule_t* slot)
{
    if (!slot) return;

    xSemaphoreTake(g_time_mutex, portMAX_DELAY);
    slot->timeout_f = 1;
    xSemaphoreGive(g_time_mutex);

}






// Driver scheduler
static void widDr_time_scheduler_task(void* arg)
{

    ds3231_reg_t* reg = NULL;

    time_t now = 0;  // keep convert time_t from struct tm update by ds3231
                    // time_t convenient for calculate

    uint8_t ret = 0;

    if (i2c_ds3231_init_reg_table(&reg) != 0)
    {
        g_task_sync_tools->err_flag = 1; // global flag set -> main reset
        vTaskDelete(NULL);
    }


    // start scheduler    
    while (!ret)
    {
        ESP_LOGI(TAG, "Scheduler checking");
        //-------------------------------------------------------------------
        //-------------------See what is the current time?
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------

        if (i2c_ds3231_read_full_reg(reg))
        {

            i2c_ds3231_clear_reg_table(reg); //

            ret = 1; // return
            g_task_sync_tools->err_flag = 1; // global error

            goto scheduler_next_loop;

        }
        else
        {
        struct tm now_tm = {0};

        i2c_ds3231_translate_reg_to_time(*reg, I2C_DS3231_REG_SEC,  &now_tm.tm_sec);
        i2c_ds3231_translate_reg_to_time(*reg, I2C_DS3231_REG_MIN,  &now_tm.tm_min);
        i2c_ds3231_translate_reg_to_time(*reg, I2C_DS3231_REG_HOUR, &now_tm.tm_hour);
        i2c_ds3231_translate_reg_to_time(*reg, I2C_DS3231_REG_DATE, &now_tm.tm_mday);
        i2c_ds3231_translate_reg_to_time(*reg, I2C_DS3231_REG_MONTH,  &now_tm.tm_mon);
        i2c_ds3231_translate_reg_to_time(*reg, I2C_DS3231_REG_YEAR, &now_tm.tm_year);

        now_tm.tm_mon -= 1;           // struct tm: month from 0
        now_tm.tm_year -= 1900;       // struct tm: year from 1900

        now = mktime(&now_tm);
        }



        //-------------------------------------------------------------------
        //-------------------check set flags timer in container
        //-------------------------------------------------------------------
        //-------------------------------------------------------------------

        xSemaphoreTake(g_time_mutex, portMAX_DELAY);



        node_time_schedule_t* el; // << timer element
        LL_FOREACH(g_time_container, el)
        {
            if (!(el->timeout_f) && (mktime(&el->schedule_time) <= now))
            {
                el->timeout_f = 1;
                ESP_LOGI(TAG, "Timer reached: %04d-%02d-%02d %02d:%02d:%02d",
                         el->schedule_time.tm_year + 1900, el->schedule_time.tm_mon + 1,
                         el->schedule_time.tm_mday, el->schedule_time.tm_hour,
                         el->schedule_time.tm_min, el->schedule_time.tm_sec);
            }
        }



        xSemaphoreGive(g_time_mutex);

scheduler_next_loop:
        vTaskDelay(pdMS_TO_TICKS(WIDDR_TIME_TASK_SLEEP));
    }
}
