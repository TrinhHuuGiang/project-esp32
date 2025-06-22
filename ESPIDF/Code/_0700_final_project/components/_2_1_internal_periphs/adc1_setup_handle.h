// Only using ADC1. ADC2 can't be used when Wi-Fi is active
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
#define ADC1_SETUP_CHANNEL          CONFIG_ADC_INPUT_CHANNEL  // e.g., ADC1_CH6 -> GPIO34
#define ADC1_SETUP_STABLE_MS        10  // wait before read (ms), must be >= 10

#define ADC1_SETUP_RESOLUTION       ADC_WIDTH_BIT_12  // 9–12 bits
#define ADC1_SETUP_RAW_MAX          ((1 << 12) - 1)  // 4095 for 12-bit ADC

// Attenuation 11dB: linear range ≈ 150 mV to 2450 mV
#define ADC1_SETUP_VOFFSET_MV       150
#define ADC1_SETUP_VRANGE_MV        (2450 - ADC_SETUP_VOFFSET_MV)  // 2300 mV




/**
 * **********************************************************
 * Sync 
 * (never use these mutex if call any APIs below, it included)
 * (only use when other device is conflicted with this peripheral)
 * *********************************************************
 * */

// get periph mutex
void take_adc1_mutex();

// get periph mutex
void release_adc1_mutex();





/**
 * **********************************************************
 * APIs
 * **********************************************************
 */


// Configure ADC1 channel with attenuation = 11 dB
void adc1_setup_init_atten11dB(void);

/**
 * Get raw ADC value.
 * @return Value from 0 to 4095 if width = 12 bits.
 */
int adc1_setup_read_raw_atten11dB(void);

/**
 * Get voltage from ADC in millivolts.
 * Linearly mapped from raw → 150mV–2450mV.
 * If input voltage > 2.45V, result will be clipped to 2450 mV.
 *
 * @return Voltage in millivolts (150 – 2450 mV)
 */
int adc1_setup_read_voltage_atten11dB(void);

#endif  // _ADC_SETUP_HANDLE_H_
