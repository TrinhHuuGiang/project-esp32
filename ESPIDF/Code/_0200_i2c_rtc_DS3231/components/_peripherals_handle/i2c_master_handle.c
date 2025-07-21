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


/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "i2c_master_handle.h"


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
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2C_MASTER_SETUP_FAILED, __FILE__, __LINE__, "Alloc failed");
        #endif
        return I2C_MASTER_ALLOC_FAILED;
    }

    // config value
    (*master_conf_addr)->mode = I2C_MODE_MASTER;

    (*master_conf_addr)->sda_io_num = I2C_MASTER_SDA_PIN;
    (*master_conf_addr)->sda_pullup_en = I2C_MASTER_SDA_PULLUP_EN;

    (*master_conf_addr)->scl_io_num = I2C_MASTER_SCL_PIN;
    (*master_conf_addr)->scl_pullup_en = I2C_MASTER_SCL_PULLUP_EN;

    ((*master_conf_addr)->master).clk_speed = I2C_MASTER_FREQ;

    (*master_conf_addr)->clk_flags = I2C_MASTER_CLOCK_FLAG;

    return PERIPH_OK;
}

// setup configuration
_peripherals_err_t i2c_master_setup_hardware(const i2c_config_t* master_conf)
{
    esp_err_t ret = i2c_param_config(I2C_MASTER_PORT, master_conf);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2C_MASTER_SETUP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return I2C_MASTER_SETUP_FAILED;
    }

    return PERIPH_OK;
}

// install driver
_peripherals_err_t i2c_master_install_driver(const i2c_config_t* master_conf)
{
    esp_err_t ret = i2c_driver_install(I2C_MASTER_PORT, master_conf->mode, 0, 0, 0);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2C_MASTER_INSTALL_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return I2C_MASTER_INSTALL_FAILED;
    }

    return PERIPH_OK;
}

// free configuration
_peripherals_err_t i2c_master_free_config(i2c_config_t* master_conf)
{
    i2c_config_t** master_conf_addr = (i2c_config_t**)master_conf;

    free(*master_conf_addr);

    *master_conf_addr = NULL; // point to NULL

    return PERIPH_OK;
}

// master send command
_peripherals_err_t i2c_master_send_command_to_7bit_addr(uint8_t adr_7bit, i2c_master_list_t list_command, TickType_t time_out)
{
    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ret = i2c_master_start(cmd);
    if(ret == ESP_OK) ret = i2c_master_write_byte(cmd, (adr_7bit << 1) | I2C_MASTER_WRITE, 1);
    if(ret == ESP_OK) ret = i2c_master_write(cmd, list_command.list_ptr, list_command.list_size, 1);
    if(ret == ESP_OK) ret = i2c_master_stop(cmd);
    if(ret == ESP_OK) ret = i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(time_out));

    i2c_cmd_link_delete(cmd); // Free cmd link

    // check
    if(ret != ESP_OK) {
        #if CONFIG_DEBUG_ENABLE != 0
        send_peripheral_err_location(I2C_MASTER_SEND_COMMAND_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return I2C_MASTER_SEND_COMMAND_FAILED;
    }

    return PERIPH_OK;
}


// master get data
_peripherals_err_t i2c_master_get_data_from_7bit_addr(uint8_t adr_7bit, i2c_master_list_t* list_command, TickType_t time_out)
{
    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ret = i2c_master_start(cmd);
    if(ret == ESP_OK) ret = i2c_master_write_byte(cmd, (adr_7bit << 1) | I2C_MASTER_READ, 1);

    // read (list_size - 1) byte + feedback ACK
    // ignore if send 1 byte
    if(ret == ESP_OK && list_command->list_size > 1) ret = i2c_master_read(cmd, list_command->list_ptr, list_command->list_size - 1, I2C_MASTER_ACK);
    
    // read last byte, send NACK
    if(ret == ESP_OK) ret = i2c_master_read_byte(cmd, &list_command->list_ptr[list_command->list_size - 1], I2C_MASTER_NACK);

    if(ret == ESP_OK) ret = i2c_master_stop(cmd);
    if(ret == ESP_OK) ret = i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(time_out));

    i2c_cmd_link_delete(cmd);

    // check
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE != 0
        send_peripheral_err_location(I2C_MASTER_GET_DATA_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return I2C_MASTER_GET_DATA_FAILED;
    }
    return PERIPH_OK;
}


// master uninstall driver
_peripherals_err_t i2c_master_un_install_driver()
{
    esp_err_t ret = i2c_driver_delete(I2C_MASTER_PORT);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(I2C_MASTER_UNINST_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return I2C_MASTER_UNINST_FAILED;
    }

    return PERIPH_OK;
}