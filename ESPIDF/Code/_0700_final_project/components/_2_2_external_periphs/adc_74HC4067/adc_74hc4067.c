/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "adc_74hc4067.h"

 
/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
extern __task_sync_t* g_task_sync_tools; // get sync tools 

 
/**
 * **********************************************************
 * Codes
 * **********************************************************
 */


// get periph mutex
void take_adc_74hc4067_mutex()
{
    xSemaphoreTake(g_task_sync_tools->adc_74hc4067_mutex, portMAX_DELAY);
}

// release periph mutex
void release_adc_74hc4067_mutex()
{
    xSemaphoreGive(g_task_sync_tools->adc_74hc4067_mutex);
}




// select analog input channel then read raw
uint8_t adc_74hc4067_select_input_channel_read_raw(adc_74hc4067_chan_t a_chan, int* value)
{
	//take mutex
	take_adc_74hc4067_mutex();

    uint32_t pins_value = a_chan & 0x0F;
    if(gpio_74HC595_set_output_bit(0x0F, pins_value))
    {
        //give mutex
        release_adc_74hc4067_mutex();
        return 1;
    }

    // read
    *value = adc1_setup_read_raw_atten11dB();

    
    //give mutex
    release_adc_74hc4067_mutex();
    return 0;
}


// select analog input channel then read millis vol
uint8_t adc_74hc4067_select_input_channel_read_mV(adc_74hc4067_chan_t a_chan, int* value)
{
	//take mutex
	take_adc_74hc4067_mutex();

    uint32_t pins_value = a_chan & 0x0F;
    if(gpio_74HC595_set_output_bit(0x0F, pins_value)) 
    {
        //give mutex
        release_adc_74hc4067_mutex();
        return 1;
    }


    // read
    *value = adc1_setup_read_voltage_atten11dB();

    
    //give mutex
    release_adc_74hc4067_mutex();
    return 0;
}
