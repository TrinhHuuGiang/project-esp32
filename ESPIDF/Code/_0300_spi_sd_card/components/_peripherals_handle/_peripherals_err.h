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
    SPI_MASTER_POLLING_TRANSACTION_FAILED

    // I2S


    // GPIO

    
    // ADC

    
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