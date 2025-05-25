// note that the spi module must config before using this library
// this lib will interact with sd card and using pin 5 as default CS

// modify comming soon , chip select now control by 74hc595

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


// define charateristic
#define SPI_SD_CARD_HOST_NUM              SPI_MASTER_PORT // using spi master host follow "spi_master_handle.h"

#define SPI_SD_CARD_PIN_CS                (5)         // default using GPIO5 (datasheet is VSPI CS)

#define SPI_SD_CARD_ROOT_PATH_STRING      "/sd_card0" // name represent for sdcard mounted

#define SPI_SD_CARD_MAX_FILE_OPEN         (10)        // Not find any document say about limit :) i think 10 is good limit


#if SPI_MASTER_CLOCK_FREQ_10M <= SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX      // set limit clock for sd_card spi interface . max 10M
    #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_10M
#elif SPI_MASTER_CLOCK_FREQ_9M <= SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX
    #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_9M
#elif SPI_MASTER_CLOCK_FREQ_8M <= SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX
    #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_8M
#elif SPI_MASTER_CLOCK_FREQ_4M <= SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX
    #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_4M
#else
    #define SPI_SD_CARD_CLOCK_SPEED SPI_MASTER_CLOCK_FREQ_1M
#endif





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

//============================== ============================== ==============================
//============================== ============================== ==============================


/**
 * @brief Step2: using <stdio> and posix to read, write to card
 */

// interract by C and POSIX




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