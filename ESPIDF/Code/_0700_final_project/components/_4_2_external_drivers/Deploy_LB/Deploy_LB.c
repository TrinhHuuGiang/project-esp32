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
 * Definitions
 * **********************************************************
 */
#include "Deploy_LB.h"



/**
 * **********************************************************
 * Variables
 * - const global in ESP32 keep table in flash
 *          otherwise if miss const table will copy to data area in ram
 * - include : logic block funtion pointer table, logic block code table
 * **********************************************************
 */

const logic_block_init_fptr logic_block_init_table[]=
{
    // test lb driver ========================================





    // default service lb driver =============================
    logic_block_test_get_battery_voltage,
    logic_block_test_play_default_audio

};




const char* logic_block_code_table[]=
{
    // test lb code   ========================================




    // default service lb code ================================
    LB_TEST_CODE_BATTERY_CHECK,
    LB_TEST_CODE_AUDIO_TEST
};






/**
 * **********************************************************
 * Definitions design general port
 * **********************************************************
 */







/**
 * **********************************************************
 * Code designed device // widget
 * **********************************************************
 */
/**
 * @brief this APIS only call 1 time after reset
 * 
 * @return !0 if failed
 * 
 *  */ 
uint8_t deploy_logic_block_from_file_config()
{
    // count number logic block
    const uint32_t logic_block_count = sizeof(logic_block_code_table) / sizeof(char*);

    

    return 0;
}
