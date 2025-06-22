// this library code for series 4 ic 74hc165 to expand input signal to 32 pin
// available for : push button, keypad, and another logic signals have short temporary status change

// hardware design: 165(no1) -> no2 -> no3 -> no4 -> esp32

// modify comming soon: 
// + hint gpio_74hc165_data_t and typedef enum and API to read any bit of register
// + no use pwm driver, replace with API read and pooling driver
//      + advantage: 
//          - driver read new data 74hc165 series and update register
//          (don't need mutex lock, ) 
//          after a period of programming
//          - No pooling or continuous interrupt required
//          - Don't need pull each input get 74hc165 (keep idle state)
//          - Driver help user get newest value with less waiting time 
//              by only read update register
//              than using callback read API (shift, latch, update)
//          - reduced complexity compared to using interrupts and pwm
//      + disadvantage:
//          - Driver needs to read data and blocking takes a long time
//          - increase queuing delay if many requests
// + Fully rtos -> no use esp_rom_delay_us -> use vTaskDelay(pdMS_TO_TICKS(1));
//       + advantage:
//          - reduce hanging another peripheral
//       + disadvantage:
//          - delay time minimum 'ms' , but esp_rom_delay_us can keep core busy waiting 'us'
//       + solution: never stop driver, loop infinity
//          because (each time driver read a bit, it was sleep some ms)


// Note:
//   74HC165 chain: no1 -> no2 -> no3 -> no4 -> ESP32
//   Logical bit order: no1 = bits 0–7, no2 = 8–15, ..., no4 = 24–31
//
// Problem:
//   ESP32 receives bits in reverse order: bit 31 first, then 30 ... down to bit 0
//   So raw data is bit-reversed from logical device order
//
// Solution:
//   To map correctly:
//     - Reverse the bit order after receiving
//     - For each bit i (0 ≤ i < N), store it at position: [N - 1 - i]
//     - Where N = number of bits in the chain (ex 32)
//   This will make bit 0 correspond to no1, bit 31 to no4


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

// rtos
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// delay busy wait us
#include "esp_rom_sys.h"

// user define
#include "gpio_setup_handle.h"

#define GPIO_74HC165_CLOCK_PIN                 CONFIG_GPIO_74HC165_CLOCK_PIN
#define GPIO_74HC165_LATCH_PIN                 CONFIG_GPIO_74HC165_LATCH_PIN
#define GPIO_74HC165_DATA_SHIFT_IN_PIN         CONFIG_GPIO_74HC165_DATA_SHIFT_IN

#define GPIO_74HC165_DATA_SHIFT_IN_LEN         (32) // only update <=32

typedef struct{
    uint32_t reg_state;
    // << this reg state will wrong by reversed order input
    // << use "gpio_74HC165_current_input_reg" will reorder true state
    // and "gpio_74hc165_in_num_t" to get value
} gpio_74hc165_data_t;


typedef enum
{
    GPIO_74HC165_INPUT_NUM_0 = 0,
    GPIO_74HC165_INPUT_NUM_1,
    GPIO_74HC165_INPUT_NUM_2,
    GPIO_74HC165_INPUT_NUM_3,
    GPIO_74HC165_INPUT_NUM_4,
    GPIO_74HC165_INPUT_NUM_5,
    GPIO_74HC165_INPUT_NUM_6,
    GPIO_74HC165_INPUT_NUM_7,
    GPIO_74HC165_INPUT_NUM_8,
    GPIO_74HC165_INPUT_NUM_9,
    GPIO_74HC165_INPUT_NUM_10,
    GPIO_74HC165_INPUT_NUM_11,
    GPIO_74HC165_INPUT_NUM_12,
    GPIO_74HC165_INPUT_NUM_13,
    GPIO_74HC165_INPUT_NUM_14,
    GPIO_74HC165_INPUT_NUM_15,
    GPIO_74HC165_INPUT_NUM_16,
    GPIO_74HC165_INPUT_NUM_17,
    GPIO_74HC165_INPUT_NUM_18,
    GPIO_74HC165_INPUT_NUM_19,
    GPIO_74HC165_INPUT_NUM_20,
    GPIO_74HC165_INPUT_NUM_21,
    GPIO_74HC165_INPUT_NUM_22,
    GPIO_74HC165_INPUT_NUM_23,
    GPIO_74HC165_INPUT_NUM_24,
    GPIO_74HC165_INPUT_NUM_25,
    GPIO_74HC165_INPUT_NUM_26,
    GPIO_74HC165_INPUT_NUM_27,
    GPIO_74HC165_INPUT_NUM_28,
    GPIO_74HC165_INPUT_NUM_29,
    GPIO_74HC165_INPUT_NUM_30,
    GPIO_74HC165_INPUT_NUM_31
} gpio_74hc165_in_num_t;


/**
 * **********************************************************
 * APIs
 * **********************************************************
 */
//============================== Basic functions ============== ==============================
//============================== ============================== ==============================

/**
 * @brief Initial input gate (only 1)
 * 
 * - set up GPIO input signal
 * 
 * 
 * @retval 0 if ok, != 0 if something wrong
 */
uint8_t gpio_74HC165_init_input_gate();

/**
 * @brief Deinit input gate
 */
uint8_t gpio_74HC165_de_init_input_gate();

/**
 * @brief Get current input register value
 * @note after get reg, AND with (1<< gpio_74hc165_in_num_t)
 */
uint32_t gpio_74HC165_current_input_reg();


#endif