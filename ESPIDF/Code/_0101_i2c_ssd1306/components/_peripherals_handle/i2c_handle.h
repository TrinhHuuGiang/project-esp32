#ifndef _I2C_HANDLE_H_
#define _I2C_HANDLE_H_

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
#define I2C_MASTER_PORT          I2C_NUM_0         // i2c has 2 port: port 0 or port 1
#define I2C_MASTER_FREQ          100000            // 100 kHz (Standard Mode), maximun 1MHz for esp32
#define I2C_MASTER_CLOCK_FLAG    I2C_SCLK_SRC_FLAG_FOR_NOMAL   // clock source is APB clock

#define I2C_MASTER_SDA_PIN       GPIO_NUM_21         // GPIO SDA PIN
#define I2C_MASTER_SDA_PULLUP_EN GPIO_PULLUP_ENABLE  // using internal pull-up

#define I2C_MASTER_SCL_PIN       GPIO_NUM_22         // GPIO SCL PIN
#define I2C_MASTER_SCL_PULLUP_EN GPIO_PULLUP_ENABLE  // using internal pull-up



/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

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
 * @retval - OK : a pointer to struct i2c_config_t
 * @retval - ALLOC_FAILED : if allocate failed
 */
_peripherals_err_t i2c_master_init_config(i2c_config_t* master_conf);


/**
 * @brief Set up hardware from master configuration struct
 * 
 * @note Set up pin, i2c peripheral characteristics from master configuration struct
 * 
 * @param i2c_config_t* Pointer from  master_init_config()
 * 
 * @retval - OK : setup ok
 * @retval - SETUP_FAILED : setup failed
 */
_peripherals_err_t i2c_master_setup_hardware(i2c_config_t* master_conf);


#endif