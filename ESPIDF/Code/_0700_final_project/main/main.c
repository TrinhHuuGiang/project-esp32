/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */
#include "main_init_api.h"


/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */
#define main_tag "MAIN_LOG"


/**
 * **********************************************************
 * Main
 * **********************************************************
 */
void app_main(void)
{
    // return err code
    int ret  = 0;



    // main loop
    while(1)
    {

        // delay
        vTaskDelay(pdMS_TO_TICKS(1000)); // check system feedback
    }

// restart
main_log_restart:
    ESP_LOGW(main_tag, "Err code: %d" ,ret);
    // restart in 5s + log
    for(int i = 3; i>0;i--)
    {
        ESP_LOGW(main_tag, "Restart in: %d" ,i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // reset chip
    esp_restart();

    // ok
    return;
}
