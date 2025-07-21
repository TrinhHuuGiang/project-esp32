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


#include "Logic_Test_design_port.h"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */

extern __task_sync_t* g_task_sync_tools; // get sync tools 



/**
 * **********************************************************
 * Definitions general
 * **********************************************************
 */

/**
 * @brief Initialize a driver logic block control a port of spi bus
 */
uint8_t logic_block_test_spi_port(cJSON* config);

/**
 * @brief Initialize a driver logic block control a port of i2c bus
 */
uint8_t logic_block_test_i2c_port(cJSON* config);

/**
 * @brief Initialize a driver logic block control a port of 75hc595 output
 */
uint8_t logic_block_test_595_logic_out_port(cJSON* config);

/**
 * @brief Initialize a driver logic block control a port of i2s sound output
 */
uint8_t logic_block_test_i2s_port(cJSON* config);


/**
 * @brief Initialize a driver logic block control a port of ledc pwm output
 */
uint8_t logic_block_test_ledc_port(cJSON* config);

/**
 * @brief Initialize a driver logic block control a port of 4067 analog input
 */
uint8_t logic_block_test_4067_analog_in_port(cJSON* config);

/**
 * @brief Initialize a driver logic block control a port of 75hc165 logic input
 */
uint8_t logic_block_test_165_logic_in_port(cJSON* config);


/**
 * @brief Initialize a driver logic block control a port of mcpwm
 */
uint8_t logic_block_test_mcpwm_port(cJSON* config);




/**
 * **********************************************************
 * Definitions designed device // widget
 * **********************************************************
 */

// battery check task
static void task_test_get_battery_voltage()
{
    uint8_t ret = 0;

    // register mqtt service
    


    while(!ret)
    {

        // g_task_sync_tools->err_flag = 1;

    }



    vTaskDelete(NULL);
}

/**
 * @brief Initialize a driver get battery voltage to ui application
 * 
 */
uint8_t logic_block_test_get_battery_voltage(cJSON* config)
{
    // init task
    BaseType_t ok = xTaskCreate(
        task_test_get_battery_voltage, 
        LB_TEST_BATTERY_CHECK_TASK_NAME,
        LB_TEST_BATTERY_CHECK_TASK_SIZE, NULL,
        LB_TEST_BATTERY_CHECK_TASK_PRIO, NULL
    );
    if (ok != pdPASS) return 1;

    return 0;
}





// audio test task
static void task_test_play_default_audio()
{
    uint8_t ret = 0;

    uint8_t topic_total_num = sizeof(lb_test_code_batter_check) /  sizeof(char*);
    
    // register mqtt service
    

    

    while(!ret)
    {

        // g_task_sync_tools->err_flag = 1;

    }


    vTaskDelete(NULL);
}

/**
 * @brief Initialize a driver control display a default audio
 */
uint8_t logic_block_test_play_default_audio(cJSON* config)
{
    // init task
    BaseType_t ok = xTaskCreate(
        task_test_play_default_audio, 
        LB_TEST_AUDIO_TEST_TASK_NAME,
        LB_TEST_AUDIO_TEST_TASK_SIZE, NULL,
        LB_TEST_AUDIO_TEST_TASK_PRIO, NULL
    );
    if (ok != pdPASS) return 1;

    return 0;
}