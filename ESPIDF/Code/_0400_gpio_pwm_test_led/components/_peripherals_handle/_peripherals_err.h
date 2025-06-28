/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


/*
    esp_err_t ret = functions();

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(ERR_CODE, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return ERR_CODE;
    }

    return PERIPH_OK;
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
    PERIPH_OK = 0 , // << always set ok 0, alot function check if(function()) will know return ok
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
    GPIO_SETUP_SET_DIRECT_FAILED,
    GPIO_SETUP_SET_PULL_FAILED,
    GPIO_SETUP_SET_LEVEL_FAILED,
    GPIO_SETUP_ISR_SERVICE_INSTALL_FAILED,
    GPIO_SETUP_ADD_HANDLER_TO_ISR_FAILED,
    GPIO_SETUP_REMOVE_HANDLER_FROM_ISR_FAILED,
    GPIO_SETUP_SET_INTR_TYPE_FAILED,
    GPIO_SETUP_ENABLE_INTR_FAILED,
    GPIO_SETUP_DISABLE_INTR_FAILED,
    GPIO_SETUP_ENABLE_WAKEUP_FAILED,
    GPIO_SETUP_DISABLE_WAKEUP_FAILED,

    // LEDC
    LEDC_SETUP_TIMER_CONFIG_FAILED,
    LEDC_SETUP_CHANNEL_DUTY_OVERFLOW,
    LEDC_SETUP_CHANNEL_CONFIG_FAILED,
    LEDC_SETUP_CHANNEL_STOP_FAILED,
    LEDC_SETUP_CHANNEL_SET_DUTY_FAILED,
    LEDC_SETUP_CHANNEL_CHANGE_DUTY_FAILED,
    LEDC_SETUP_INSTALL_FADE_DRIVER_FAILED,
    LEDC_SETUP_START_FADE_FAILED,
    
    // MCPWM
    MCPWM_SETUP_INIT_RESOLUTION_FAILED,
    MCPWM_SETUP_INIT_FREQ_DUTY_FAILED,
    MCPWM_SETUP_INIT_GPIO_FUNCTION_FAILED,
    MCPWM_SETUP_INIT_START_OUTPUT_SIGNAL_FAILED,
    MCPWM_SETUP_INIT_STOP_OUTPUT_SIGNAL_FAILED,
    MCPWM_SETUP_INIT_SET_DUTY_FAILED,
    MCPWM_SETUP_INIT_SET_DEADTIME_EN_FAILED,
    MCPWM_SETUP_INIT_SET_DEADTIME_DIS_FAILED,

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