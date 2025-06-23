/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

#include "adc1_setup_handle.h"


/**
 * **********************************************************
 * Variable
 * **********************************************************
 */

extern __task_sync_t* g_task_sync_tools; // get sync tools 

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// get periph mutex
void take_adc1_mutex()
{
    xSemaphoreTake(g_task_sync_tools->adc_setup_handle_mutex, portMAX_DELAY);
}

// get periph mutex
void release_adc1_mutex()
{
    xSemaphoreGive(g_task_sync_tools->adc_setup_handle_mutex);
}




// config adc attenuation 11db
// -> maximum input 3.3v, but linear range from 150 - 2450 mV (0.1-2.45v)
void adc1_setup_init_atten11dB(void)
{
	//take mutex
	take_adc1_mutex();

    
    adc1_config_width(ADC1_SETUP_RESOLUTION);
    adc1_config_channel_atten(ADC1_SETUP_CHANNEL, ADC_ATTEN_DB_11);

    //give mutex
    release_adc1_mutex();
}

// get raw adc value 
int  adc1_setup_read_raw_atten11dB(void)
{
	//take mutex
	take_adc1_mutex();

    vTaskDelay(pdMS_TO_TICKS(ADC1_SETUP_STABLE_MS)); // wait stable

    int raw = adc1_get_raw(ADC1_SETUP_CHANNEL);
    if (raw < 0 || raw > ADC1_SETUP_RAW_MAX) {
        // param error or something wrong
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(ADC1_SETUP_GET_RAW_FAILED, __FILE__, __LINE__, "adc1 read failed");
        #endif
        
        
        //give mutex
        release_adc1_mutex();
        return -1;
    }
    
    //give mutex
    release_adc1_mutex();
    return raw;
}

// get 
int  adc1_setup_read_voltage_atten11dB(void)
{
    // :) no mutex here

    int raw = adc1_setup_read_raw_atten11dB();
    if (raw < 0) return -1;

    // calculate [0..4095] → [150..2450] mV
    int voltage = ADC1_SETUP_VOFFSET_MV + 
                  (raw * ADC1_SETUP_VRANGE_MV) / ADC1_SETUP_RAW_MAX;

    return voltage;
}


