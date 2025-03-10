/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "i2c_handle.h"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */


 
/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */
// initial configuration
_peripherals_err_t i2c_master_init_config(i2c_config_t* master_conf)
{
    i2c_config_t** master_conf_addr = (i2c_config_t**)master_conf;

    *master_conf_addr = (i2c_config_t*)calloc(1, sizeof(i2c_config_t));

    if(*master_conf_addr == NULL)
    {
        // user check NULL -> failed
        return ALLOC_FAILED;
    }

    // config value
    (*master_conf_addr)->mode = I2C_MODE_MASTER;

    (*master_conf_addr)->sda_io_num = I2C_MASTER_SDA_PIN;
    (*master_conf_addr)->sda_pullup_en = I2C_MASTER_SDA_PULLUP_EN;

    (*master_conf_addr)->scl_io_num = I2C_MASTER_SCL_PIN;
    (*master_conf_addr)->scl_pullup_en = I2C_MASTER_SCL_PULLUP_EN;

    ((*master_conf_addr)->master).clk_speed = I2C_MASTER_FREQ;

    (*master_conf_addr)->clk_flags = I2C_MASTER_CLOCK_FLAG;

    return OK;
}

// setup configuration
_peripherals_err_t i2c_master_setup_hardware(i2c_config_t* master_conf)
{
    



    return OK;
}