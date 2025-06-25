

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
uint8_t adc1_setup_init_atten11dB(void)
{
	//take mutex
	take_adc1_mutex();
    
    esp_err_t ret = adc1_config_width(ADC1_SETUP_RESOLUTION);

    if(ret) return 1;

    ret = adc1_config_channel_atten(ADC1_SETUP_CHANNEL, ADC_ATTEN_DB_11);

    if(ret) return 2;

    //give mutex
    release_adc1_mutex();

    return 0;
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
    // :) no mutex here double call will deadlock

    int raw = adc1_setup_read_raw_atten11dB();
    if (raw < 0) return -1;

    // calculate [0..4095] → [0..3300] mV
    int voltage = ADC1_SETUP_VOFFSET_MV + 
                 ( (double)(raw) / (double)ADC1_SETUP_RAW_MAX ) * ADC1_SETUP_VRANGE_MV;

    // fprintf(stderr,"raw: %d, voltage:%d\n", raw, voltage);

    return voltage;
}


