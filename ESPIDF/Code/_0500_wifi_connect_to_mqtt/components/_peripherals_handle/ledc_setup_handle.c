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
#include "ledc_setup_handle.h"


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

 
// timer config
_peripherals_err_t ledc_setup_timer_config(ledc_timer_t timer_order_num, uint32_t ledc_freq_hz, ledc_timer_bit_t ledc_resolution)
{
    // config 1 of 4 high speed timer
    ledc_timer_config_t timer_conf = 
    {
        .speed_mode = LEDC_SETUP_LEDC_SPEED_MODE,
        .duty_resolution = ledc_resolution,
        .timer_num = timer_order_num,
        .freq_hz = ledc_freq_hz, //Hz
        .clk_cfg = LEDC_AUTO_CLK
    };

    esp_err_t ret = ledc_timer_config(&timer_conf);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_TIMER_CONFIG_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return LEDC_SETUP_TIMER_CONFIG_FAILED;
    }

    return PERIPH_OK;
}

// channel config and start
_peripherals_err_t ledc_setup_channel_config_and_start(int gpio_num, ledc_channel_t channel_order_num, 
    ledc_timer_t timer_order_num, ledc_timer_bit_t ledc_resolution ,uint32_t init_duty)
{
    if(init_duty >= (1<<ledc_resolution))
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_CHANNEL_DUTY_OVERFLOW, __FILE__, __LINE__, "Duty over flow");
        #endif
        return LEDC_SETUP_CHANNEL_DUTY_OVERFLOW; 
    }

    ledc_channel_config_t ledc_conf = 
    {
        .gpio_num = gpio_num,
        .speed_mode = LEDC_SETUP_LEDC_SPEED_MODE,
        .channel = channel_order_num,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer_order_num,
        .duty = init_duty, // from 0 - 2^duty_resolution-1
        .hpoint = 0,       // start point start duty (set phase) 0 -> 2^duty_resolution - 1
        .flags.output_invert = 0 // disable invert
    };

    // config 1 of 4 high speed ledc (only using 4 led highspeed of 8 ledc highspeed)
    esp_err_t ret = ledc_channel_config(&ledc_conf);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_CHANNEL_CONFIG_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return LEDC_SETUP_CHANNEL_CONFIG_FAILED;
    }

    return PERIPH_OK;

}

// stop channel ledc
_peripherals_err_t ledc_setup_stop_channel(ledc_channel_t channel, uint32_t idle_level)
{
    esp_err_t ret = ledc_stop(LEDC_SETUP_LEDC_SPEED_MODE ,channel, idle_level);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_CHANNEL_STOP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return LEDC_SETUP_CHANNEL_STOP_FAILED;
    }

    return PERIPH_OK;
}

// change PWM Duty cycle software
_peripherals_err_t ledc_setup_change_pwm_duty(ledc_channel_t channel, uint32_t duty, ledc_timer_bit_t ledc_resolution)
{
    if(duty >= (1<<ledc_resolution))
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_CHANNEL_DUTY_OVERFLOW, __FILE__, __LINE__, "Duty over flow");
        #endif
        return LEDC_SETUP_CHANNEL_DUTY_OVERFLOW; 
    }

    esp_err_t ret = ledc_set_duty(LEDC_SETUP_LEDC_SPEED_MODE, channel, duty);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_CHANNEL_SET_DUTY_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return LEDC_SETUP_CHANNEL_SET_DUTY_FAILED;
    }

    ret = ledc_update_duty(LEDC_SETUP_LEDC_SPEED_MODE,channel);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_CHANNEL_CHANGE_DUTY_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return LEDC_SETUP_CHANNEL_CHANGE_DUTY_FAILED;
    }

    return PERIPH_OK;
}


// install driver before using fade function (only call 1 time esle uinstall before recall)
_peripherals_err_t ledc_setup_install_hardware_auto_pwm_duty_driver()
{
    esp_err_t ret = ledc_fade_func_install(ESP_INTR_FLAG_IRAM);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_INSTALL_FADE_DRIVER_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return LEDC_SETUP_INSTALL_FADE_DRIVER_FAILED;
    }

    return PERIPH_OK;
}

// uninstall fade driver
_peripherals_err_t ledc_setup_un_install_hardware_auto_pwm_duty_driver()
{
    ledc_fade_func_uninstall();

    return PERIPH_OK;

}

// change PWM auto by hardware
_peripherals_err_t ledc_setup_auto_fade_pwm_duty_by_hardware(ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
{   

    esp_err_t ret = ledc_set_fade_with_time(LEDC_SETUP_LEDC_SPEED_MODE, channel, target_duty, max_fade_time_ms);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_CHANNEL_CHANGE_DUTY_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return LEDC_SETUP_CHANNEL_CHANGE_DUTY_FAILED;
    }
    
    ret = ledc_fade_start(LEDC_SETUP_LEDC_SPEED_MODE, channel, LEDC_FADE_WAIT_DONE); 
    // done fade, do not using flag LEDC_FADE_NO_WAIT (ledc_fade_start can't recall on 1 channel at a time)

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(LEDC_SETUP_START_FADE_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return LEDC_SETUP_START_FADE_FAILED;
    }

    ESP_LOGW("ledc_fade", "->3");


    return PERIPH_OK;
}
