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
#include "gpio_setup_handle.h"


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// gpio_reset_pin
_peripherals_err_t gpio_setup_reset_pin_to_origin(gpio_num_t GPIO_pin_num)
{
    gpio_reset_pin(GPIO_pin_num); // always return ok

    return PERIPH_OK;
}

// gpio_set_pull_mode
_peripherals_err_t gpio_setup_io_direction(gpio_num_t GPIO_pin_num, gpio_mode_t GPIO_direct)
{
    esp_err_t ret = gpio_set_direction(GPIO_pin_num, GPIO_direct);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_SET_DIRECT_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_SET_DIRECT_FAILED;
    }

    return PERIPH_OK;
}

// gpio_set_direction
_peripherals_err_t gpio_setup_pull_res(gpio_num_t gpio_num, gpio_pull_mode_t pull)
{
    esp_err_t ret = gpio_set_pull_mode(gpio_num, pull);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_SET_PULL_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_SET_PULL_FAILED;
    }

    return PERIPH_OK;   
}

// gpio_set_level
_peripherals_err_t gpio_setup_output_logic_level(gpio_num_t gpio_num, uint32_t level)
{
    esp_err_t ret = gpio_set_level(gpio_num, level);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_SET_LEVEL_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_SET_LEVEL_FAILED;
    }

    return PERIPH_OK;   
}

// gpio_get_level
_peripherals_err_t gpio_setup_get_logic_level(gpio_num_t gpio_num, uint8_t* logic_level)
{
    *logic_level = gpio_get_level(gpio_num);

    return PERIPH_OK;  
}



// =================================== Only for all GPIO               ===============================
// =================================== Install & uninstall isr service ===============================
// =================================== For all GPIO (only call 1 time) ===============================
// gpio_install_isr_service
_peripherals_err_t gpio_setup_install_isr_service_for_gpio_system()
{
    esp_err_t ret = gpio_install_isr_service(ESP_INTR_FLAG_IRAM); // the interrupt service will create in IRAM (instruction ram)
    // for fastest execute  

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_ISR_SERVICE_INSTALL_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_ISR_SERVICE_INSTALL_FAILED;
    }

    return PERIPH_OK;
}


// gpio_uninstall_isr_service
_peripherals_err_t gpio_setup_un_install_isr_service_for_gpio_system()
{
    gpio_uninstall_isr_service(); // return void

    return PERIPH_OK;
}



// =================================== add isr handler for one GPIO    ===============================
// =================================== =============================== ===============================
// gpio_isr_handler_add
_peripherals_err_t gpio_setup_add_handler_for_pin(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)
{
    // isr_handler: function pointer handler
    // typedef void (*gpio_isr_t)(void *); <= funtion pointer, `args` is pointer to value will input function pointed by gpio_isr_t
    esp_err_t ret = gpio_isr_handler_add(gpio_num, isr_handler, args);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_ADD_HANDLER_TO_ISR_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_ADD_HANDLER_TO_ISR_FAILED;
    }

    return PERIPH_OK;
}

// gpio_isr_handler_remove
_peripherals_err_t gpio_setup_remove_handler_for_pin(gpio_num_t gpio_num)
{
    esp_err_t ret = gpio_isr_handler_remove(gpio_num);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_REMOVE_HANDLER_FROM_ISR_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_REMOVE_HANDLER_FROM_ISR_FAILED;
    }

    return PERIPH_OK;
}

// gpio_set_intr_type
_peripherals_err_t gpio_setup_isr_active_type_for_pin(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    esp_err_t ret = gpio_set_intr_type(gpio_num, intr_type);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_SET_INTR_TYPE_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_SET_INTR_TYPE_FAILED;
    }

    return PERIPH_OK;
}

// gpio_intr_enable();
_peripherals_err_t gpio_setup_enable_interrupt_for_pin(gpio_num_t gpio_num)
{
    esp_err_t ret = gpio_intr_enable(gpio_num);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_ENABLE_INTR_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_ENABLE_INTR_FAILED;
    }

    return PERIPH_OK;

}

// gpio_intr_disable
_peripherals_err_t gpio_setup_disable_interrupt_for_pin(gpio_num_t gpio_num)
{
    esp_err_t ret = gpio_intr_disable(gpio_num);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_DISABLE_INTR_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_DISABLE_INTR_FAILED;
    }

    return PERIPH_OK;
}

// =================================== Dis/en wake up                  ===============================
// =================================== =============================== ===============================
// gpio_wakeup_enable
_peripherals_err_t gpio_setup_bind_wakeup_for_pin(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    esp_err_t ret = gpio_wakeup_enable(gpio_num, intr_type);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_ENABLE_WAKEUP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_ENABLE_WAKEUP_FAILED;
    }

    return PERIPH_OK;
}

// gpio_wakeup_disable
_peripherals_err_t gpio_setup_un_bind_wakeup_for_pin(gpio_num_t gpio_num)
{
    esp_err_t ret = gpio_wakeup_disable(gpio_num);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(GPIO_SETUP_DISABLE_WAKEUP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return GPIO_SETUP_DISABLE_WAKEUP_FAILED;
    }

    return PERIPH_OK;
}

