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

/**
 * **********************************************************
 * Description
 * - This library prepare task control device 
 *  and will implement by " Deploy_LB.c "
 * - Include:
 *   + test general in / out designed port
 *   + test special purpose device or widget: baterry check, player wav
 * **********************************************************
 */


#ifndef    _LOGIC_BLOCK_TEST_H_
#define   _LOGIC_BLOCK_TEST_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
// --------------> C common libs
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// --------------> Parse
#include <cJSON.h>    // parse json file


// user define
#include "__task_sync.h"
#include "_priority.h"

#include "gpio_74hc595.h"
#include "Deploy_LB.h"

// service
#include "widDr_batterycheck.h"
#include "widDr_net_mqtt_manager.h"

/**
 * **********************************************************
 * Special logic block code, use identify with application UI interface on smart phone
 * **********************************************************
 */

// general in/ out designed port
// (this code have to unique with database of config application)
#define LB_TEST_CODE_SPI_PORT                  "T_000"
#define LB_TEST_CODE_I2C_PORT                  "T_001"
#define LB_TEST_CODE_595_LOGIC_OUPUT_PORT      "T_002"
#define LB_TEST_CODE_I2S_PORT                  "T_003"
#define LB_TEST_CODE_LEDC_PORT                 "T_004"
#define LB_TEST_CODE_4067_ANALOG_INPUT_PORT    "T_005"
#define LB_TEST_CODE_165_LOGIC_INPUT_PORT      "T_006"
#define LB_TEST_CODE_MCPWM_PORT                "T_007"


// designed device
// (this code have to unique with database of config application)
#define LB_TEST_CODE_BATTERY_CHECK             "DF_000"
#define LB_TEST_CODE_AUDIO_TEST                "DF_001"


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

#define LB_TEST_BATTERY_CHECK_TASK_NAME   "test_batt"
#define LB_TEST_BATTERY_CHECK_TASK_SIZE   TASK_STACK_SIZE_LOW
#define LB_TEST_BATTERY_CHECK_TASK_PRIO   TASK_PRIO_REALTIME
#define LB_TEST_BATTERY_CHECK_TASK_DELAY  1000 // 1s then handle + feed back

typedef enum
{
    // recv
    LB_TEST_BATTERY_CHECK_TASK_TOPIC_ONOFF_MEASURE,

    // send
    LB_TEST_BATTERY_CHECK_TASK_TOPIC_FEEDBACK_VOLTAGE
};


const char* lb_test_code_batter_check[]=
{
    // recv
    "OM",

    // send
    "FV"
};


/**
 * 
 * @brief Initialize a driver get battery voltage to ui application
 */
uint8_t logic_block_test_get_battery_voltage(cJSON* config);









#define LB_TEST_AUDIO_TEST_TASK_NAME   "test_audio"
#define LB_TEST_AUDIO_TEST_TASK_SIZE   TASK_STACK_SIZE_LOW
#define LB_TEST_AUDIO_TEST_TASK_PRIO   TASK_PRIO_REALTIME
#define LB_TEST_AUDIO_TEST_TASK_DELAY  1000 // 1s then handle + feed back

typedef enum
{
    // recv
    LB_TEST_AUDIO_TEST_TASK_TOPIC_ONOFF_SPEAKER,

    // send
    LB_TEST_AUDIO_TEST_TASK_TOPIC_FEEDBACK_ONOFF
};

const char* lb_test_code_batter_check[]=
{
    // recv
    "OS",

    // send
    "FO"
};


/**
 * @brief Initialize a driver control display a default audio
 */
uint8_t logic_block_test_play_default_audio(cJSON* config);


#endif



 