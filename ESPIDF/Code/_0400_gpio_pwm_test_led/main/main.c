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


/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
// i2c
static i2c_config_t* master_conf=NULL;

// gpio
static gpio_74hc165_data_t* s_gpio_74hc165_data = NULL;
static uint32_t old_reg_value = 0;

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
// check input register and update to lcd if input changed
static void update_value_to_lcd(void* arg);

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

void app_main(void)
{
    // lcd init
    i2c_master_init_config(&master_conf);

    i2c_master_setup_hardware(master_conf);

    i2c_master_install_driver(master_conf);
    
    i2c_master_free_config(&master_conf);

    i2c_ssd1306_on_off_screen(SSD1306_ADDR, 1);

    i2c_ssd1306_clear_screen(SSD1306_ADDR, I2C_SSD1306_PAGE_ADDR_MODE);

    // install isr service for GPIO
    if(gpio_setup_install_isr_service_for_gpio_system()) return;

    // install series 74hc165 input service
    if(gpio_74HC165_init_input_gate(&s_gpio_74hc165_data)) return;

    // start capture input event
    if(gpio_74HC165_start_capture_random_events()) return;

    // create task handle print value to lcd
    int ret = 0;

    xTaskCreate(update_value_to_lcd, "update input reg to lcd", 4096, &ret, 5, NULL);

    while(1)
    {
        ESP_LOGI("main loop", "--- wait 1000ms");
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("main loop", "___ done");
    }

    return;
}


static void update_value_to_lcd(void* arg)
{
    int* ret = (int*)arg;

    uint8_t set_state = GPIO_74HC165_NOT_SET_FLAG;

    char* reg1 = (char*)calloc(17, sizeof(char)); // 16 is limit character of page, + 1 for \0
    char* reg2 = (char*)calloc(17, sizeof(char));
    char* reg3 = (char*)calloc(17, sizeof(char));
    char* reg4 = (char*)calloc(17, sizeof(char));

    memset(reg1,' ',16);
    memset(reg2,' ',16);
    memset(reg3,' ',16);
    memset(reg4,' ',16);

    // print init interface on lcd
    i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 0);
    i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);
    i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Q7 of IC1 -----v" , I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);

    i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 2);
    i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);
    i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Q7 of IC2 -----v" , I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);
    
    i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 4);
    i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);
    i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Q7 of IC3 -----v" , I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);
    
    i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 6);
    i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);
    i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Q7 of IC4 -----v" , I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);

    while(!(*ret))
    {
        if(gpio_74HC165_check_flag_and_renew_register(s_gpio_74hc165_data, &set_state)) (*ret) = 1;

        if( (set_state == GPIO_74HC165_SET_FLAG) && (old_reg_value != s_gpio_74hc165_data->reg_state) )
        {
            // update
            old_reg_value = s_gpio_74hc165_data->reg_state;

            // first 74hc165
            for(int i = 0; i<8; i++)
            {
                reg1[15 - i*2] = ((s_gpio_74hc165_data->reg_state >> i) & 1) + '0';
            }
            
            // second
            for(int i = 8; i<16; i++)
            {
                reg2[15 - (i-8)*2] = ((s_gpio_74hc165_data->reg_state >> i) & 1) + '0';
            }     
            // third
            for(int i = 16; i<24; i++)
            {
                reg3[15 - (i-16)*2] = ((s_gpio_74hc165_data->reg_state >> i) & 1) + '0';
            }     
            // fourd
            for(int i = 24; i<32; i++)
            {
                reg4[15 - (i-24)*2] = ((s_gpio_74hc165_data->reg_state >> i) & 1) + '0';
            }     

            // print
            i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 1);
            i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);
            i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, reg1 , I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);


            i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 3);
            i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);
            i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, reg2 , I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);

            i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 5);
            i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);
            i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, reg3 , I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);

            i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 7);
            i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0);
            i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, reg4 , I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);

        }
        // else not set :v continue wait

        //delay
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // exit

    free(reg1);
    free(reg2);
    free(reg3);
    free(reg4);

    vTaskDelete(NULL);
}