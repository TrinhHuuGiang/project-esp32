/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/



// note that the spi module must config before using this library


// modify comming soon , chip select now control by 74hc595
// modify max defaut frequency by 4MHz


// this spi module now depend comunicate on spi chanel and chip select
//  depend on 74hc595 series
//  
// Problem: the same problem like 74hc4067, when multi spi device using
//      after set chip select, then go to comunicate with spi, 
//      another spi task maybe change chip select channel
//      
//      - but quite different when 74hc4067 is only one object control select channel,
//          when spi can be (set, unset) cs by many device
//
// Solution: a mutex chip select mutex, supply at "74hc595.h"
//      - 

#ifndef _SPI_SD_CARD_H_
#define _SPI_SD_CARD_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
// c
#include <stdint.h>
#include <stdlib.h>

// esp
#include "esp_vfs_fat.h"
#include "esp_err.h"

//  defined
#include "spi_master_handle.h"
#include "_peripherals_err.h"

#include "gpio_setup_handle.h"
#include "gpio_74hc595.h"


// define charateristic
#define SPI_SD_CARD_HOST_NUM              SPI_MASTER_PORT // using spi master host follow "spi_master_handle.h"

#define SPI_SD_CARD_PIN_CS                (3)         // default using GPIO5 (datasheet is VSPI CS)
                    // because my board use 595 to control sdcard             
                    // and esp_vfs_fat.h need a cs pin
                    // i will fake CS by this GPIO3 (UART RX) but not use this
                    //      i will reset pin to origin in "spi_sd_card_mount_vfs_with_fatfs_on_card"
                    //      to disconnect it with UART0 RX before use this library


#define SPI_SD_CARD_ROOT_PATH_STRING      "/sd_card0" // name represent for sdcard mounted

#define SPI_SD_CARD_MAX_FILE_OPEN         (10)        // Not find any document say about limit :) i think 10 is good limit
                                                // until has problem


// #if SPI_MASTER_CLOCK_FREQ_10M <= SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX      // set limit clock for sd_card spi interface . max 10M
//     #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_10M
// #elif SPI_MASTER_CLOCK_FREQ_9M <= SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX
//     #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_9M
// #elif SPI_MASTER_CLOCK_FREQ_8M <= SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX
//     #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_8M

// #if SPI_MASTER_CLOCK_FREQ_4M <= SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX // max 4MHz
//     #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_4M
// #else
    #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_1M
// #endif

/**
 * **********************************************************
 * Sync 
 * (never use these mutex if call any APIs below, it included)
 * (only use when other device is conflicted with this peripheral)
 * 
 * there sync reconfigured to suit the circuit. set command can be removed for pure spi purposes
 * 
 * Note: 
 *  - before and after start a transaction handle file, we need get and release mutex by hand
 *      because can't modify POSIX or C style libs
 * 
 *  - hardware design circuit
 *       important: Always switch spi channel by switch chip select by 595 series
 *          
 *           +     // setup cs after give sema (set low)
 *                  if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0))
 * 
 *           +     // unsetup cs before release sema (set high)
 *                  if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))
 * *********************************************************
 * */

// take and release chip select mutex at "74hc595.h"

    // an example transaction chip select for spi sdcard
        // 1. take mutex before chipselect session
        // take_cs_spi_mutex();
        
        // 2. set chip select
        // if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0))

        // 3. do something

        // 4. unset chip select
        // if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))

        // 5. release mutex after unset chip select
        // release periph mutex


/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

//============================== Config and connect to sd card  ==============================
//============================== ============================== ==============================

/**
 * @brief Step1: mount sd with vfs
 * 
 * @return 0 ok, !0 fail
 */
uint8_t spi_sd_card_mount_vfs_with_fatfs_on_card(sdmmc_card_t** out_card);

//============================== READ WRITE by C style lib      ==============================
//============================== ============================== ==============================


/**
 * @brief Step2: using <stdio> and posix to read, write to card
 */

// Before always:
                // take_cs_mutex
                // set_
// interract by C (stdio.h) and POSIX

// After always: 
                // unset_
                // release_cs_mutex


    // an example transaction chip select for spi sdcard
        // 1. take mutex before chipselect session
        // take_cs_spi_mutex();
        
        // 2. set chip select
        // if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 0))

        // 3. do something

        // 4. unset chip select
        // if(gpio_74HC595_set_output_bit(1<<GPIO_74HC595_OUTPUT_SPI_SDCS, 1<<GPIO_74HC595_OUTPUT_SPI_SDCS))

        // 5. release mutex after unset chip select
        // release periph mutex


//============================== Disconnect and clean========== ==============================
//============================== ============================== ==============================
/**
 * @brief Step3: unmount card
 * 
 * @return 0 ok, !0 fail
 * 
 */
uint8_t spi_sd_card_un_mount_vfs_with_fatfs_on_card(sdmmc_card_t *card);


#endif