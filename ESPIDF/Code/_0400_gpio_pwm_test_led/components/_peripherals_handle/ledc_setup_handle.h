// esp32 have 4 timer for 8 channel high speed
// and   have 4 timer for 8 channel slow speed
// here we only use 4 timer for 4 channel high speed
// these function only config 1 - 1 (timer0 - channel0, .. , timer3 - channel3)


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
#include "ledc.h"
#include "esp_err.h"        // const char *esp_err_to_name(esp_err_t code);

//user
#include "_peripherals_err.h"


/**
 * **********************************************************
 * APIs
 * **********************************************************/

// timer config

// start channel ledc

// stop channel ledc

// change PWM Duty cycle software

// change PWM auto by hardware

// change timer frequency and resolution of ledc channel, change PWM duty cycle ()

#endif