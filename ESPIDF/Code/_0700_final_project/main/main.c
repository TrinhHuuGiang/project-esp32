/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */
#include "main_init_api.h"
#define main_tag "MAIN_LOG"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */ 
extern __task_sync_t* g_task_sync_tools; // get sync tools 


/**
 * **********************************************************
 * Main
 * **********************************************************
 */
void app_main(void)
{
/** ==========================================================================
 * Init Layer (See README.md)
--------------------------------------------------------------------------- */

    // return err code when init Layer 1-> 4
    int ret  = 0;

                                /** =========================================
                                * LAYER 1: System architecture infrastructure
                                ------------------------------------------- */

    if(task_sync_tools_init()) 
    {
        ret = 1;
        goto main_log_restart; // << init g_task_sync_tools
    }
    

                                /** =========================================
                                * LAYER 2: System resource
                                ------------------------------------------- */
    
    /** ======================
    * LAYER 2.1: Internal peripherals
    ------------------------ */




    /** ======================
    * LAYER 2.2: External peripherals
    ------------------------ */



    /** ======================
    * LAYER 2.3: External peripherals
    ------------------------ */



    /** ======================
    * LAYER 2.4: Network application protocol
    ------------------------ */


                                /** =========================================
                                * LAYER 3: External devices
                                ------------------------------------------- */


                                /** =========================================
                                * LAYER 4: Controller and logic
                                ------------------------------------------- */

    /** ======================
    * LAYER 4.1: System driver
    ------------------------ */



    /** ======================
    * LAYER 4.2: System driver
    ------------------------ */
    
    // Widget driver display performance
    
    

    // Widget driver time realtime


    


    

/** ==========================================================================
 * Main loop check err flag
--------------------------------------------------------------------------- */

    // main loop until an error feedback by err_flag (set by some widget, driver, or user code)
    while(!g_task_sync_tools->err_flag)
    {
        // delay
        vTaskDelay(pdMS_TO_TICKS(1000)); // check system feedback

        // log before check flag
        ESP_LOGI(main_tag, "main check");
    }



/** ----------------------------------------------------------------------------
 * Restart
 *  2 possible cases:
 *      - init failed -> ret != 0
 *      - global flag set -> unexpected error(from driver, user code)
==============================================================================*/
main_log_restart:
    if(ret)
    {
        ESP_LOGW(main_tag, "Err code: %d" ,ret);
    }
    else if(g_task_sync_tools->err_flag) // by widget or somewhere 
    {
        ESP_LOGW(main_tag, "SYSTEM ERR FLAG SET !!!");
    }

    // restart in 5s + log
    for(int i = 5; i>0;i--)
    {
        ESP_LOGW(main_tag, "Restart in: %d" ,i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // reset chip
    esp_restart();

    // ok
    return;
}
