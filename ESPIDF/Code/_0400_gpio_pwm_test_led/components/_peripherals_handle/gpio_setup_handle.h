// This library concentrate on GPIO using general perpose I/O funtion
// if the pad of pin GPIO linked with another peripheral by GPIO matrix or config the IO_MUX, 
// the functions below may unused because digital read, write can be block by some peripherals
// (GPIO read still work with PWM)


// GPIO trapping when boot state: 0, 2 ,5, 12, 15
// + do not connect external pull resistor

// GPIO for PSRAM (no apply for esp32): 16,17
// if board using not devkitC, these pins should not use

// pin 34-39 do not have internal pull resistor
// these pin only input and only set input mode

// pin TXD and RXD (GPIO 1 and 3)
// only use for debug, print serial to terminal

// The ADC2 peripheral will be used by WIFI if wifi start
// do not using these pin function ADC with wifi


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
#include "driver/gpio.h"           // GPIO
#include "hal/gpio_types.h"
#include "esp_intr_alloc.h"
#include "esp_err.h"        // const char *esp_err_to_name(esp_err_t code);

//user
#include "_peripherals_err.h"


/**
 * **********************************************************
 * APIs
 * **********************************************************/


// =================================== Set up for independence GPIO    ===============================
// =================================== Reset, set direction            ===============================
// =================================== Get level (input), set level (ouput) ==========================
// gpio_reset_pin
_peripherals_err_t gpio_setup_reset_pin_to_origin(gpio_num_t GPIO_pin_num);

// gpio_set_direction
_peripherals_err_t gpio_setup_io_direction(gpio_num_t GPIO_pin_num, gpio_mode_t GPIO_direct);

// gpio_set_pull_mode
_peripherals_err_t gpio_setup_pull_res(gpio_num_t gpio_num, gpio_pull_mode_t pull);

// gpio_set_level
_peripherals_err_t gpio_setup_output_logic_level(gpio_num_t gpio_num, uint32_t level);

// gpio_get_level
_peripherals_err_t gpio_setup_get_logic_level(gpio_num_t gpio_num,  uint8_t* logic_level);



// =================================== Only for all GPIO               ===============================
// =================================== Install & uninstall isr service ===============================
// =================================== For all GPIO (only call 1 time) ===============================
// gpio_install_isr_service
_peripherals_err_t gpio_setup_install_isr_service_for_gpio_system();


// gpio_uninstall_isr_service
_peripherals_err_t gpio_setup_un_install_isr_service_for_gpio_system();



// =================================== add isr handler for one GPIO    ===============================
// =================================== =============================== ===============================
// gpio_isr_handler_add
_peripherals_err_t gpio_setup_add_handler_for_pin(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args);

// gpio_isr_handler_remove
_peripherals_err_t gpio_setup_remove_handler_for_pin(gpio_num_t gpio_num);

// gpio_set_intr_type
_peripherals_err_t gpio_setup_isr_active_type_for_pin(gpio_num_t gpio_num, gpio_int_type_t intr_type);

// gpio_intr_enable();
_peripherals_err_t gpio_setup_enable_interrupt_for_pin(gpio_num_t gpio_num);

// gpio_intr_disable
_peripherals_err_t gpio_setup_disable_interrupt_for_pin(gpio_num_t gpio_num);

// =================================== Dis/en wake up                  ===============================
// =================================== =============================== ===============================
// gpio_wakeup_enable
_peripherals_err_t gpio_setup_bind_wakeup_for_pin(gpio_num_t gpio_num, gpio_int_type_t intr_type);

// gpio_wakeup_disable
_peripherals_err_t gpio_setup_un_bind_wakeup_for_pin(gpio_num_t gpio_num);


#endif