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


// ESP32 contains two MCPWM peripheral uints: MCPWM0 and MCPWM
// each uint have 3 A/B pair channel work together control by 3 timers and 3 PWM operators
// this library just focus on 1 unit

// esp32 support API functions: Configure outputs, Operate outputs, Adjust motor driven, Sync timers work together,
// Capture signals and Fault Handler

// This libraries just intent to: Config, Operate and adjust
// Now only support control 1 H bridge:
//  + brushed DC motor 1 phase using 2 pwm: direction and speed
//  + brushed DC motor 1 phase using 1 pwm (speed) and 2 gpio (direction) 


#ifndef _MCPWM_SETUP_HANDLE_H_
#define _MCPWM_SETUP_HANDLE_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************/
//c
#include <stdlib.h> // dynamic alloction
#include <stdio.h>  // print serial
#include <stdint.h> // type int

//esp32
#include "driver/mcpwm.h"
#include "esp_err.h"        // const char *esp_err_to_name(esp_err_t code);

//user
#include "_peripherals_err.h"

#define MCPWM_SETUP_COMMON_RESOLUTION       (8)  // 2^8 = 256
                                                 // clock source max 160Mhz
                                                 // prescaler max 8bit -> max div 256, min div 1
                                                 // clock div : 625KHz -> 160MHz
                                                 // with resolution 8 , timer supply clock: 2441Hz -> 625KHz
                                                 // Option frequency below

#define MCPWM_SETUP_COMMON_FREQ_5K          (5000)
#define MCPWM_SETUP_COMMON_FREQ_10K         (10000)
#define MCPWM_SETUP_COMMON_FREQ_15K         (15000)
#define MCPWM_SETUP_COMMON_FREQ_20K         (20000)

// set timer mode for motor
#define MCPWM_SETUP_TIMER_COUNTER_MODE     MCPWM_UP_COUNTER    // suitable for asymetric  
#define MCPWM_SETUP_DUTY_SET_MODE          MCPWM_DUTY_MODE_0   // active high duty


// set deadtime (using for control with 2 pwm A/B)
#define MCPWM_SETUP_DEADTIME_MODE_TYPE     MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE  /*!<MCPWMXA Out = MCPWMXA In with rising edge delay,  MCPWMXB = MCPWMXA In with compliment of falling edge delay*/
                                           // see more in refernce manual
                                           // this mode make channel A and B out signal depend on channel A in signal
                                           // this mode sure when duty A high, duty B low and similar to A low B high
                                           // Apply this with death time we can safe H brigde short curcuit
                                           // when duty ~ 50 (A high 50% first, B high 50% after that) motor like is stopping
                                           // when duty > 50 motor rotate 1 direct
                                           // when duty < 50 motor rotate reverse
#define MCPWM_SETUP_DEADTIME_RISING_DELAY  (5UL)  //     1 == 100ns
#define MCPWM_SETUP_DEADTIME_FALLING_DELAY (5UL)  // ->  5 == 500ns is safe  . 200ns is common . higher maybe no stable
                                                      // 1/500ns = 2MHz -> transistor must switch faster 2MHz and fine with low duty frequency
                                                      // example with 20khz, 500ns ~ 1% duty, 5khz ~ 0.25%

/**
 * **********************************************************
 * APIs
 * **********************************************************/


// mcpwm_timer_t
//mcpwm_config_t 
//  mcpwm_group_set_resolution()
// mcpwm_timer_set_resolution()
_peripherals_err_t mcpwm_setup_set_timer_frequency_resolution(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num,
    uint32_t frequency, float cmpr_a_duty_percent, float cmpr_b_duty_percent,
    unsigned long resolution);


// Configure
//  mcpwm_gpio_init()
//  mcpwm_io_signals_t
//  mcpwm_set_pin()
//  mcpwm_pin_config_t
_peripherals_err_t mcpwm_setup_init_GPIO_funtion(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, int gpio_num);



// operate (connect timer x to operator A or B of it and send signal)

//  mcpwm_start() mcpwm_stop().
_peripherals_err_t mcpwm_setup_start_output_signal(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

_peripherals_err_t mcpwm_setup_stop_output_signal(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

//  mcpwm_set_duty()
// mcpwm_set_duty_in_us()
//  mcpwm_get_duty()

_peripherals_err_t mcpwm_setup_set_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, float duty);

// adjust
//  mcpwm_deadtime_type_t
// mcpwm_deadtime_enable()
// mcpwm_deadtime_disable().

_peripherals_err_t mcpwm_setup_deadtime_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

_peripherals_err_t mcpwm_setup_deadtime_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

#endif