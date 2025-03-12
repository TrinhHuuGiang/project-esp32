/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "i2c_ssd1306.h"

// Control command
#define I2C_SSD1306_SEND_COMMAND      (0x80) // Case Co = 1, D/C = 0 -> send command
#define I2C_SSD1306_SEND_DATA         (0xC0) // Case Co = 1, D/C = 1 -> send data (sometime mix command)

#define I2C_SSD1306_SEND_ONLY_DATA    (0x00)     // Case Co = 0, D/C = don't care -> send data

// Basic command
#define I2C_SSD1306_SET_CONTRAST      (0x81)
#define I2C_SSD1306_ON_ALL_LED        (0xA5)
#define I2C_SSD1306_OFF_ALL_LED       (0xA4)
#define I2C_SSD1306_ON_REVERSE_LIGHT  (0xA7)
#define I2C_SSD1306_OFF_REVERSE_LIGHT (0xA6)

#define I2C_SSD1306_SETTING_CHARGE_PUMP (0x8D)
#define I2C_SSD1306_EN_CHARGE_PUMP    (0x14)
#define I2C_SSD1306_ON_SCREEN         (0xAF)
#define I2C_SSD1306_OFF_SCREEN        (0xAE)
#define I2C_SSD1306_DIS_CHARGE_PUMP   (0x10)


// 

 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// --------------------Basic functions----------------------

