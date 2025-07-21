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
