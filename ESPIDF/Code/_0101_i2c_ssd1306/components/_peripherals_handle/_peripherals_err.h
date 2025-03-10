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
    OK = 0 ,
    ALLOC_FAILED,
    SETUP_FAILED

} _peripherals_err_t;

/**
 * **********************************************************
 * API
 * **********************************************************
 */
/**
 * @brief Print line, file, error code
 */
void send_peripheral_err_location(_peripherals_err_t code, char* file, int line);

#endif