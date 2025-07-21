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

// ssd1306 library for Oled 128x64
// address available: 0x3C, 0x3D
#ifndef _I2C_SSD1306_H_
#define _I2C_SSD1306_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "i2c_master_handle.h"
#include "i2c_ssd1306_bitmap.h"


// choose addressing mode
#define I2C_SSD1306_HOR_ADDR_MODE   (0x00)
#define I2C_SSD1306_VER_ADDR_MODE   (0x01)
#define I2C_SSD1306_PAGE_ADDR_MODE  (0x02)

// time out send data
#define I2C_SSD1306_SEND_DATA_TIMEOUT (500) // require 100 ms ~1/100k * 128 x 64 bit / 8 * (8+1) = 92.16ms
                                            // double require time :) this time suitable for case send max data ~ 1 frame 128x64
                                            // 500 is fine for all case

// Control command
#define I2C_SSD1306_SEND_COMMAND      (0x80) // Case Co = 1, D/C = 0 -> send command
#define I2C_SSD1306_SEND_DATA         (0xC0) // Case Co = 1, D/C = 1 -> send data (sometime mix command)

#define I2C_SSD1306_SEND_ONLY_DATA    (0x40)     // Case Co = 0, D/C = 1 (0 don't care)-> send data

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

// Addressing mode
#define I2C_SSD1306_CHOOSE_ADDR_MODE   (0x20)

#define I2C_SSD1306_LAST_PAGE_INDEX    (7U)
#define I2C_SSD1306_LAST_SEGMENT_INDEX (127U)

#define I2C_SSD1306_PAGE_CHOOSE_PAGE   (0xB0) // or this command with PAGE bit

#define I2C_SSD1306_PAGE_CHOOSE_SEG_L  (0x00) // or this command with low bit segment
#define I2C_SSD1306_PAGE_CHOOSE_SEG_H  (0x10) // or this command with high bit segment

// Data type
#define I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING   (1)
#define I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32    (2)
#define I2C_SSD1306_DATA_TYPE_TO_PRINT_FLOAT    (3)

// convenience string
#define I2C_SSD1306_EMPTY_PAGE "                "

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */
//============================== Basic functions ============== ==============================
//============================== ============================== ==============================

/**
 * @brief Set up contrast level
 * 
 * @param level input 0-255 to set contrast level
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_setup_contrast(uint8_t addr, uint8_t level);

/**
 * @brief Turn on all led
 * 
 * @param on_off input 0 to normal display, input 1 to on all led
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_turn_on_all_led(uint8_t addr, uint8_t on_off);


/**
 * @brief Reverse light display
 * 
 * @param on_off input 0 to normal display, input 1 to reverse light
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_reverse_light_display(uint8_t addr, uint8_t on_off);

/**
 * @brief On off screen
 * 
 * @param on_off input 0 to off screen, input 1 to continue display
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_on_off_screen(uint8_t addr, uint8_t on_off);





//============================== Set up address write data functions =========================
//============================== ============================== ==============================
/**
 * @brief Choose addressing mode
 * 
 * @param addr_mode input addressing mode
 * 
 * @note choose mode by some macro `choose addressing mode` above
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_choose_addressing_mode(uint8_t addr, uint8_t addr_mode);

/**
 * @brief choose page for Page Addressing mode
 * 
 * @param page_num ssd1306 support Page Addressing mode, only display 1 page you was chosen
 * 
 * @note input 0 - 7 to choose Page display, content auto play throughout only this page
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_page_addr_choose_page(uint8_t addr, uint8_t page_num);

/**
 * @brief choose start segment for Page Addressing mode
 * 
 * @param segment_num ssd1306 support 128 segment (input 0 - 127)
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_page_addr_choose_start_segment(uint8_t addr, uint8_t segment_num);

/**
 * @brief chose page start and stop for Only Horizontal/Vertical mode
 * 
 * @note input page start (0-7) and page stop (0-7)
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_hv_addr_choose_group_page(uint8_t addr, uint8_t page_start, uint8_t page_stop);

/**
 * @brief chose start and stop segment for Only Horizontal/Vertical mode
 * 
 * @note input segment start (0-127) and page stop (0-127)
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_hv_addr_choose_group_segment(uint8_t addr, uint8_t segment_start,uint8_t segment_end);





//============================== Write data functions ========= ==============================
//============================== ============================== ==============================

/**
 * @brief Send array segment 8bit to print
 * 
 * @note After set up where to write data, call this function and enter array 8bit data were set
 * @note This function copy array of content to maximun 1 image 128x64 = 1KB will create on RAM
 * @note sure that dont send any thing too big, and remember I2C_SSD1306_SEND_DATA_TIMEOUT is 
 * @note calculated for 1 image 128x64
 * 
 * @param content pointer to array 8bit data you want to print
 * @param size_content size of array uint8_t
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_print_something(uint8_t addr, const uint8_t* content, uint32_t size_content);

/**
 * @brief Send empty segment to clean CGRAM
 * 
 * @param addr_mode clean will change to horizontal mode. Sure enter addr_mode to recover old mode
 * 
 * @return 0 ok, !0 fail
 */
uint8_t i2c_ssd1306_clear_screen(uint8_t addr, uint8_t addr_mode);

/**
 * @brief Auto format content to ASCII bitmap and send to display
 * 
 * @note After setting up where to write data, call this function.
 * @note Input data types must follow those defined in the macro section.
 * @note Any ASCII >= 0x7F and UTF-8 >= 0x7F will be ignored.
 * @note Float values are automatically rounded to 6 decimal places.
 * @warning Warning: This function assumes the input data type is int, float, or string.
 * @warning If you pass a pointer to uint16_t or a smaller type, the function will expand and read all 32 bits (including 16 bits of garbage).
 * @warning This may cause unexpected behavior if the input type is incorrect.
 * @warning Always ensure that the input is int, float, or string.
 * 
 * @param content Pointer to data
 * @param type_of_data Cast type of data; currently supports string, int32, and float.
 * 
 * @return 0 if successful, non-zero if failed.
 */
uint8_t i2c_ssd1306_convert_and_print_ASCII_bitmap(uint8_t addr, const void* content, uint8_t type_data);

#endif