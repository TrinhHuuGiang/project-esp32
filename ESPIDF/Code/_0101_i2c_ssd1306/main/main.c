/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c_master_handle.h"

#include "i2c_ssd1306.h"

#define SSD1306_ADDR (0x3C)

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

void app_main(void)
{
    // check enable debug
    #if CONFIG_DEBUG_ENABLE != 0
    fprintf(stderr,"[Debug mode]\n");
    #else
    fprintf(stderr,"[No-debug mode]\n");
    #endif

    i2c_config_t* master_conf=NULL;

    // config i2c master

    i2c_master_init_config(&master_conf);

    i2c_master_setup_hardware(master_conf);

    i2c_master_install_driver(master_conf);
    
    i2c_master_free_config(&master_conf);

    // send to i2c ssd1306

    i2c_ssd1306_on_off_screen(SSD1306_ADDR, 1);

    i2c_ssd1306_choose_addressing_mode(SSD1306_ADDR, I2C_SSD1306_HOR_ADDR_MODE);

    uint8_t count = 0;
    uint8_t reverse_state = 0;
    // while (1)
    // {   
    //     i2c_ssd1306_print_something(SSD1306_ADDR, epd_bitmap_allArray[count], BIT_MAP_IMAGE_128x64_USERDEFINE_SIZE);
    //     if((++count) >= BIT_MAP_IMAGE_128x64_USERDEFINE_QUANTITY)
    //     {
    //         count=0;
    //         reverse_state = 1- reverse_state;
    //         i2c_ssd1306_reverse_light_display(SSD1306_ADDR, reverse_state);
    //     }
    //     vTaskDelay(pdMS_TO_TICKS(1000)); // Nghỉ 10ms (1 giây)
    // }

    while (1)
    {   
        i2c_ssd1306_print_something(SSD1306_ADDR, &ASCII_normal_8x8_bitmap[count][0], ASCII_NORMAL_8x8_BMP_SIZE);
        if((++count) >= ASCII_NORMAL_8x8_BMP_QUANTITY)
        {
            count=0;
            reverse_state = 1- reverse_state;
            i2c_ssd1306_reverse_light_display(SSD1306_ADDR, reverse_state);
        }
        vTaskDelay(pdMS_TO_TICKS(200)); // Nghỉ 200ms
    }
}
