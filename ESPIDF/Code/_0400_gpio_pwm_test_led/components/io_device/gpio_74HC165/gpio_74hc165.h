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


// this library code for series 4 ic 74hc165 to expand input signal to 32 pin
// available for : push button, keypad, and another logic signals have short temporary status change

#ifndef _GPIO_74HC165_H_
#define _GPIO_74HC165_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>


// delay busy wait us
#include "esp_rom_sys.h"

// user define
#include "gpio_setup_handle.h"
#include "ledc_setup_handle.h"

#define GPIO_74HC165_LEDC_TIMER_CLOCK          LEDC_TIMER_0
#define GPIO_74HC165_LEDC_CHANNEL_CLOCK        LEDC_CHANNEL_0
#define GPIO_74HC165_LEDC_FREQ_CLOCK           (1000000)
#define GPIO_74HC165_LEDC_RESOLUTION_CLOCK     (1)
#define GPIO_74HC165_LEDC_DUTY_CLOCK           (1U) // 0 == 0%, 1 == 50%
#define GPIO_74HC165_CLOCK_PIN                 CONFIG_GPIO_74HC165_CLOCK_PIN

#define GPIO_74HC165_LEDC_TIMER_LATCH          LEDC_TIMER_1
#define GPIO_74HC165_LEDC_CHANNEL_LATCH        LEDC_CHANNEL_1 
#define GPIO_74HC165_LEDC_FREQ_LATCH           (100000)
#define GPIO_74HC165_LEDC_RESOLUTION_LATCH     (5)
#define GPIO_74HC165_LEDC_DUTY_LATCH           (29U) // see explain below
#define GPIO_74HC165_LATCH_PIN                 CONFIG_GPIO_74HC165_LATCH_PIN

#define GPIO_74HC165_DATA_SHIFT_IN_PIN             CONFIG_GPIO_74HC165_DATA_SHIFT_IN
#define GPIO_74HC165_DATA_SHIFT_IN_INTERRUPT_TYPE  GPIO_INTR_POSEDGE
    // when config hardware, to save energy, idle input value should be LOW, because when shift low,
    // the output 5V will devided voltage by 1k and 620 resistor to 3V for safe esp32 input pin
    // here we will set interrupt type when raising (LOW to HIGH) to capture any signal rising
    // after that using 'gpio_74HC165_check_flag_and_renew_register' below function to get real input by hand
    // interrupt by edge help interrupt

#define GPIO_74_HC165_DATA_SHIFT_DELAY_BY_HAND_US  (2) // micro second, with previous clock 1MHz
                                                        // when change shift by hand, delay should slower for capture realdata
                                                        // 2us is ok


#define GPIO_74HC165_SET_FLAG   (1U)
#define GPIO_74HC165_NOT_SET_FLAG   (0U)

typedef struct{
    uint32_t reg_state;
    volatile uint8_t capture_flags;
    // << note: when 'compiler' optimizing, value of variable maybe optimize.
    // example, when compiler optimize 1 function:
    //  int a = 1;
    //  while (a == 1)
    //  { loop }
    // we expect that 'a' will change by interrupt (another function) in future
    // optimizer don't know, it can change:
    //  while (1) and loop will infinite
} gpio_74hc165_data_t;

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */
//============================== Basic functions ============== ==============================
//============================== ============================== ==============================


// initial input gate 32 bit with series of 4 74HC165
// initial pwm signal for latch (PL):
//  - allow parallel input (active LOW)
//  - latch data (high)
//  - latch duty: = 90 / 10 (calculate 87.5%)
//  - latch freq: = clk duty / 10

// pwm signal for signal for generate clock input (when PL signal is High)
//  - clock Low to High -> shift data to Q7
//  - clk duty: 50/50
//  - clk freq: 

// example with 1 74HC165
// [model]:
// cycle : 1     2     3     4     5     6     7     8     9     10
// clk   : |--|__|--|__|--|__|--|__|--|__|--|__|--|__|--|__|--|__|--|__

// latch : |-----------------------------------------------|___________  << cover 7-9 rasing
// case1 : |---------------------------------------------|_____________  << cover 7-8 rasing
//         _|---------------------------------------------|____________  << cover 7 rasing
// case2 : |-------------------------------------------------|_________  << cover 8~9 rasing
//         __|-------------------------------------------------|_______  << cover 8 rasing

