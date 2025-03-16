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

#include "i2c_master_handle.h"

typedef struct
{
    uint8_t reg[16];
} ds3231_reg_t;

// time out send data
#define I2C_DS3231_READ_DATA_TIMEOUT (4)    // DS3231 have 19 address from 0x00 -> 0x12
                                            // -> require 2 ms ~ 1/100k * 19*8 bit / 8 * (8+1) = 1.71ms
                                            // double require time:) this time suitable for case send max data

// 16 registers address from 00H-> 0FH
#define I2C_DS3231_REG_SEC_ADDR         (0U)//0x00
#define I2C_DS3231_REG_MIN_ADDR         (1U)
#define I2C_DS3231_REG_HOUR_ADDR        (2U)
#define I2C_DS3231_REG_DAY_ADDR         (3U)
#define I2C_DS3231_REG_DATE_ADDR        (4U)
#define I2C_DS3231_REG_MONTH_ADDR       (5U)
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

// 16 registers represent from 00H -> 0FH
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

// Century limit
#define I2C_DS3231_LOWEST_OFFSET_YEAR  (1900U)
#define I2C_DS3231_HIGHEST_OFFSET_YEAR (2000U)

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

// --------------------Init & de-init----------------------

/**
 * @brief initial a register table truct to save and send data
 * 
 * @note input a NULL pointer type ds3231_reg_t*, then funtion will allocate by calloc
 * 
 * @param ds3231_reg_table* pointer to struct ds3231 register table
 * 
 * @retval 0 if ok
 * @retval !0 if can't allocate
 */
uint8_t i2c_ds3231_init_reg_table(ds3231_reg_t* ds3231_reg_table);

/**
 * @brief De-init register table
 * 
 * @param ds3231_reg_table* pointer to struct ds3231 register table
 */
void i2c_ds3231_clear_reg_table();

// --------------------Read time functions----------------------

/**
 * @brief Read data from ds3231
 * @note Using OR operator to select expected registers
 * @note Example (I2C_DS3231_REG_SEC | I2C_DS3231_REG_MIN) to get second and minute
 * @param request Each bit of type uint16_t corresponds to a flag that selects type of the 16 registers above
 */
uint8_t i2c_ds3231_read_full_reg(uint16_t request, ds3231_reg_t* ds3231_reg_table);


/**
 * @brief Translate 1 data time register (ignore control, control/status register) to 1 decimal
 * @note Example I2C_DS3231_REG_SEC to get second and minute
 * @param request Each bit of type uint16_t corresponds to a flag that convert 1 type of time
 * @retval uint8_t is value after convert to decimal (highest value is year, from 1900 - 2099)
 */
uint16_t i2c_ds3231_translate_reg_to_time(uint16_t type);

// --------------------Write time functions----------------------

/**
 * @brief Write data to ds3231
 */
void i2c_ds3231_set_time_to_reg_table();


/**
 * @brief Write data to ds3231
 * 
 * @note Using OR operator to select expected registers to write
 * @note Example (I2C_DS3231_REG_SEC | I2C_DS3231_REG_MIN) to write second and minute
 * 
 * @param request Each bit of type uint16_t corresponds to a flag that selects type of the 16 registers above
 */
uint8_t i2c_ds3231_write_full_reg();

#endif