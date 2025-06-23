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
