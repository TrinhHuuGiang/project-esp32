// this library code for series 4 ic 74hc565 to expand output signal to 32 pin
// depend on internal peripheral: SPI, GPIO (latch clock)
// hardware design: ESP32 -> 595 (no1) -> no2 -> no3 -> no4

// available for : keypad, relay, ... , and another device need control on off logic


// update comming soon: reverse output flag ( depend on design, we want 
// implement order by High index to low index or reverse)

#ifndef _GPIO_74HC565_H_
#define _GPIO_74HC565_H_



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

// user define
// #include "gpio_setup_handle.h" // :) we use default handle cs pin of spi, it support 
                                    //raise LOW to HIGH for latch (LOW when shift and HIGH after shift out)
#include "spi_master_handle.h"

// Latch shift in register (ST_CP pin)
#define GPIO_74HC595_LATCH_PIN       CONFIG_GPIO_74HC595_LATCH_PIN   // GPIO 5 LATCH

// Clock SPI (1MHz at 3.3V)
#define GPIO_74HC595_SPI_SCK_FREQ    SPI_MASTER_CLOCK_FREQ_1M
#define GPIO_74HC595_SPI_CLOCK_MODE  SPI_MASTER_CLOCK_MODE_0

#define GPIO_74HC595_SPI_COMMAND_LEN     (0)   // no command bit
#define GPIO_74HC595_SPI_ADDRESS_LEN     (0)   // 74hc :) no address
#define GPIO_74HC595_SPI_DUMMY_LEN       (0)   // send immediate
#define GPIO_74HC595_SPI_TX_BIT_DATA_LEN (32)  // only update <=32 output


// init state output
#define GPIO_74HC595_INIT_PINS_MOD       (0xFFFFFFFF) 
#define GPIO_74HC595_INIT_PINS_VALUE     (0xFFFFFFFF) // output full level 1 (modify if needed)


// Note:
// - ESP32 uses little-endian (LSByte is stored at the lowest address) memory layout:
//      + example: 0x11223344 -> Memory (byte-wise) = [0x44][0x33][0x22][0x11] 
//      + solution to send as big-endian over SPI (MSByte first), reverse the byte order 
//          of each 32-bit element before sending.
// - default SPI send MSBit
//      example: 0b11000100 -> send -> 1 then 1 then 0 0 0 1 0 0
//      solution send to LSBit device: config spi_transaction_t 

// in this library, a value 32bit keep 32 output 
//    0b (31)   ....    (0)
//       MSBit          LSBit
//    Little endian: (7-0)  (15-8)   (23-16)  (31-24)
//    send MSBit:    (7-0)  (15-8)   (23-16)  (31-24)
//    595 series:    4(7-0) 3(7-0)   2(7-0)   1(7-0)
//      => last register get true order bit but wrong order byte
//      => devboard need register 4 keep bit (31-24)
//      => solution: reverse byte before send
//    result: 
//      send MSBit:    (31-24)  (23-16)   (15-8)  (7-0)
//      595 series:    4(7-0)   3(7-0)   2(7-0)   1(7-0)




// Use this order for cpu calculate with bit order
// note: ignore id port 0-> 7 if general perpose output
// see note below
typedef enum
{
    // id port 0->3 use for analog select 74hc4067
    GPIO_74HC595_OUTPUT_AS0 = 0,
    GPIO_74HC595_OUTPUT_AS1,
    GPIO_74HC595_OUTPUT_AS2,
    GPIO_74HC595_OUTPUT_AS3,

    // id port 4->7 use for micro sd, spi chipselect device 0 1 2
    GPIO_74HC595_OUTPUT_SPI_SDCS,
    GPIO_74HC595_OUTPUT_SPI_CS0,
    GPIO_74HC595_OUTPUT_SPI_CS1,
    GPIO_74HC595_OUTPUT_SPI_CS2,
    
    // id port 8->31 general 24 port output
    GPIO_74HC595_OUTPUT_NUM_0, 
    GPIO_74HC595_OUTPUT_NUM_1,
    GPIO_74HC595_OUTPUT_NUM_2,
    GPIO_74HC595_OUTPUT_NUM_3,
    GPIO_74HC595_OUTPUT_NUM_4,
    GPIO_74HC595_OUTPUT_NUM_5,
    GPIO_74HC595_OUTPUT_NUM_6,
    GPIO_74HC595_OUTPUT_NUM_7,
    GPIO_74HC595_OUTPUT_NUM_8,
    GPIO_74HC595_OUTPUT_NUM_9,
    GPIO_74HC595_OUTPUT_NUM_10,
    GPIO_74HC595_OUTPUT_NUM_11,
    GPIO_74HC595_OUTPUT_NUM_12,
    GPIO_74HC595_OUTPUT_NUM_13,
    GPIO_74HC595_OUTPUT_NUM_14,
    GPIO_74HC595_OUTPUT_NUM_15,
    GPIO_74HC595_OUTPUT_NUM_16,
    GPIO_74HC595_OUTPUT_NUM_17,
    GPIO_74HC595_OUTPUT_NUM_18,
    GPIO_74HC595_OUTPUT_NUM_19,
    GPIO_74HC595_OUTPUT_NUM_20,
    GPIO_74HC595_OUTPUT_NUM_21,
    GPIO_74HC595_OUTPUT_NUM_22,
    GPIO_74HC595_OUTPUT_NUM_23
} gpio_74hc595_gate_num_t;

typedef struct
{
    uint32_t reg_state;  // keep old state of output gate
                            // modify by 'gpio_74hc595_gate_num_t'

    // spi handle
    spi_device_handle_t device_handle;
    
    spi_transaction_ext_t* transaction_config;   // prepare/ polling / delete transaction

} gpio_74hc595_data_t;


/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

/**
 * Init output gate
 * - Init and regist spi device, latch + transaction
 * - Clean dummy bit after chip reset
 */
uint8_t gpio_74HC595_init_output_gate();

/**
 * De init output gate
 * - De init transaction, unregist then deinit spi device
 */
uint8_t gpio_74HC595_de_init_output_gate();

/**
 * Return current 32bit output (no thread safe, can read old value)
 * 
 * Input a uint32_t and get a copy of current reg_state
 */
uint8_t gpio_74HC595_get_current_output_bit(uint32_t* reg_state);


/**
 * Pins order: Input OR (1)   represent for where number pin need set logic
 * Pins value: Input OR (1/0) represent for what type of logic set for these pin
 * 
 * Execute:
 * + add value to transaction -> shift out
 * + latch value
 * + update reg_state
 * 
 * Note logic:
 * + pins_value &= pins_order;      // :) if user input excess value of order pins, clean it
 * + temp_reg_state = reg_state;
 * + temp_reg_state &=~ pins_order; // clear old bit value
 * + temp_reg_state |= pins_value;
 * + If set success => do update reg_state = temp_reg_state
 * + Or return err
 */
uint8_t gpio_74HC595_set_output_bit(uint32_t pins_order, uint32_t pins_value);

#endif // _GPIO_74HC565_H_