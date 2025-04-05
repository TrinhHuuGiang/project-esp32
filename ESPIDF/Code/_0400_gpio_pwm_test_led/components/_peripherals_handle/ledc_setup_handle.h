// esp32 have 4 timer for 8 channel high speed
// and   have 4 timer for 8 channel slow speed
// here we only use 4 timer and 8 channel high speed
// for convenient, should set 1 - 1 (timer0 - channel0, .. , timer3 - channel3)


#ifndef _LEDC_SETUP_HANDLE_H_
#define _LEDC_SETUP_HANDLE_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************/
//c
#include <stdlib.h> // dynamic alloction
#include <stdio.h>  // print serial
#include <stdint.h> // type int

//esp32
#include "driver/ledc.h"
#include "hal/ledc_types.h"
#include "esp_err.h"        // const char *esp_err_to_name(esp_err_t code);

//user
#include "_peripherals_err.h"

#define LEDC_SETUP_COMMON_FREQUENCY_1KHZ   (1000)
#define LEDC_SETUP_COMMON_FREQUENCY_5KHZ   (5000)
#define LEDC_SETUP_COMMON_FREQUENCY_10KHZ  (10000)

#define LEDC_SETUP_FIX_COMMON_RESOLUTION  (7)           // 2^7 = 128 duty level

#define LEDC_SETUP_LEDC_SPEED_MODE  LEDC_HIGH_SPEED_MODE

/**
 * **********************************************************
 * APIs
 * **********************************************************/
// =================================== Timer config                ===================================
// =================================== =========================== ===================================

// timer config
_peripherals_err_t ledc_setup_timer_config(ledc_timer_t timer_order_num, uint32_t ledc_freq_hz);




// =================================== Start stop pwm              ===================================
// =================================== =========================== ===================================
// channel config and start
_peripherals_err_t ledc_setup_channel_config_and_start(int gpio_num, ledc_channel_t channel_order_num, 
    ledc_timer_t timer_order_num, uint32_t init_duty);

// stop channel ledc
_peripherals_err_t ledc_setup_stop_channel(ledc_channel_t channel, uint32_t idle_level);


// =================================== Change duty by soft ware    ===================================
// =================================== =========================== ===================================


// change PWM Duty cycle software
_peripherals_err_t ledc_setup_change_pwm_duty(ledc_channel_t channel, uint32_t duty);

// =================================== Change duty by hard ware    ===================================
// =================================== Fade effect                 ===================================

// install driver before using fade function (only call 1 time esle uinstall before recall)
_peripherals_err_t ledc_setup_install_hardware_auto_pwm_duty_driver();

// uninstall fade driver
_peripherals_err_t ledc_setup_un_install_hardware_auto_pwm_duty_driver();

// change PWM auto by hardware
_peripherals_err_t ledc_setup_auto_fade_pwm_duty_by_hardware(ledc_channel_t channel, 
    uint32_t target_duty, int max_fade_time_ms);

#endif