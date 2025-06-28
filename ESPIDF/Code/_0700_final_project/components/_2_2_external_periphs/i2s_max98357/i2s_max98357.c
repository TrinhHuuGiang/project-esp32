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

#include "i2s_max98357.h"


 /**
 * **********************************************************
 * Prototype
 * **********************************************************
 */

 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// turn on by set gpio SD to HIGH
uint8_t i2s_max98357_turn_on()
{
    // reset pin
    if(gpio_setup_reset_pin_to_origin(I2S_MAX98357_SD_PIN)) return 1;

    // no pull, set ouput
    if(gpio_setup_io_direction(I2S_MAX98357_SD_PIN, GPIO_MODE_OUTPUT)) return 2; 

    // set logic output
    if(gpio_setup_output_logic_level(I2S_MAX98357_SD_PIN, 1)) return 3;

    return 0;
}


// turn on by set gpio SD to LOW
uint8_t i2s_max98357_turn_off()
{
    // reset pin <- need it if turn off call first after reset
    if(gpio_setup_reset_pin_to_origin(I2S_MAX98357_SD_PIN)) return 1;

    // no pull, set ouput
    if(gpio_setup_io_direction(I2S_MAX98357_SD_PIN, GPIO_MODE_OUTPUT)) return 2; 

    // set logic output is low
    if(gpio_setup_output_logic_level(I2S_MAX98357_SD_PIN, 0)) return 3;

    return 0;
}