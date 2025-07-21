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
#include "mcpwm_setup_handle.h"


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */
// mcpwm_timer_t
//mcpwm_config_t 
//  mcpwm_group_set_resolution()
// mcpwm_timer_set_resolution()
_peripherals_err_t mcpwm_setup_set_timer_frequency_resolution(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num,
    uint32_t frequency, float cmpr_a_duty_percent, float cmpr_b_duty_percent,
    unsigned long resolution)
{
    mcpwm_config_t timer_pwm_config =
    {
        .frequency = frequency,
        .cmpr_a = cmpr_a_duty_percent,
        .cmpr_b = cmpr_b_duty_percent,
        .duty_mode = MCPWM_SETUP_DUTY_SET_MODE,
        .counter_mode = MCPWM_SETUP_TIMER_COUNTER_MODE
    };

    esp_err_t ret =  mcpwm_timer_set_resolution(mcpwm_num, timer_num, resolution);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MCPWM_SETUP_INIT_RESOLUTION_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MCPWM_SETUP_INIT_RESOLUTION_FAILED;
    }
    
    ret =  mcpwm_init(mcpwm_num, timer_num, &timer_pwm_config);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MCPWM_SETUP_INIT_FREQ_DUTY_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MCPWM_SETUP_INIT_FREQ_DUTY_FAILED;
    }

    return PERIPH_OK;
}

// Configure
//  mcpwm_gpio_init()
//  mcpwm_io_signals_t
//  mcpwm_set_pin()
//  mcpwm_pin_config_t
_peripherals_err_t mcpwm_setup_init_GPIO_funtion(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, int gpio_num)
{
    esp_err_t ret =  mcpwm_gpio_init(mcpwm_num, io_signal, gpio_num);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MCPWM_SETUP_INIT_GPIO_FUNCTION_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MCPWM_SETUP_INIT_GPIO_FUNCTION_FAILED;
    }

    return PERIPH_OK;
}


// operate

//  mcpwm_start() mcpwm_stop().
_peripherals_err_t mcpwm_setup_start_output_signal(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    esp_err_t ret =  mcpwm_start(mcpwm_num, timer_num);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MCPWM_SETUP_INIT_START_OUTPUT_SIGNAL_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MCPWM_SETUP_INIT_START_OUTPUT_SIGNAL_FAILED;
    }

    return PERIPH_OK;
}

_peripherals_err_t mcpwm_setup_stop_output_signal(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    esp_err_t ret =  mcpwm_stop(mcpwm_num, timer_num);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MCPWM_SETUP_INIT_STOP_OUTPUT_SIGNAL_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MCPWM_SETUP_INIT_STOP_OUTPUT_SIGNAL_FAILED;
    }

    return PERIPH_OK;
}

//  mcpwm_set_duty()
// mcpwm_set_duty_in_us()
//  mcpwm_get_duty()

_peripherals_err_t mcpwm_setup_set_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, float duty)
{
    esp_err_t ret =  mcpwm_set_duty(mcpwm_num, timer_num, gen, duty);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MCPWM_SETUP_INIT_SET_DUTY_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MCPWM_SETUP_INIT_SET_DUTY_FAILED;
    }

    return PERIPH_OK;
}


// adjust
//  mcpwm_deadtime_type_t
// mcpwm_deadtime_enable()
// mcpwm_deadtime_disable().

_peripherals_err_t mcpwm_setup_deadtime_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    esp_err_t ret =  mcpwm_deadtime_enable(mcpwm_num, timer_num, MCPWM_SETUP_DEADTIME_MODE_TYPE,
        MCPWM_SETUP_DEADTIME_RISING_DELAY, MCPWM_SETUP_DEADTIME_FALLING_DELAY);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MCPWM_SETUP_INIT_SET_DEADTIME_EN_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MCPWM_SETUP_INIT_SET_DEADTIME_EN_FAILED;
    }

    return PERIPH_OK;
}

_peripherals_err_t mcpwm_setup_deadtime_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    esp_err_t ret =  mcpwm_deadtime_disable(mcpwm_num, timer_num);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MCPWM_SETUP_INIT_SET_DEADTIME_DIS_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MCPWM_SETUP_INIT_SET_DEADTIME_DIS_FAILED;
    }

    return PERIPH_OK;
}
