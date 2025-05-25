/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "gpio_74hc595.h"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
static gpio_74hc595_data_t* s_data_struct = NULL;

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */


/**
 * Init output gate
 * - Init and regist spi device, latch + transaction
 * - Clean dummy bit after chip reset
 */
uint8_t gpio_74HC595_init_output_gate()
{
    // create data struct
    if(s_data_struct) // !=NULL
    {
        return 1;
    }

    s_data_struct = calloc(1, sizeof(gpio_74hc595_data_t));

    if(!s_data_struct) // ==NULL
    {
        return 2;
    }

    // SPI
    spi_device_interface_config_t* device_config = NULL; // init / deinit device config, 
                                                // regist/unregist device config


    if(spi_master_init_device_config(&(device_config), GPIO_74HC595_SPI_COMMAND_LEN,
        GPIO_74HC595_SPI_ADDRESS_LEN, GPIO_74HC595_SPI_DUMMY_LEN, GPIO_74HC595_SPI_CLOCK_MODE,
        GPIO_74HC595_SPI_SCK_FREQ, GPIO_74HC595_LATCH_PIN, 0,0,0,0,0//<< flag 0 -> MSB first
    )) 
    {
        spi_master_free_device_config( device_config);
        return 3;
    }

    if(spi_master_register_device_config(device_config, &(s_data_struct->device_handle)))
    {
        spi_master_free_device_config( device_config);
        return 4;
    }

    // free config after use
    spi_master_free_device_config( device_config);

    // create spi transaction
    if(spi_master_prepare_transaction(&(s_data_struct->transaction_config),
        GPIO_74HC595_SPI_COMMAND_LEN , GPIO_74HC595_SPI_ADDRESS_LEN,
        GPIO_74HC595_SPI_DUMMY_LEN, 0, 0,
        GPIO_74HC595_SPI_TX_BIT_DATA_LEN, 0
    ))
    {
        return 5;
    }

    // try call 'gpio_74HC595_set_output_bit', update 74hc595 to clean dummy value
    // if failed -> de init :) and return error
    // if de init error ??? return error => serious error
    if(gpio_74HC595_set_output_bit(GPIO_74HC595_INIT_PINS_MOD, 
        GPIO_74HC595_INIT_PINS_VALUE))
    {
        if(gpio_74HC595_de_init_output_gate())
        {
            return 7; // can't deinit
        }
        return 6; // de init success
    }

    // ok
    return 0;
}

/**
 * De init output gate
 * - De init transaction, unregist then deinit spi device
 */
uint8_t gpio_74HC595_de_init_output_gate()
{
    if(s_data_struct->device_handle == NULL) return 0; // no longer available

    // delete transaction
    if(s_data_struct->transaction_config)
    {
        if(spi_master_delete_transaction(&(s_data_struct->transaction_config))) return 1;
        s_data_struct->transaction_config = NULL;
    }

    // delete and auto set handle = NULL
    if(s_data_struct->device_handle)
    {
        if(spi_master_un_register_device_config(s_data_struct->device_handle)) return 2;
        s_data_struct->device_handle = NULL;
    }
    
    // free data struct
    free(s_data_struct);
    s_data_struct = NULL;

    // ok
    return 0;
}

/**
 * Return current 32bit output
 * Parse by AND with 'gpio_74hc595_gate_num_t'
 * 
 * Input a uint32_t and get a copy of current reg_state
 */
uint8_t gpio_74HC595_get_current_output_bit(uint32_t* reg_state)
{
    if(!s_data_struct) // not initialized
    {
        return 1;
    }

    *reg_state = s_data_struct->reg_state;
    return 0;
}


/**
 * Pins order: Input OR (1)   represent for where number pin need set logic
 * Pins value: Input OR (1/0) represent for what type of logic set for these pin
 * 
 * Execute:
 * + add value to transaction -> shift out
 * + latch value
 * + update reg_state
 * 
 * Note logic:
 * + pins_value &= pins_order;      // :) if user input excess value of order pins, clean it
 * + temp_reg_state = reg_state;
 * + temp_reg_state &=~ pins_order; // clear old bit value
 * + temp_reg_state |= pins_value;
 * + If set success => do update reg_state = temp_reg_state
 * + Or return err
 */
uint8_t gpio_74HC595_set_output_bit(uint32_t pins_order, uint32_t pins_value)
{
    if(!s_data_struct)// not initialized
    {
        return 1;
    }

    // logic
    pins_value &= pins_order;
    uint32_t temp_reg_state = s_data_struct->reg_state;
    
    temp_reg_state &=~pins_order;

    temp_reg_state |= pins_value;

    // set by spi
    uint8_t* tx_buffer_mod_ptr = (uint8_t*)(s_data_struct->transaction_config->base.tx_buffer);
    s_data_struct->transaction_config->base.tx_buffer = NULL;// just direct tx_buffer_pointer to NULL before modify tx buffer


    // *tx_buffer_mod_ptr = temp_reg_state; <-- wrong because uint8_t will cast temp_reg_state
    memcpy(tx_buffer_mod_ptr, &temp_reg_state, GPIO_74HC595_SPI_TX_BIT_DATA_LEN/8);

    s_data_struct->transaction_config->base.tx_buffer = tx_buffer_mod_ptr; // re-block tx buffer

    // transaction
    if(spi_master_polling_transaction(s_data_struct->device_handle, s_data_struct->transaction_config)!= PERIPH_OK) return 2;

    // ok
    s_data_struct->reg_state = temp_reg_state;
    return 0;
}