// [Evaluate]: 
// - case 1 and case 2 point that the duty may be async or faster, or slower clock shift signal
// - Suppose the difference between the time of latching the data and shifting the data is 1 clock pulse.
// At the times when 'shifting' coincides with 'latching', there is no guarantee that the signal will be 
// pushed out. Therefore, these sensitive times can be considered as having or not having signal shift.
// Case shift 7 signal still reach, so for sure always shift out all 8 signal of a 74HC165, latch
// duty should > 80 %

// -> Each 74HC165 can latch 8 bits of data. To avoid miss when taking 8 signal, 
// we will take an extra clock cycle (8 continuous signals and 1 repeating signal).
// some time 2 clock from 2 pwm may be asynchronous, latch can longer than 9 cycle
// best case is duty 90/10
// 90% time data will shift out and 10% of time will prepare to latch new data

// [Calculate]:
// Clock
// - To avoid affect of high frequency attenuation:
// the clock frequency will take 1MHz for convenice devide frequency with duty recommended in datasheet is 50/50.
// Latch
// - take the latch frequency as 1/10 ~ 100Khz to covert ~ 10 cycle of clock and convenience for devide frequency
// - take resolution base on maximum frequency of 1MHz and duty must > 80%
// + so 1MHz / 100KHz = 10 -> the resolution level must < 10 value
// + resolution: 2^x < 10 -> x = 0, 1, 2 ,3
// + 2^8 = 8 -> minimum duty = 0% , maximum duty = 87.5% -> satisfy the condition


// [application for 4 74HC165]
// we arm input 32 pin with 4 series 74hc165
// - clock keep 1MHz
// - latch frequency : 1MHz devide 40 -> 25KHz
// + shift time / pre latch require : (32 + 1 ) / 40 
// + resolution option: 5 -> 0 (note alway use max resolution if can. the function divide of esp32 have limit lowest resolution)
// + maximum duty : 1/(2^5)  * (2^5 - 1) = 96%
// + ideal duty time: 9/10 (90%) -> ~ 29/2^5
// -> shift time / prelatch : 29/ 32 > 33 / 40 (ok)


/**
 * @brief Initial input gate
 * 
 * - set up GPIO input signal
 * 
 * - set up 2 pwm high speed signal timer and channel
 * - 1 for shift clock
 * - 1 for latch
 * 
 * @note This function will using 2 pwm timer 0, 1 high speed and 2 channel high speed 0, 1
 * @warning shouldn't using pwm timer 0 and 1 and 2 channel high speed 0, 1 for another perpose.
 * 
 * @param data_struct Input 'pointer' to your 'struct pointer' NULL , 
 * function will alloc for you data register and flags 
 * 
 * @retval 0 if ok, != 0 if something wrong
 */
uint8_t gpio_74HC165_init_input_gate(gpio_74hc165_data_t** data_struct);

/**
 * @brief Deinit input gate
 */
uint8_t gpio_74HC165_de_init_input_gate(gpio_74hc165_data_t** data_struct);

/**
 * @brief Start capture event
 * 
 * - Capture random event by continuos shift data from series 74HC165
 * 
 * - This function register an event with gpio interrupt service
 * 
 * - When >= 1 signal have logic differ form normal logic, it will be capture
 * 
 * - interrupt will set flag capture
 * 
 * @retval 0 if ok, !=0 if something wrong
 */
uint8_t gpio_74HC165_start_capture_random_events();

/**
 * @brief Optional stop capture event
 */
uint8_t gpio_74HC165_stop_capture_random_events();


/**
 * @brief Check flag, if set then renew value for 32bit register
 * 
 * - 32bit register, each bit equal with 1 state of input pin (total 32pin of series 4 x 74hc165)
 * 
 * - when flag not set -> no update register
 * 
 * - when flag is set -> turn off pwm and push clock and latch by hand to get exact 32 value
 * 
 * @retval 0 if ok, !=0 if something wrong
 */
uint8_t gpio_74HC165_check_flag_and_renew_register(gpio_74hc165_data_t* data_struct, uint8_t* set_state);


#endif