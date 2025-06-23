// this external periphs depend
// SPI: on series 74hc595 select S0->S3 ( output port 0->3)
//      note: 74hc595 has to init before use this library
// ADC: read value after set select port
//      note: each adc read value in "adc1_setup_handle.h" has been config delay 10ms
//          before read (stable time)
// harware design: link adc1 chan6 gpio 34 with sig pin of 74hc4067
//                  so only select channel here and read adc by "adc1_setup_handle.h" funtions


// thead not safe: 74hc4067 need adc and spi 
//      so after set analog channel by spi (74hc595) then while read adc
//      someone (16 channel) can change the analog input channel 

// solution: use mutex and perform channel switching and reading in the same api

#ifndef _ADC_74HC565_H_
#define _ADC_74HC565_H_



/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
// c
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

// user define
#include "adc1_setup_handle.h"
#include "gpio_74hc595.h"


#include "__task_sync.h"


// 74hc4067
// out port     : 3   2  1  0
// analog select: S3  S2 S1 S0
//       chan 0 : 0   0  0  0 -> enum 0
//       chan 1 : 0   0  0  1 -> enum 1
// ...
//       chan 15: 1   1  1  1 -> enum 15

// 74hc595 clear 4 LSB (3-0) == (S3-S0) then update by or with 74hc4067 enum  

// 74hc4067 channel
typedef enum
{
    ADC_74HC4067_CHAN_0 = 0,// special use for measure battery voltage on board
    ADC_74HC4067_CHAN_1,
    ADC_74HC4067_CHAN_2,
    ADC_74HC4067_CHAN_3,
    ADC_74HC4067_CHAN_4,
    ADC_74HC4067_CHAN_5,
    ADC_74HC4067_CHAN_6,
    ADC_74HC4067_CHAN_7,
    ADC_74HC4067_CHAN_8,
    ADC_74HC4067_CHAN_9,
    ADC_74HC4067_CHAN_10,
    ADC_74HC4067_CHAN_11,
    ADC_74HC4067_CHAN_12,
    ADC_74HC4067_CHAN_13,
    ADC_74HC4067_CHAN_14,
    ADC_74HC4067_CHAN_15
} adc_74hc4067_chan_t;






/**
 * **********************************************************
 * Sync 
 * (never use these mutex if call any APIs below, it included)
 * (only use when other device is conflicted with this peripheral)
 * 
 * Note:
 * + used it in "spi_sd_mount_to_fs"
 * + anytime interact with spi sdcard file system: fprintf, fwrite, ...
 * + remember take before use and release after done
 * *********************************************************
 * */

// get periph mutex
void take_adc_74hc4067_mutex();

// release periph mutex
void release_adc_74hc4067_mutex();





/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

 

// select analog input channel then read raw
uint8_t adc_74hc4067_select_input_channel_read_raw(adc_74hc4067_chan_t a_chan, int* value);


// select analog input channel then read millis vol
uint8_t adc_74hc4067_select_input_channel_read_mV(adc_74hc4067_chan_t a_chan, int* value);



#endif