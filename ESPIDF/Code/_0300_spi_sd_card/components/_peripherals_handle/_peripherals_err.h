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


#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_
/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include <stdio.h>
#include "esp_log.h"

typedef enum
{
    PERIPH_OK = 0 ,
    // I2C
    I2C_MASTER_ALLOC_FAILED,
    I2C_MASTER_SETUP_FAILED,
    I2C_MASTER_INSTALL_FAILED,
    I2C_MASTER_UNINST_FAILED,
    I2C_MASTER_SEND_COMMAND_FAILED,
    I2C_MASTER_GET_DATA_FAILED,

    // SPI
    SPI_MASTER_ALLOC_FAILED,
    SPI_MASTER_DEVICE_CLK_OVER_FREQ,
    SPI_MASTER_INS_CONFIG_BUS_FAILED,
    SPI_MASTER_REG_DEVICE_CONFIG_FAILED,
    SPI_MASTER_UN_REG_DEVICE_CONFIG_FAILED,
    SPI_MASTER_UN_INS_CONFIG_BUS_FAILED,
    SPI_MASTER_POLLING_TRANSACTION_FAILED,

    // I2S


    // GPIO

    
    // ADC

    // Other device
    SPI_SD_CARD_REGIST_PATH_FAILED,
    SPI_SD_CARD_MOUNT_TO_CARD_FAILED,
    SPI_SD_CARD_UN_MOUNT_CARD_FAILED,
    SPI_SD_CARD_UN_REGIST_PATH_FAILED
} _peripherals_err_t;

/**
 * **********************************************************
 * API
 * **********************************************************
 */
/**
 * @brief Print error code, file, line, comment
 */
void send_peripheral_err_location(_peripherals_err_t code, char* file, int line,const char* comment);

#endif