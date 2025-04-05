// ESP32 contains two MCPWM peripheral uints: MCPWM0 and MCPWM
// each uint have 3 A/B pair channel work together control by 3 timers and 3 PWM operators
// this library just focus on 1 unit

// esp32 support API functions: Configure outputs, Operate outputs, Adjust motor driven, Sync timers work together,
// Capture signals and Fault Handler

// This libraries just intent to: Config, Operate and adjust
// Now only support control 1 H bridge:
//  + brushed DC motor 1 phase using 2 pwm: direction and speed
//  + brushed DC motor 1 phase using 1 pwm (speed) and 2 gpio (direction) 


#ifndef _GPIO_SETUP_HANDLE_H_
#define _GPIO_SETUP_HANDLE_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************/
//c
#include <stdlib.h> // dynamic alloction
#include <stdio.h>  // print serial
#include <stdint.h> // type int

//esp32
#include "mcpwm.h"
#include "esp_err.h"        // const char *esp_err_to_name(esp_err_t code);

//user
#include "_peripherals_err.h"


/**
 * **********************************************************
 * APIs
 * **********************************************************/

// Configure
//  mcpwm_unit_t
//  mcpwm_gpio_init()
//  mcpwm_io_signals_t
//  mcpwm_set_pin()
//  mcpwm_pin_config_t
// mcpwm_timer_t
//mcpwm_config_t 
//  mcpwm_group_set_resolution()
// mcpwm_timer_set_resolution()
// mcpwm_init() 

// operate
//  mcpwm_set_signal_high() mcpwm_set_signal_low() then call  mcpwm_set_duty_type() to resume with previously set duty cycle.
//  mcpwm_start() mcpwm_stop().
//  mcpwm_set_duty()  mcpwm_set_duty_in_us()
//  mcpwm_get_duty()
// mcpwm_set_duty_type()
// mcpwm_generator_t
// mcpwm_init()
// mcpwm_duty_type_t.

// adjust
//  mcpwm_set_frequency()
// mcpwm_get_frequency()
//  mcpwm_deadtime_type_t
// mcpwm_deadtime_enable()
// mcpwm_deadtime_disable().


#endif