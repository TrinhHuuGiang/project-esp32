/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _I2C_SSD1306_BITMAP_H_
#define _I2C_SSD1306_BITMAP_H_

#include <stdint.h>

#define ASCII_NORMAL_8x8_BMP_QUANTITY  (95)
#define ASCII_NORMAL_8x8_BMP_SIZE      (8U)

#define ASCII_PRINT_ABLE_OFFSET (0x20)

// bit map value from 0 -> 94 == ASCII index from 0x20 (32) -> 0x7E (126)
extern const uint8_t ASCII_normal_8x8_bitmap[ASCII_NORMAL_8x8_BMP_QUANTITY][8];

#endif