// set contrast
uint8_t i2c_ssd1306_setup_contrast(uint8_t addr, uint8_t level)
{
    i2c_master_list_t set_contrast;

    set_contrast.list_size = 4; // control, contrast command, level contrast
    set_contrast.list_ptr = (uint8_t*)calloc(4,sizeof(uint8_t));

    // if failed
    if(set_contrast.list_ptr == NULL)
    {
        return 1;
    }

    // else package command
    set_contrast.list_ptr[0] = I2C_SSD1306_SEND_COMMAND;
    set_contrast.list_ptr[1] = I2C_SSD1306_SET_CONTRAST;
    set_contrast.list_ptr[2] = I2C_SSD1306_SEND_COMMAND;
    set_contrast.list_ptr[3] = level;

    // send command
    if(i2c_master_send_command_to_7bit_addr(addr, set_contrast, I2C_SSD1306_SEND_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(set_contrast.list_ptr);
        return 2;
    }
    
    // ok
    free(set_contrast.list_ptr);
    return 0;
}

// turn all led
uint8_t i2c_ssd1306_turn_on_all_led(uint8_t addr, uint8_t on_off)
{
    i2c_master_list_t on_all_led;

    on_all_led.list_size = 2; // control, command, on off
    on_all_led.list_ptr = (uint8_t*)calloc(2,sizeof(uint8_t));

    // if failed
    if(on_all_led.list_ptr == NULL)
    {
        return 1;
    }

    // else package command
    on_all_led.list_ptr[0] = I2C_SSD1306_SEND_COMMAND;
    if(on_off == 1)
    {on_all_led.list_ptr[1] = I2C_SSD1306_ON_ALL_LED;}
    else if(on_off == 0)
    {on_all_led.list_ptr[1] = I2C_SSD1306_OFF_ALL_LED;}
    else// user input something wrong :)
    {
        free(on_all_led.list_ptr);
        return 2;
    }

    // send command
    if(i2c_master_send_command_to_7bit_addr(addr, on_all_led, I2C_SSD1306_SEND_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(on_all_led.list_ptr);
        return 3;
    }
    
    // ok
    free(on_all_led.list_ptr);
    return 0;
}


// reverse light
uint8_t i2c_ssd1306_reverse_light_display(uint8_t addr, uint8_t on_off)
{
    i2c_master_list_t reverse_light;

    reverse_light.list_size = 2; // control, command, on off
    reverse_light.list_ptr = (uint8_t*)calloc(2,sizeof(uint8_t));

    // if failed
    if(reverse_light.list_ptr == NULL)
    {
        return 1;
    }

    // else package command
    reverse_light.list_ptr[0] = I2C_SSD1306_SEND_COMMAND;
    if(on_off == 1)
    {reverse_light.list_ptr[1] = I2C_SSD1306_ON_REVERSE_LIGHT;}
    else if(on_off == 0)
    {reverse_light.list_ptr[1] = I2C_SSD1306_OFF_REVERSE_LIGHT;}
    else// user input something wrong :)
    {
        free(reverse_light.list_ptr);
        return 2;
    }

    // send command
    if(i2c_master_send_command_to_7bit_addr(addr, reverse_light, I2C_SSD1306_SEND_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(reverse_light.list_ptr);
        return 3;
    }
    
    // ok
    free(reverse_light.list_ptr);
    return 0;
}

// on off screen
uint8_t i2c_ssd1306_on_off_screen(uint8_t addr, uint8_t on_off)
{
    i2c_master_list_t on_off_screen;

    on_off_screen.list_size = 6; // control, command, on off
    on_off_screen.list_ptr = (uint8_t*)calloc(6,sizeof(uint8_t));

    // if failed
    if(on_off_screen.list_ptr == NULL)
    {
        return 1;
    }

    // else package command
    on_off_screen.list_ptr[0] = I2C_SSD1306_SEND_COMMAND;
    if(on_off == 1)
    {
        on_off_screen.list_ptr[1] = I2C_SSD1306_SETTING_CHARGE_PUMP;

        on_off_screen.list_ptr[2] = I2C_SSD1306_SEND_COMMAND;
        on_off_screen.list_ptr[3] = I2C_SSD1306_EN_CHARGE_PUMP;

        on_off_screen.list_ptr[4] = I2C_SSD1306_SEND_COMMAND;
        on_off_screen.list_ptr[5] = I2C_SSD1306_ON_SCREEN;
    }
    else if(on_off == 0)
    {
        on_off_screen.list_ptr[1] = I2C_SSD1306_OFF_SCREEN;

        on_off_screen.list_ptr[2] = I2C_SSD1306_SEND_COMMAND;
        on_off_screen.list_ptr[3] = I2C_SSD1306_SETTING_CHARGE_PUMP;

        on_off_screen.list_ptr[4] = I2C_SSD1306_SEND_COMMAND;
        on_off_screen.list_ptr[5] = I2C_SSD1306_DIS_CHARGE_PUMP;
    }
    else// user input something wrong :)
    {
        free(on_off_screen.list_ptr);
        return 2;
    }

    // send command
    if(i2c_master_send_command_to_7bit_addr(addr, on_off_screen, I2C_SSD1306_SEND_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(on_off_screen.list_ptr);
        return 3;
    }
    
    // ok
    free(on_off_screen.list_ptr);
    return 0;
}



// // --------------------Set up address write data functions----------------------
// // choose addressing mode
// uint8_t i2c_ssd1306_choose_addressing_mode(uint8_t addr, uint8_t addr_mode)
// {

// }

// // Page addressing mode - Choose page
// uint8_t i2c_ssd1306_page_addr_choose_page(uint8_t addr, uint8_t page_num)
// {

// }

// // Page addressing mode - Choose start segment
// uint8_t i2c_ssd1306_page_addr_choose_start_segment(uint8_t addr, uint8_t segment_num)
// {

// }

// // Horizontal/Vertical addressing mode - choose group page
// uint8_t i2c_ssd1306_hv_addr_choose_group_page(uint8_t addr, uint8_t page_start, uint8_t page_stop)
// {

// }

// // Horizontal/Vertical addressing mode - choose start stop segment
// uint8_t i2c_ssd1306_hv_addr_choose_group_segment(uint8_t addr, uint8_t segment_start,uint8_t segment_end)
// {

// }

// // --------------------Write data functions----------------------

// // print some thing
// uint8_t i2c_ssd1306_print_something(uint8_t addr, const char* content)
// {

// }