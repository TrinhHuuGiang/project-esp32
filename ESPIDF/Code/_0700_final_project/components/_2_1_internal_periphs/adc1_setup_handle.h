// Only using ADC1. ADC2 can't be used when Wi-Fi is active
// using this library for 74hc4067.

#ifndef _ADC1_SETUP_HANDLE_H_
#define _ADC1_SETUP_HANDLE_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
// C standard headers
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

// ESP-IDF headers
#include "driver/adc.h"
#include "esp_err.h"  // esp_err_to_name()

// User-defined includes
#include "__task_sync.h"
#include "_peripherals_err.h"

// ADC channel config
#define ADC1_SETUP_CHANNEL          CONFIG_ADC_INPUT_CHANNEL  // (hardware fixed) ADC1_CH6 -> GPIO34
#define ADC1_SETUP_STABLE_MS        10  // wait before read (ms), must be >= 10

#define ADC1_SETUP_RESOLUTION       ADC_WIDTH_BIT_12  // 9–12 bits
#define ADC1_SETUP_RAW_MAX          ((1 << 12) - 1)  // 4095 for 12-bit ADC

// Attenuation 11dB (0-3905mV) drop to (0-1100mV vref) : linear range ≈ 150 mV to 2450 mV
// but esp32 limit at 3.3v -> Vrange input: 0-3300mV
#define ADC1_SETUP_VOFFSET_MV       0
#define ADC1_SETUP_VRANGE_MV        (3905 - ADC1_SETUP_VOFFSET_MV)




/**
 * **********************************************************
 * Sync 
 * (never use these mutex if call any APIs below, it included)
 * (only use when other device is conflicted with this peripheral)
 * *********************************************************
 * */

// get periph mutex
void take_adc1_mutex();

// release periph mutex
void release_adc1_mutex();





/**
 * **********************************************************
 * APIs
 * **********************************************************
 */


// Configure ADC1 channel with attenuation = 11 dB
// :) this function return !0 is fail, but no log error (comming soon)
uint8_t adc1_setup_init_atten11dB(void);

/**
 * Get raw ADC value.
 * @return Value from 0 to 4095 if width = 12 bits.
 * @retval -1 if cant read
 */
int adc1_setup_read_raw_atten11dB(void);

/**
 * Get voltage from ADC in millivolts.
 * Linearly mapped from raw → 150mV–2450mV.
 * Full range return 0-3300mV (limit by esp32 gpio)
 * If input voltage > 2.45V, result will be not linear
 *
 * @return Voltage in millivolts (0 – 3300 mV)
 * @retval -1 if cant read
 */
int adc1_setup_read_voltage_atten11dB(void);

#endif  // _ADC_SETUP_HANDLE_H_
