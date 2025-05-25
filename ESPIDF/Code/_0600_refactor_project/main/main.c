// common libs
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// esp libs
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "_peripherals_err.h"

#include "spi_master_handle.h"

#include "gpio_74hc595.h"

#define main_tag "MAIN_LOG"

void app_main(void)
{
    int ret  = 0;

    // [SPI]
    // varable
    spi_bus_config_t* spi_master_bus_conf = NULL;

    if(spi_master_init_bus_config(&spi_master_bus_conf) != PERIPH_OK) {ret = 1; goto tag_main_log;}
    if(spi_master_install_bus_config(spi_master_bus_conf) != PERIPH_OK) {ret = 2; goto tag_main_log;}    
    spi_master_free_bus_config(&spi_master_bus_conf);

    // 595
    if(gpio_74HC595_init_output_gate()) {ret = 3; goto tag_main_log;}

    // loop
    uint32_t reg_595;

    gpio_74hc595_gate_num_t pins_order = 0xFFFFFFFF;
    
    uint32_t pin_value = 0x55555555; // 0xAAAAAAAA

    uint8_t odd_even = 0;

    while(1)
    {
        if(gpio_74HC595_get_current_output_bit(&reg_595)){ret = 4; goto tag_main_log;};

        if(gpio_74HC595_set_output_bit(pins_order, pin_value)){ret = 5; goto tag_main_log;};

        if(odd_even)
        pin_value = 0x55555555;
        else
        pin_value = 0xAAAAAAAA;

        odd_even = 1-odd_even;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }


tag_main_log:
    ESP_LOGW(main_tag, "Err code: %d" ,ret);
    return;
}
