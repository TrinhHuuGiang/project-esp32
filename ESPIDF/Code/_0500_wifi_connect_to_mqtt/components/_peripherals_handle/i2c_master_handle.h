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

// note: Only communicate with only 1 device at a time
// default using i2c num 0
// use i2c master in special task to avoid busy waiting 

#ifndef _I2C_MASTER_HANDLE_H_
#define _I2C_MASTER_HANDLE_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
//c
#include <stdlib.h> // dynamic alloction
#include <stdio.h>  // print serial
#include <stdint.h> // type int

//esp32
#include "driver/i2c.h"     // I2C_NUM_ , i2c functions
#include "hal/gpio_types.h" // GPIO_NUM_

#include "esp_err.h"  // const char *esp_err_to_name(esp_err_t code);

//user
#include "_peripherals_err.h"

// Macro i2c master define
#define I2C_MASTER_PORT          CONFIG_I2C_MASTER_PORT_NUMBER //(I2C_NUM_0) i2c has 2 port: port 0 or port 1
#define I2C_MASTER_FREQ          CONFIG_I2C_MASTER_PORT_CLOCK  //(100000) 100 kHz (Standard Mode), maximun 1MHz for esp32
#define I2C_MASTER_CLOCK_FLAG    I2C_SCLK_SRC_FLAG_FOR_NOMAL   // clock source is APB clock

#define I2C_MASTER_SDA_PIN       CONFIG_I2C_MASTER_SDA_PIN        //(GPIO_NUM_21) GPIO SDA PIN
#define I2C_MASTER_SDA_PULLUP_EN CONFIG_I2C_MASTER_SDA_PULLUP_EN  //(GPIO_PULLUP_ENABLE) using internal pull-up

#define I2C_MASTER_SCL_PIN       CONFIG_I2C_MASTER_SCL_PIN        //(GPIO_NUM_22) GPIO SCL PIN
#define I2C_MASTER_SCL_PULLUP_EN CONFIG_I2C_MASTER_SCL_PULLUP_EN  //(GPIO_PULLUP_ENABLE) using internal pull-up, 

// struct save data 8bit for i2c master
typedef struct 
{
    uint8_t* list_ptr;
    uint32_t list_size;
}i2c_master_list_t;






/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

// =================================== Prepare config, free config ===================================
// =================================== =========================== ===================================

/**
 * @brief Initial master configuration struct
 * 
 * Example: 
 * 
 * i2c_config_t* master_conf = NULL;
 * 
 * i2c_master_init_config(&master_conf);
 * 
 * @note Set up macros for i2c module at "Definitions - Macro i2c master define"
 *
 * @param master_conf a pointer (i2c_config_t**) to NULL pointer (i2c_config_t*) to get value initial
 *  
 * @retval - PERIPH_OK : a pointer to struct i2c_config_t
 * @retval - I2C_MASTER_ALLOC_FAILED : if allocate failed
 */
_peripherals_err_t i2c_master_init_config(i2c_config_t* master_conf);


/**
 * @brief Free initial master configuration struct after install driver
 * 
 * @note After install driver, this struct no longer used
 * 
 * @param master_conf pointer (i2c_config_t**) to pointer (i2c_config_t*)
 * 
 * @retval - PERIPH_OK : always ok :v just free allocate
 */
_peripherals_err_t i2c_master_free_config(i2c_config_t* master_conf);





// =================================== Install & uninstall ===================================
// =================================== =================== ===================================

/**
 * @brief Set up hardware from master configuration struct
 * 
 * @note Set up pin, i2c peripheral characteristics from master configuration struct
 * 
 * @param i2c_config_t* Pointer from  master_init_config()
 * 
 * @retval - PERIPH_OK : setup ok
 * @retval - I2C_MASTER_SETUP_FAILED : setup failed
 */
_peripherals_err_t i2c_master_setup_hardware(const i2c_config_t* master_conf);

/**
 * @brief Install driver to start
 * 
 * @note Install driver after the I2C driver is configured (setup hardware)
 * 
 * @param i2c_config_t* Pointer from  master_init_config()
 * 
 * @retval - PERIPH_OK : install success
 * @retval - I2C_MASTER_INSTALL_FAILED : failed
 * 
 */
_peripherals_err_t i2c_master_install_driver(const i2c_config_t* master_conf);


/**
 * @brief Delete driver to stop
 * 
 * @note Delete driver after no longer used
 * 
 * @param None
 * 
 * @retval I2C_MASTER_UNINST_FAILED : if failed
 * @retval PERIPH_OK : if success
 */
_peripherals_err_t i2c_master_un_install_driver();





// =================================== Exchange data ===================================
// =================================== ============== ===================================

/**
 * @brief Send array of 8bit commands have prepared
 * 
 * @note - Sort all execute command in order from low index to high index
 * @note - I2C clock 100KHz -> (1/100k)s * 1000times * 9bit = 0.09ms for 1byte+1ack
 * @note - should calculate suitable waiting time
 * 
 * @param adr_7bit Almost i2c device have 7bit address (some have 10bit :v)
 * @param i2c_master_list_t a struct include 8bit list data and size of list
 * @param time_out unit in milliseconds
 * 
 * @retval I2C_MASTER_SEND_COMMAND_FAILED : if failed
 * @retval PERIPH_OK : if success
 */
_peripherals_err_t i2c_master_send_command_to_7bit_addr(uint8_t adr_7bit, i2c_master_list_t list_command, TickType_t time_out);

/**
 * @brief Get array of 8bit data from slave
 * 
 * @note - Sort all data get by time in order from low index to high index
 * @note - Input number of data, prepare array size to get into struct i2c_master_list_t
 * 
 * @param adr_7bit Almost i2c device have 7bit address (some have 10bit :v)
 * @param i2c_master_list_t* a pointer to a struct pointer i2c_master_list_t
 * @param timeout
 * 
 * @retval I2C_MASTER_GET_DATA_FAILED : if failed
 * @retval PERIPH_OK : if success
 */
_peripherals_err_t i2c_master_get_data_from_7bit_addr(uint8_t adr_7bit, i2c_master_list_t* list_command, TickType_t time_out);


#endif