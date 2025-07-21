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

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

#include "i2s_max98357.h"


 /**
 * **********************************************************
 * Prototype
 * **********************************************************
 */

 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// turn on by set gpio SD to HIGH
uint8_t i2s_max98357_turn_on()
{
    // reset pin
    if(gpio_setup_reset_pin_to_origin(I2S_MAX98357_SD_PIN)) return 1;

    // no pull, set ouput
    if(gpio_setup_io_direction(I2S_MAX98357_SD_PIN, GPIO_MODE_OUTPUT)) return 2; 

    // set logic output
    if(gpio_setup_output_logic_level(I2S_MAX98357_SD_PIN, 1)) return 3;

    return 0;
}


// turn on by set gpio SD to LOW
uint8_t i2s_max98357_turn_off()
{
    // reset pin <- need it if turn off call first after reset
    if(gpio_setup_reset_pin_to_origin(I2S_MAX98357_SD_PIN)) return 1;

    // no pull, set ouput
    if(gpio_setup_io_direction(I2S_MAX98357_SD_PIN, GPIO_MODE_OUTPUT)) return 2; 

    // set logic output is low
    if(gpio_setup_output_logic_level(I2S_MAX98357_SD_PIN, 0)) return 3;

    return 0;
}