#ifndef _I2C_SSD1306_BITMAP_H_
#define _I2C_SSD1306_BITMAP_H_

#include <stdint.h>

#define ASCII_NORMAL_8x8_BMP_QUANTITY  (95)
#define ASCII_NORMAL_8x8_BMP_SIZE      (8U)

#define ASCII_PRINT_ABLE_OFFSET (0x20)

// bit map value from 0 -> 94 == ASCII index from 0x20 (32) -> 0x7E (126)
extern const uint8_t ASCII_normal_8x8_bitmap[ASCII_NORMAL_8x8_BMP_QUANTITY][8];


// image user define: ~5KB
// 0. epd_bitmap_bear_cat,
// 1. epd_bitmap_bird,
// 2. epd_bitmap_cat_ai,
// 3. epd_bitmap_dog,
// 4. epd_bitmap_racoon
#define BIT_MAP_IMAGE_128x64_USERDEFINE_QUANTITY (5U)

#define BIT_MAP_IMAGE_128x64_USERDEFINE_SIZE     (1024) //128*64/8 ~ 92.16ms at 100kHz

extern const uint8_t* epd_bitmap_allArray[BIT_MAP_IMAGE_128x64_USERDEFINE_SIZE];

#endif