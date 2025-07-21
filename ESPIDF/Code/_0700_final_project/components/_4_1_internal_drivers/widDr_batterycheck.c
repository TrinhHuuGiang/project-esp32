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
#include "widDr_batterycheck.h"

/**
 * **********************************************************
 * Code
 * **********************************************************
 */
// read 74hc4067 channel 0 then translate into real voltage
uint8_t winDr_get_battery_mV(int* value)
{
    int temp_mV;
    if(adc_74hc4067_select_input_channel_read_mV(ADC_74HC4067_CHAN_0, &temp_mV))
        return 1;
    
    if(temp_mV < 0)
    {
        *value = temp_mV; // not available
    }
    else
    {
        *value = ((double)temp_mV / 22.0 ) * 122 + 700 ; // 700mV of diode, 22k and 100k is voltage divider
    }
    

    return 0;
}
