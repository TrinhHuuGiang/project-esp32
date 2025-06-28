/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// add function on off speaker
// depend on "i2s_master_output.h"
// this library expand API help I2S - Inter-Integrated Circuit Sound
//      added some functions to enable and disable the i2s max98357 module

// control only by "widDr_wav_player.h" otherwise not thread safe

#ifndef _I2S_MAX98357_H_
#define _I2S_MAX98357_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
//c
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

// user define
#include "gpio_setup_handle.h"

// config hardware
#define I2S_MAX98357_SD_PIN   CONFIG_I2S_MAX98357_SD_PIN // SHUT DOWN PIN - GPIO17

 /**
 * **********************************************************
 * APIs
 * **********************************************************
 */
// turn on by set gpio SD to HIGH
uint8_t i2s_max98357_turn_on();


// turn on by set gpio SD to LOW
uint8_t i2s_max98357_turn_off();


// on / off i2s max98357





#endif