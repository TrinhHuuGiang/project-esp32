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
#include "esp_log.h"

#include "i2c_master_handle.h"

#include "i2c_ds3231.h"
#include "i2c_ssd1306.h"

#include <string.h>

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

    // ds3231
    ds3231_reg_t* reg_tb = NULL;
    ESP_LOGI("init reg table", "code: %d", i2c_ds3231_init_reg_table(&reg_tb));

    uint16_t h=0,m=0,s=0,d=0,dt=0,mth=0,y=0;

    // ssd1306
    int contr = 0; // set contrast (from 0 - 100 :V max is 255)
    int tg_display = 0;

    i2c_ssd1306_on_off_screen(SSD1306_ADDR, 1);

    i2c_ssd1306_choose_addressing_mode(SSD1306_ADDR, I2C_SSD1306_PAGE_ADDR_MODE);

    char array_print[17] = {0};
    char sub_str1[17] = {0};
    char sub_str2[17] = {0};
    char sub_str3[17] = {0};

    fprintf(stderr,"[1]\n");

    // print hour
    i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 0);

    fprintf(stderr,"[11]\n");

    i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);

    fprintf(stderr,"[111]\n");

    strcat(array_print,"    hh:mm:ss    ");

    fprintf(stderr,"[1111]\n");

    for(int i = 0; i < 16; i++)
    {
        i2c_ssd1306_print_something(SSD1306_ADDR, &ASCII_normal_8x8_bitmap[array_print[i]-ASCII_PRINT_ABLE_OFFSET][0],ASCII_NORMAL_8x8_BMP_SIZE);
    }

    fprintf(stderr,"[2]\n");

    // print date
    i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 3);
    i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);

    memset(array_print,0,17);
    strcat(array_print,"   dd:mm:yyyy   ");
    for(int i = 0; i < 16; i++)
    {
        i2c_ssd1306_print_something(SSD1306_ADDR, &ASCII_normal_8x8_bitmap[array_print[i]-ASCII_PRINT_ABLE_OFFSET][0],ASCII_NORMAL_8x8_BMP_SIZE);
    }
    
    fprintf(stderr,"[3]\n");

    //print day
    i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 6);
    i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);

    memset(array_print,0,17);
    strcat(array_print,"     To day     ");
    for(int i = 0; i < 16; i++)
    {
        i2c_ssd1306_print_something(SSD1306_ADDR, &ASCII_normal_8x8_bitmap[array_print[i]-ASCII_PRINT_ABLE_OFFSET][0],ASCII_NORMAL_8x8_BMP_SIZE);
    }

    //loop

    while (1)
    {   
        vTaskDelay(pdMS_TO_TICKS(500)); // Nghỉ 500ms cho ổn định

        ESP_LOGI("get time data","code: %d", i2c_ds3231_read_full_reg(reg_tb));
        // debug, get time
        ESP_LOGI("Date time code", "Code: \nHour %u : %u : %u - Day %u : Date %u : Month %u : Year %u",
        i2c_ds3231_translate_reg_to_time(*reg_tb, I2C_DS3231_REG_HOUR, &h),
        i2c_ds3231_translate_reg_to_time(*reg_tb, I2C_DS3231_REG_MIN, &m),
        i2c_ds3231_translate_reg_to_time(*reg_tb, I2C_DS3231_REG_SEC, &s),
        i2c_ds3231_translate_reg_to_time(*reg_tb, I2C_DS3231_REG_DAY, &d),
        i2c_ds3231_translate_reg_to_time(*reg_tb, I2C_DS3231_REG_DATE, &dt),
        i2c_ds3231_translate_reg_to_time(*reg_tb, I2C_DS3231_REG_MONTH, &mth),
        i2c_ds3231_translate_reg_to_time(*reg_tb, I2C_DS3231_REG_YEAR, &y)
        );
        // print time
        ESP_LOGI("Date time code", "Hour %u : %u : %u - Day %u : Date %u : Month %u : Year %u",
        h,m,s,d,dt,mth,y
        );
        
        // print to display
        // print hour
        i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 1);
        i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);

        memset(array_print,0,17);
        sprintf(sub_str1, "%02u",h);
        sprintf(sub_str2, "%02u",m);
        sprintf(sub_str3, "%02u",s);
        sprintf(array_print, "    %.2s:%.2s:%.2s    ",sub_str1,sub_str2,sub_str3);
        for(int i = 0; i < 16; i++)
        {
            i2c_ssd1306_print_something(SSD1306_ADDR, &ASCII_normal_8x8_bitmap[array_print[i]-ASCII_PRINT_ABLE_OFFSET][0],ASCII_NORMAL_8x8_BMP_SIZE);
        }

        // print date
        i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 4);
        i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);

        memset(array_print,0,17);
        sprintf(sub_str1, "%02u",dt);
        sprintf(sub_str2, "%02u",mth);
        sprintf(sub_str3, "%04u",y);
        sprintf(array_print, "   %.2s:%.2s:%.4s   ",sub_str1,sub_str2,sub_str3);
        for(int i = 0; i < 16; i++)
        {
            i2c_ssd1306_print_something(SSD1306_ADDR, &ASCII_normal_8x8_bitmap[array_print[i]-ASCII_PRINT_ABLE_OFFSET][0],ASCII_NORMAL_8x8_BMP_SIZE);
        }

        //print day
        i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 7);
        i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);

        memset(array_print,0,17);
        sprintf(array_print, "     To day     ");

        switch (d)
        {
        case 1:
            sprintf(array_print, "     Monday     ");
            break;
        case 2:
            sprintf(array_print, "     Tueday     ");
            break;
        case 3:
            sprintf(array_print, "     Wedday     ");
            break;
        case 4:
            sprintf(array_print, "     Thuday     ");
            break;
        case 5:
            sprintf(array_print, "     Friday     ");
            break;
        case 6:
            sprintf(array_print, "     Satday     ");
            break;
        case 7:
            sprintf(array_print, "     Sunday     ");
            break;
        default:
        sprintf(array_print, "     Error!     ");
            break;
        }

        for(int i = 0; i < 16; i++)
        {
            i2c_ssd1306_print_something(SSD1306_ADDR, &ASCII_normal_8x8_bitmap[array_print[i]-ASCII_PRINT_ABLE_OFFSET][0],ASCII_NORMAL_8x8_BMP_SIZE);
        }

        // toggle
        
        if(contr == 150) 
        {
            contr = 0;
            tg_display=1-tg_display;
            i2c_ssd1306_reverse_light_display(SSD1306_ADDR,tg_display);
        }

        i2c_ssd1306_setup_contrast(SSD1306_ADDR,contr++);

    }
}
