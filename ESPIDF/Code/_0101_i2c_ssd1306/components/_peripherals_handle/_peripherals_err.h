#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_
/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include <stdio.h>

typedef enum
{
    PERIPH_OK = 0 ,
    I2C_MASTER_ALLOC_FAILED,
    I2C_MASTER_SETUP_FAILED,
    I2C_MASTER_INSTALL_FAILED,
    I2C_MASTER_UNINST_FAILED,
    I2C_MASTER_SEND_COMMAND_FAILED,
    I2C_MASTER_GET_DATA_FAILED

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