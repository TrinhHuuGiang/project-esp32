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


// DS3231 library for DS3231 rtc module
// address available 1101000 (0x68)
// this libraries only interact with 16 first register of DS3231. (ignore 10H-> 12H for temperature function)
#ifndef _I2C_DS3231_H_
#define _I2C_DS3231_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include <stdint.h>
#include <string.h>

#include "i2c_master_handle.h"

#define I2C_DS3231_MAX_ADDRESS (16U)      // only hanlde 16 first address
#define I2C_DS3231_UNIQUE_ADDRESS (0x68)  // ds3231 only have address x68

typedef struct
{
    uint8_t reg[I2C_DS3231_MAX_ADDRESS];
} ds3231_reg_t;

// time out send data
#define I2C_DS3231_READ_DATA_TIMEOUT (100)    // DS3231 have 19 address from 0x00 -> 0x12
                                            // -> require 2 ms ~ 1/100k * (19+1) * (8+1) = 1.8ms
                                            // double require time:) this time suitable for case send max data
                                            // after test, 4ms not enough, must > 10ms
                                            // 100ms for sure :)
// 16 registers address from 00H-> 0FH (these are indexs of table and address of register)
#define I2C_DS3231_REG_SEC_ADDR         (0U)//0x00
#define I2C_DS3231_REG_MIN_ADDR         (1U)
#define I2C_DS3231_REG_HOUR_ADDR        (2U)
#define I2C_DS3231_REG_DAY_ADDR         (3U)
#define I2C_DS3231_REG_DATE_ADDR        (4U)
#define I2C_DS3231_REG_MONTH_CENT_ADDR  (5U)
#define I2C_DS3231_REG_YEAR_ADDR        (6U)
#define I2C_DS3231_REG_A1_SEC_ADDR      (7U)
#define I2C_DS3231_REG_A1_MIN_ADDR      (8U)
#define I2C_DS3231_REG_A1_HOUR_ADDR     (9U)
#define I2C_DS3231_REG_A1_DAY_DATE_ADDR (10U)
#define I2C_DS3231_REG_A2_MIN_ADDR      (11U)
#define I2C_DS3231_REG_A2_HOUR_ADDR     (12U)
#define I2C_DS3231_REG_A2_DAY_DATE_ADDR (13U)
#define I2C_DS3231_REG_CONTROL_ADDR     (14U)
#define I2C_DS3231_REG_CONTROL_STATUS_ADDR (15U)//0x0F

// type of time/command from 16 registers(using choose type of time and control)
#define I2C_DS3231_REG_SEC         (1U << 0)
#define I2C_DS3231_REG_MIN         (1U << 1)
#define I2C_DS3231_REG_HOUR        (1U << 2)
#define I2C_DS3231_REG_DAY         (1U << 3)
#define I2C_DS3231_REG_DATE        (1U << 4)
#define I2C_DS3231_REG_MONTH       (1U << 5)
#define I2C_DS3231_REG_YEAR        (1U << 6) 
#define I2C_DS3231_REG_A1_SEC      (1U << 7)
#define I2C_DS3231_REG_A1_MIN      (1U << 8)
#define I2C_DS3231_REG_A1_HOUR     (1U << 9)
#define I2C_DS3231_REG_A1_DAY_DATE (1U << 10)
#define I2C_DS3231_REG_A2_MIN      (1U << 11)
#define I2C_DS3231_REG_A2_HOUR     (1U << 12)
#define I2C_DS3231_REG_A2_DAY_DATE (1U << 13)
#define I2C_DS3231_REG_CONTROL     (1U << 14)
#define I2C_DS3231_REG_CONTROL_STATUS (1U << 15)

// Alarm
#define I2C_DS3231_NO_ALARM     (0b00)
#define I2C_DS3231_BOTH_ALARM   (0b11)
#define I2C_DS3231_ALARM_1_ONLY (0b01)
#define I2C_DS3231_ALARM_2_ONLY (0b10)

// Year offset limit
#define I2C_DS3231_LOWEST_OFFSET_YEAR  (1900U)
#define I2C_DS3231_HIGHEST_OFFSET_YEAR (2000U)

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */


// --------------------Init & de-init----------------------
//============================== ============================== ==============================
//============================== ============================== ==============================

/**
 * @brief initial a register table truct to save and send data
 * 
 * @note input a pointer to NULL pointer type ds3231_reg_t*, then funtion will allocate by calloc
 * 
 * @param ds3231_reg_table* pointer to struct ds3231 register table
 * 
 * @retval 0 if ok, !0 if like: can't alloc
 */
