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

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "spi_sd_mount_to_fs.h"

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */


//============================== ============================== ==============================
//============================== ============================== ==============================
//Step1: mount sd with vfs
uint8_t spi_sd_card_mount_vfs_with_fatfs_on_card(sdmmc_card_t** out_card)
{
    // slave config
    sdmmc_host_t host_config_input = SDSPI_HOST_DEFAULT();
    host_config_input.slot = SPI_SD_CARD_HOST_NUM;  // port
    host_config_input.max_freq_khz = SPI_SD_CARD_CLOCK_SPEED / 1000 ; // input unit by kHz , SPI_SD_CARD_CLOCK_SPEED is MHz
    host_config_input.flags = SDMMC_HOST_FLAG_SPI;  // spi sdmmc mode

    // cs pin config
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.host_id = host_config_input.slot;
    slot_config.gpio_cs = SPI_SD_CARD_PIN_CS; // chip select pin

    // mount config
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = false, // do not format if mount failed
        .max_files = SPI_SD_CARD_MAX_FILE_OPEN, // max file open
        .allocation_unit_size = 0   // default input 0 is alloc 1 sector (512B)
                                    // suggest: 4KB for 4GB, 8KB for 8GB,...
                                    // .allocation_unit_size alse called cluster size.
                                    // when the file <= cluster size it still waste cluster size
                                    // like 1KB == 32KB cluster size if cluster = 32KB
                                    // Cluster must divisible with a sector

                                    // if only read, write but not format, don't care `allocation_unit_size`
    };

    esp_err_t ret = esp_vfs_fat_sdspi_mount(SPI_SD_CARD_ROOT_PATH_STRING, &host_config_input, &slot_config,
    &mount_config, out_card);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_SD_CARD_MOUNT_TO_CARD_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return 1;
    }

    // ok
    return 0;
}

//============================== ============================== ==============================
//============================== ============================== ==============================


//Step2: using <stdio> and posix to read, write to card



//============================== Disconnect and clean========== ==============================
//============================== ============================== ==============================
//Step3: unmount card
uint8_t spi_sd_card_un_mount_vfs_with_fatfs_on_card(sdmmc_card_t *card)
{
    esp_err_t ret = esp_vfs_fat_sdcard_unmount(SPI_SD_CARD_ROOT_PATH_STRING,card);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_SD_CARD_UN_MOUNT_CARD_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return 1;
    }

    // ok
    return 0;
}
