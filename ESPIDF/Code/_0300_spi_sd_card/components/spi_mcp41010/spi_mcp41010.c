/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "spi_mcp41010.h"

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// register and get handle
uint8_t spi_mcp41010_register_and_get_handle(int CS_pin, spi_device_handle_t* device_handle)
{
    spi_device_interface_config_t* device_conf = NULL;

    if(spi_master_init_device_config(&device_conf, SPI_MCP41010_COMMAND_LEN,SPI_MCP41010_ADDRESS_LEN,SPI_MCP41010_DUMMY_LEN,
    SPI_MCP41010_MODE_0, SPI_MCP41010_CLK_SPEED, CS_pin, 0, 0, 0,
    0, 0) != PERIPH_OK) return 1;

    if(spi_master_register_device_config(device_conf, device_handle) != PERIPH_OK) return 2;

    if(spi_master_free_device_config(&device_conf) != PERIPH_OK) return 3;

    return 0;
}

// un-register
uint8_t spi_mcp41010_un_register_and_delete_handle(spi_device_handle_t device_handle)
{
    if(device_handle == NULL)
    {
        return 0;
    }

    // delete and auto set handle = NULL
    if(spi_master_un_register_device_config(device_handle)!= PERIPH_OK) return 1;

    return 0;

}


// get transaction template
uint8_t spi_mcp41010_create_transaction_template(spi_transaction_ext_t* spi_expand_transaction)
{
    if(spi_master_prepare_transaction(spi_expand_transaction,
        SPI_MCP41010_COMMAND_LEN,SPI_MCP41010_ADDRESS_LEN,SPI_MCP41010_DUMMY_LEN,
        SPI_MCP41010_COMMAND_WRITE_DATA, 0, 
        SPI_MCP41010_TX_BIT_DATA_LEN, 0)!= PERIPH_OK) return 1;

    return 0;
}


// admin add data to transaction (no code :) add by hand)


// delete transaction template
uint8_t spi_mcp41010_delete_transaction_template(spi_transaction_ext_t *spi_expand_transaction)
{
    if(spi_master_delete_transaction(spi_expand_transaction)!=PERIPH_OK) return 1;

    return 0;
}


// set level
uint8_t spi_mcp41010_send_potentiometer_level(const spi_device_handle_t device_handle, spi_transaction_ext_t *spi_expand_transaction,
    uint8_t level)
{
    // write level to send

    uint8_t* tx_buffer_mod_ptr = (uint8_t*)(spi_expand_transaction->base.tx_buffer);

    spi_expand_transaction->base.tx_buffer = NULL; // just direct tx_buffer_pointer to NULL before modify tx buffer

    *tx_buffer_mod_ptr = level;

    spi_expand_transaction->base.tx_buffer = tx_buffer_mod_ptr; // re-block tx buffer

    // transaction
    if(spi_master_polling_transaction(device_handle, spi_expand_transaction)!= PERIPH_OK) return 1;

    // ok
    return 0;
}