uint8_t i2c_ds3231_init_reg_table(ds3231_reg_t* ds3231_reg_table);

/**
 * @brief De-init register table
 * 
 * @param ds3231_reg_table* pointer to pointer struct ds3231 register table
 */
void i2c_ds3231_clear_reg_table(ds3231_reg_t* ds3231_reg_table);

// --------------------Read time functions----------------------

/**
 * @brief Read 16 first register data from ds3231
 * @param ds3231_reg_table reg table after init
 * @retval 0 if ok, !0 if something wrong like: time out, can't alloc
 */
uint8_t i2c_ds3231_read_full_reg( ds3231_reg_t* ds3231_reg_table);


/**
 * @brief Translate 1 data time register (ignore control, control/status) to 1 decimal
 * @note Example I2C_DS3231_REG_SEC to get second (decimal uint16_t)
 * @note only input from I2C_DS3231_REG_SEC to I2C_DS3231_REG_A2_DAY_DATE
 * @param type_time input from I2C_DS3231_REG_SEC to I2C_DS3231_REG_A2_DAY_DATE
 * @param return_value you need create a variable uint16_t then input it pointer to get value after convert to decimal (highest value is year, from 1900 - 2099)
 * @retval 0 if ok, !0 if something wrong like : type_time not available
 */
uint8_t i2c_ds3231_translate_reg_to_time(ds3231_reg_t ds3231_reg_table, uint16_t type_time, uint16_t* return_value);






// --------------------Write time functions----------------------
//============================== ============================== ==============================
//============================== ============================== ==============================

/**
 * @brief Write data decimal to ds3231 truct register table (ignore control, control/status)
 * 
 * @note Input decimal, and type of time from I2C_DS3231_REG_SEC to I2C_DS3231_REG_A2_DAY_DATE
 * @note format 12hour AM/PM will auto translate to 24Hour
 * 
 * @param type_time from I2C_DS3231_REG_SEC to I2C_DS3231_REG_A2_DAY_DATE
 * 
 * @retval 0 if ok, !0 if something wrong like : type_time not available
 */
uint8_t i2c_ds3231_set_time_to_reg_table(uint16_t type_time, uint16_t dec_value);


/**
 * @brief Write data to ds3231
 * 
 * @note Using OR operator to select expected registers to write
 * @note Example (I2C_DS3231_REG_SEC | I2C_DS3231_REG_MIN) to write second and minute
 * 
 * @param request Each bit of type uint16_t corresponds to a flag that selects type of the 16 registers above
 * @retval 0 if ok, !0 if something wrong like : time out, can't alloc
 */
uint8_t i2c_ds3231_write_full_reg();







//============================== ============================== ==============================
//============================== ============================== ==============================
// --------------------Control square wave functions----------------------
// Will disable square wave for using alarm mode

// --------------------Control Alarm functions----------------------
// We use default when powered by Vcc - this suit table with alarm mode
// Control register :
// - bit[7:3] = 0 == Vbat mode maintain oscillation [7] and disable SQW [6], no update temperature [5]
// - bit[2] = 1 == interrupt alarm mode
// - bit[1] = 0 disable alarm 2, bit[0] = 0 disable alarm 1

// Control/status register:
// - bit[7] : status of Vbat mode osillator
// - bit[6:4]: always 0
// - bit[3]  = 1 so enable 32kHz out of pin 32K, set 0 if not using this source
// - bit[2:1] is flag for bit[5] and bit[1:0]


// set thời gian và các bit chế độ báo thức trên các thanh thời gian và kích hoạt bit [1:0]
/**
 * @brief Enable or disable alarm
 * 
 * @note Enable: clear alarm flag (status), set alarm enable bit (control)
 * @note Disable: clear alarm flag (status), clear alarm enable bit (control)
 * @note Macro: 4 macro form I2C_DS3231_NO_ALARM to I2C_DS3231_ALARM_2_ONLY
 * 
 * @param alarm_num see alarm macro
 * 
 * @retval 0 if ok, !0 if something wrong like : time out, can't alloc
 */
uint8_t i2c_ds3231_enable_disable_alarm(uint8_t alarm_num);

/**
 * @brief check alarm flag
 * 
 * @note Check: alarm flag
 * @note alarm_flag return: 0 (no flag set), 1 (flag 1 is set), 2 (flag 2 is set), 3 (both flag are set)
 * @param alarm_n_flag create a uint8_t then input it pointer
 * 
 * @retval 0 if ok, !0 if something wrong like : time out, can't alloc
 */
uint8_t i2c_ds3231_check_alarm_status(uint8_t* alarm_flag);

#endif