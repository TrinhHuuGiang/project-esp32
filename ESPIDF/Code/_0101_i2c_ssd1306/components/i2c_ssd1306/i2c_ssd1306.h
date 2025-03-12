#ifndef _I2C_SSD1306_H_
#define _I2C_SSD1306_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include <stdlib.h>

#include "i2c_master_handle.h"
#include "i2c_ssd1306_bitmap.h"


// choose addressing mode
#define I2C_SSD1306_HOR_ADDR_MODE   (0x00)
#define I2C_SSD1306_VER_ADDR_MODE   (0x01)
#define I2C_SSD1306_PAGE_ADDR_MODE  (0x02)

// time out send data
#define I2C_SSD1306_SEND_DATA_TIMEOUT (100) // 100 ms ~1/100k * 128 x 64 bit / 8 * (8+1) = 92.16ms

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

// --------------------Basic functions----------------------

/**
 * @brief Set up contrast level
 * 
 * @param level input 0-255 to set contrast level
 * 
 */
uint8_t i2c_ssd1306_setup_contrast(uint8_t addr, uint8_t level);

/**
 * @brief Turn on all led
 * 
 * @param on_off input 0 to normal display, input 1 to on all led
 * 
 */
uint8_t i2c_ssd1306_turn_on_all_led(uint8_t addr, uint8_t on_off);


/**
 * @brief Reverse light display
 * 
 * @param on_off input 0 to normal display, input 1 to reverse light
 */
uint8_t i2c_ssd1306_reverse_light_display(uint8_t addr, uint8_t on_off);

/**
 * @brief On off screen
 * 
 * @param on_off input 0 to off screen, input 1 to continue display
 */
uint8_t i2c_ssd1306_on_off_screen(uint8_t addr, uint8_t on_off);



// --------------------Set up address write data functions----------------------
/**
 * @brief Choose addressing mode
 * 
 * @param addr_mode input addressing mode
 * 
 * @note choose mode by some macro `choose addressing mode` above
 */
uint8_t i2c_ssd1306_choose_addressing_mode(uint8_t addr, uint8_t addr_mode);

/**
 * @brief choose page for Page Addressing mode
 * 
 * @param page_num ssd1306 support Page Addressing mode, only display 1 page you was chosen
 * 
 * @note input 0 - 7 to choose Page display, content auto play throughout only this page
 */
uint8_t i2c_ssd1306_page_addr_choose_page(uint8_t addr, uint8_t page_num);

/**
 * @brief choose start segment for Page Addressing mode
 * 
 * @param segment_num ssd1306 support 128 segment (input 0 - 127)
 */
uint8_t i2c_ssd1306_page_addr_choose_start_segment(uint8_t addr, uint8_t segment_num);

/**
 * @brief chose page start and stop for Only Horizontal/Vertical mode
 * 
 * @note input page start (0-7) and page stop (0-7)
 */
uint8_t i2c_ssd1306_hv_addr_choose_group_page(uint8_t addr, uint8_t page_start, uint8_t page_stop);

/**
 * @brief chose start and stop segment for Only Horizontal/Vertical mode
 * 
 * @note input segment start (0-127) and page stop (0-127)
 */
uint8_t i2c_ssd1306_hv_addr_choose_group_segment(uint8_t addr, uint8_t segment_start,uint8_t segment_end);

// --------------------Write data functions----------------------

/**
 * @brief Send a message to print
 * 
 * @note After set up where to write data, call this function and enter a string will be printed
 * 
 * @param content pointer to string you want to print
 */
uint8_t i2c_ssd1306_print_something(uint8_t addr, const char* content);

#endif