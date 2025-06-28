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
#include "i2c_ssd1306.h"

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



// --------------------Set up address write data functions----------------------
// choose addressing mode
uint8_t i2c_ssd1306_choose_addressing_mode(uint8_t addr, uint8_t addr_mode)
{
    i2c_master_list_t addr_set_mode;

    addr_set_mode.list_size = 4; // control, command, on off
    addr_set_mode.list_ptr = (uint8_t*)calloc(4,sizeof(uint8_t));

    // if failed
    if(addr_set_mode.list_ptr == NULL)
    {
        return 1;
    }

    // else package command
    addr_set_mode.list_ptr[0] = I2C_SSD1306_SEND_COMMAND;
    if(addr_mode == I2C_SSD1306_HOR_ADDR_MODE)
    {
        addr_set_mode.list_ptr[1] = I2C_SSD1306_CHOOSE_ADDR_MODE;

        addr_set_mode.list_ptr[2] = I2C_SSD1306_SEND_COMMAND;
        addr_set_mode.list_ptr[3] = I2C_SSD1306_HOR_ADDR_MODE;
    }
    else if(addr_mode == I2C_SSD1306_VER_ADDR_MODE)
    {
        addr_set_mode.list_ptr[1] = I2C_SSD1306_CHOOSE_ADDR_MODE;

        addr_set_mode.list_ptr[2] = I2C_SSD1306_SEND_COMMAND;
        addr_set_mode.list_ptr[3] = I2C_SSD1306_VER_ADDR_MODE;
    }
    else if(addr_mode == I2C_SSD1306_PAGE_ADDR_MODE)
    {
        addr_set_mode.list_ptr[1] = I2C_SSD1306_CHOOSE_ADDR_MODE;

        addr_set_mode.list_ptr[2] = I2C_SSD1306_SEND_COMMAND;
        addr_set_mode.list_ptr[3] = I2C_SSD1306_PAGE_ADDR_MODE;
    }
    else// user input something wrong :)
    {
        free(addr_set_mode.list_ptr);
        return 2;
    }

    // send command
    if(i2c_master_send_command_to_7bit_addr(addr, addr_set_mode, I2C_SSD1306_SEND_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(addr_set_mode.list_ptr);
        return 3;
    }
    
    // ok
    free(addr_set_mode.list_ptr);
    return 0;
}

// Page addressing mode - Choose page
uint8_t i2c_ssd1306_page_addr_choose_page(uint8_t addr, uint8_t page_num)
{
    i2c_master_list_t set_page;

    set_page.list_size = 2; // control, command, on off
    set_page.list_ptr = (uint8_t*)calloc(2,sizeof(uint8_t));

    // if failed
    if(set_page.list_ptr == NULL)
    {
        return 1;
    }

    // else package command
    set_page.list_ptr[0] = I2C_SSD1306_SEND_COMMAND;
    if(page_num <= I2C_SSD1306_LAST_PAGE_INDEX)
    {
        set_page.list_ptr[1] = I2C_SSD1306_PAGE_CHOOSE_PAGE | page_num;
    }
    else// user input something wrong :)
    {
        free(set_page.list_ptr);
        return 2;
    }

    // send command
    if(i2c_master_send_command_to_7bit_addr(addr, set_page, I2C_SSD1306_SEND_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(set_page.list_ptr);
        return 3;
    }
    
    // ok
    free(set_page.list_ptr);
    return 0;
}

// Page addressing mode - Choose start segment
uint8_t i2c_ssd1306_page_addr_choose_start_segment(uint8_t addr, uint8_t segment_num)
{
    i2c_master_list_t set_seg;

    set_seg.list_size = 4; // control, command, on off
    set_seg.list_ptr = (uint8_t*)calloc(4,sizeof(uint8_t));

    // if failed
    if(set_seg.list_ptr == NULL)
    {
        return 1;
    }

    // else package command
    set_seg.list_ptr[0] = I2C_SSD1306_SEND_COMMAND;
    if(segment_num <= I2C_SSD1306_LAST_SEGMENT_INDEX)
    {
        set_seg.list_ptr[1] = I2C_SSD1306_PAGE_CHOOSE_SEG_L | (segment_num & 0x0F);

        set_seg.list_ptr[2] = I2C_SSD1306_SEND_COMMAND;
        set_seg.list_ptr[3] = I2C_SSD1306_PAGE_CHOOSE_SEG_H | (segment_num >> 4);
    }
    else// user input something wrong :)
    {
        free(set_seg.list_ptr);
        return 2;
    }

    // send command
    if(i2c_master_send_command_to_7bit_addr(addr, set_seg, I2C_SSD1306_SEND_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(set_seg.list_ptr);
        return 3;
    }
    
    // ok
    free(set_seg.list_ptr);
    return 0;
}

// // Horizontal/Vertical addressing mode - choose group page
// uint8_t i2c_ssd1306_hv_addr_choose_group_page(uint8_t addr, uint8_t page_start, uint8_t page_stop)
// {

// }

// // Horizontal/Vertical addressing mode - choose start stop segment
// uint8_t i2c_ssd1306_hv_addr_choose_group_segment(uint8_t addr, uint8_t segment_start,uint8_t segment_end)
// {

// }

// --------------------Write data functions----------------------

// print some thing
uint8_t i2c_ssd1306_print_something(uint8_t addr, const uint8_t* content, u_int32_t size_content)
{
    i2c_master_list_t set_content;

    set_content.list_size = size_content+1; // control, command, on off
    set_content.list_ptr = (uint8_t*)calloc(set_content.list_size, sizeof(uint8_t));

    // if failed
    if(set_content.list_ptr == NULL)
    {
        return 1;
    }

    // package command
    // add control
    set_content.list_ptr[0] = I2C_SSD1306_SEND_ONLY_DATA;

    // memcpy(dest, src, size)
    memcpy(&(set_content.list_ptr[1]), content, size_content);

    // send command
    if(i2c_master_send_command_to_7bit_addr(addr, set_content, I2C_SSD1306_SEND_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(set_content.list_ptr);
        return 2;
    }
    
    // ok
    free(set_content.list_ptr);
    return 0;
}

// clear screen
uint8_t i2c_ssd1306_clear_screen(uint8_t addr, uint8_t addr_mode)
{
    uint8_t* empty_CGRAM = NULL;

    // create 0 data write to CGRAM
    empty_CGRAM = calloc(1024, sizeof(uint8_t)); // 128seg * 8page
    if(empty_CGRAM == NULL)
    {
        return 1;
    }

    // horizontal mode
    if(i2c_ssd1306_choose_addressing_mode(addr, I2C_SSD1306_HOR_ADDR_MODE))
    {
        free(empty_CGRAM);
        return 2;
    }

    // print
    if(i2c_ssd1306_print_something(addr, empty_CGRAM, 1024))
    {
        free(empty_CGRAM);
        return 3;
    }

    // free data
    free(empty_CGRAM);

    // recover old mode
    if(i2c_ssd1306_choose_addressing_mode(addr, addr_mode))
    {
        return 4;
    }

    // ok
    return 0;
}


// convert to ASCII and print
uint8_t i2c_ssd1306_convert_and_print_ASCII_bitmap(uint8_t addr,const void* raw_content, uint8_t type_data)
{
    uint32_t size_content = 0;

    // input NULL
    if(raw_content == NULL)
    {return 1;}
    
    // convert with type was definited
    switch (type_data)
    {
    case I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING:
        {
            char* raw_content_ptr = (char*)raw_content;
            size_content = strlen(raw_content_ptr);
            for(int i = 0; i< size_content; i++)
            {
                if(raw_content_ptr[i] >= 0x20 && raw_content_ptr[i] <= 0x7E)
                {
                    if(i2c_ssd1306_print_something(addr, &ASCII_normal_8x8_bitmap[raw_content_ptr[i]-ASCII_PRINT_ABLE_OFFSET][0],ASCII_NORMAL_8x8_BMP_SIZE))
                    {
                        return 2; // can't send data
                    }
                }
                // otherwise will be ignore
            }
        }
        break;
    case I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32:
    case I2C_SSD1306_DATA_TYPE_TO_PRINT_FLOAT:
        {
            // float maximum 3.40282347 × 10^38 ~ 1 sign + 38 integer + 1 dot + 6 frac ~ need round about 50
            char* content_8bit = (char*)calloc(50, sizeof(char)); // create array to keep 8bit data after convert
            if(content_8bit == NULL)
            {return 3;}

            // start convert
            if (type_data == I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32)
            { 
                if (sprintf(content_8bit, "%d", *(int*)raw_content)<0)
                {
                    free(content_8bit);
                    return 4; // sprintf return negative number unkown error
                }
            }
            else
            {
                if (sprintf(content_8bit, "%.6f", *(float*)raw_content)<0)
                {
                    free(content_8bit);
                    return 4; // sprintf return negative number unkown error
                }
            }

            size_content = strlen(content_8bit);
            for(int i = 0; i< size_content; i++)
            {
                if(i2c_ssd1306_print_something(addr, &ASCII_normal_8x8_bitmap[content_8bit[i]-ASCII_PRINT_ABLE_OFFSET][0],ASCII_NORMAL_8x8_BMP_SIZE))
                {
                    free(content_8bit);
                    return 5; // can't send data
                }
            }

            //free
            free(content_8bit);
        }
        break;
    default:
        // :) input unknown type
        return 6; 
    }

    // ok
    return 0;
}