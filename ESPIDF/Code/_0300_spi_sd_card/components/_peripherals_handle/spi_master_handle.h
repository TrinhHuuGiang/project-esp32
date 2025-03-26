// Note: Communicate with only one device at a time
// Default: using SPI3 (VSPI)
// Use SPI master in a dedicated task to avoid busy waiting; transactions will use polling mode.

#ifndef _SPI_MASTER_HANDLE_H_
#define _SPI_MASTER_HANDLE_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

//c
#include <stdlib.h> // Dynamic allocation
#include <stdint.h> // type int

//esp32
#include "driver/spi_master.h"

#include "esp_heap_caps.h"  // using pvPortMallocCaps(size, MALLOC_CAP_DMA)
                            // to allocate heap size (internal RAM, staring from a 32-bit boundary and 
                            // having a length of multiples of 4 bytes) suitable for DMA using
#include "esp_err.h"


//user
#include "_peripherals_err.h"

// Macro i2c master define
#define SPI_MASTER_PORT      CONFIG_SPI_MASTER_PORT_NUMBER // 1 is SPI2 (HSPI) , default 2 is SPI3 (VSPI)
#define SPI_MASTER_MOSI_PIN  CONFIG_SPI_MASTER_MOSI_PIN
#define SPI_MASTER_MISO_PIN  CONFIG_SPI_MASTER_MISO_PIN
#define SPI_MASTER_SCLK_PIN  CONFIG_SPI_MASTER_SCLK_PIN

#define SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX CONFIG_SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX 

// spi mode 0: (0, 0) ||| 1: (0, 1) |||  2: (1, 0) ||| 3: (1, 1)
#define SPI_MASTER_CLOCK_MODE_0 0
#define SPI_MASTER_CLOCK_MODE_1 1
#define SPI_MASTER_CLOCK_MODE_2 2
#define SPI_MASTER_CLOCK_MODE_3 3

// clock suitable for IO matrix
#define SPI_MASTER_CLOCK_FREQ_1M (APB_CLK_FREQ/80)    ///< 1Mhz
#define SPI_MASTER_CLOCK_FREQ_4M (APB_CLK_FREQ/20)    ///< 4Mhz
#define SPI_MASTER_CLOCK_FREQ_8M SPI_MASTER_FREQ_8M   ///< 8MHz
#define SPI_MASTER_CLOCK_FREQ_9M SPI_MASTER_FREQ_9M   ///< 8.89MHz
#define SPI_MASTER_CLOCK_FREQ_10M SPI_MASTER_FREQ_10M  ///< 10MHz
#define SPI_MASTER_CLOCK_FREQ_11M SPI_MASTER_FREQ_11M  ///< 11.43MHz
#define SPI_MASTER_CLOCK_FREQ_13M SPI_MASTER_FREQ_13M  ///< 13.33MHz
#define SPI_MASTER_CLOCK_FREQ_16M SPI_MASTER_FREQ_16M  ///< 16MHz
#define SPI_MASTER_CLOCK_FREQ_20M SPI_MASTER_FREQ_20M  ///< 20MHz
#define SPI_MASTER_CLOCK_FREQ_26M SPI_MASTER_FREQ_26M  ///< 26.67MHz

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

// =================================== Prepare config, free config ===================================
// =================================== =========================== ===================================

/**
 * @brief Initial spi master config infor
 * 
 * @param master_conf is address of pointer spi_bus_config_t*
 * 
 * @note the pointer `master_conf` should be NULL, then this function will create new memory for it point to
 * 
 * @retval - PERIPH_OK : a pointer to struct spi_bus_config_t
 * @retval - SPI_MASTER_ALLOC_FAILED : if allocate failed
 */
_peripherals_err_t spi_master_init_bus_config( spi_bus_config_t* master_conf);

/**
 * @brief Initial spi device slave config infor
 * 
 * @param device_conf is address of pointer spi_device_interface_config_t*
 * @param command_len set default how many bit using for command, can temporary modify when prepare transaction
 * @param address_len set default how many bit using for address, can temporary modify when prepare transaction
 * @param dummy_len set default how many bit using for dummy, can temporary modify when prepare transaction
 * @param mode choose spi mode macro
 * @param clk_speed choose clock speed macro (Hz)
 * @param CS_pin gpio number. -1 if no use
 * @param cs_ena_pretrans Amount of SPI bit-cycles the cs should be activated before the transmission (0-16). This only works on half-duplex transactions.
 * @param cs_ena_posttrans Amount of SPI bit-cycles the cs should stay active after the transmission (0-16)
 * @param duty_cycle_pos Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
 * @param input_delay_ns delay time between SCLK and MISO for master rececive right data
 * @param flags Normal is 0, but can set specific functions by or bit SPI_DEVICE_*, see: spi_master.h
 * 
 * @note the pointer `device_conf` should be NULL, then this function will create new memory for it point to
 * 
 * @retval - PERIPH_OK : a pointer to struct spi_bus_config_t
 * @retval - SPI_MASTER_ALLOC_FAILED : if allocate failed
 * @retval - SPI_MASTER_DEVICE_CLK_OVER_FREQ : device required frequency > maximum limit
 */
_peripherals_err_t spi_master_init_device_config( spi_device_interface_config_t* device_conf,
    uint8_t command_len, uint8_t address_len, uint8_t dummy_len,
    uint8_t mode, int clk_speed, int CS_pin,
    uint16_t cs_ena_pretrans, uint8_t cs_ena_posttrans, uint16_t duty_cycle_pos,
    int input_delay_ns, uint32_t flags
    );

/**
 * @brief Free bus config after use
 * 
 * @retval - PERIPH_OK
 */
_peripherals_err_t spi_master_free_bus_config( spi_bus_config_t* master_conf);


/**
 * @brief Free device config after use
 * 
 * @retval - PERIPH_OK
 */
_peripherals_err_t spi_master_free_device_config( spi_device_interface_config_t* device_conf);





// =================================== Install & uninstall ===================================
// =================================== =================== ===================================

/**
 * @brief install bus config
 * 
 * @retval - PERIPH_OK
 * @retval - SPI_MASTER_INS_CONFIG_BUS_FAILED
 */
_peripherals_err_t spi_master_install_bus_config(const spi_bus_config_t* master_conf);


/**
 * @brief register device config and get handle
 * 
 * @note device_handle param must be address of pointer to NULL value 'spi_device_handle_t'
 * 
 * @param device_conf pointer to data get from 'spi_master_init_device_config()'
 * @param device_handle add address of pointer spi_device_handle_t NULL value for function fill handle
 * 
 * @retval - PERIPH_OK
 * @retval - SPI_MASTER_REG_DEVICE_CONFIG_FAILED
 */
_peripherals_err_t spi_master_register_device_config(const spi_device_interface_config_t* device_conf, 
    spi_device_handle_t* device_handle);


/**
 * @brief Un-register slave device
 * 
 * @note spi_device_handle_t is pointer to struct spi_device_t
 * @note - [typedef struct spi_device_t *spi_device_handle_t]
 * @note - So now it will point to a un-safe memory.
 * @note - Point it to NULL or no re-use for safe.
 * 
 * @retval - PERIPH_OK
 * @retval - SPI_MASTER_UN_REG_DEVICE_CONFIG_FAILED
 */
_peripherals_err_t spi_master_un_register_device_config(spi_device_handle_t device_handle);


/**
 * @brief Un-install spi bus config (spi master port)
 * 
 * @retval - PERIPH_OK
 * @retval - SPI_MASTER_UN_INS_CONFIG_BUS_FAILED
 */
_peripherals_err_t spi_master_un_install_bus_config();




// =================================== Create Transaction ===================================
// =================================== ================== ===================================

/**
 * @brief prepare transaction struct before start
 * 
 * @param spi_expand_transaction address of pointer 'spi_expand_transaction' to get transaction config
 * @param command_len and address_len, dummy_len is length by bit of these field
 * @param command_data and address_data is data of these 
 * @param tx_data_len and rx_data_len is length of transmit and receive data. rx_data_len always <= tx_data_len
 * 
 * @warning 1. tx_buffer in 'spi_expand_transaction' is a pointer to constant 'buffer'
 * @warning - that mean this function will alloc tx and rx buffer. but tx buffer always block write
 * @warning - suggest: create a pointer to this tx_buffer, then point 'const' tx_pointer to NULL before modify buffer
 * @warning - after that, re-point 'const' tx_pointer to keep save data
 * @warning 2. This function always create at least 2 32bit buffer for tx and rx.
 * 
 * @note - after create 'spi_expand_transaction' , the feild 'base.tx_buffer' must be set data
 * @note by hand equal tx_data_len
 * @note - 'spi_expand_transaction' can reuse normally if specific field not change
 * 
 * @retval - PERIPH_OK
 * @retval - SPI_MASTER_ALLOC_FAILED : if allocate failed
 */
_peripherals_err_t spi_master_prepare_transaction( spi_transaction_ext_t* spi_expand_transaction,
    uint8_t command_len, uint8_t address_len, uint8_t dummy_len,
    uint16_t command_data, uint64_t address_data, 
    size_t tx_data_len_by_bit, size_t rx_data_len_by_bit);



/**
 * @brief after not using transaction template, free it by this function
 * 
 * @retval - PERIPH_OK
 */
_peripherals_err_t spi_master_delete_transaction( spi_transaction_ext_t* spi_expand_transaction);





// =================================== SET CONTENT ===================================
// =================================== BY HAND     ===================================
// after get transaction tempalate, fill data for tx and rx buffer by hand
// the tx buffer pointed by a pointer to const, see warning 'spi_master_prepare_transaction'





// =================================== Transaction ===================================
// =================================== =========== ===================================

/**
 * @brief start polling transaction from master to slave
 * 
 * @param spi_expand_transaction pointer 'spi_expand_transaction', because the field rx_buffer maybe modify
 * @param device_handle handle of device get from 'spi_master_register_device_config()'
 * 
 * @retval - PERIPH_OK
 * @retval - SPI_MASTER_POLLING_TRANSACTION_FAILED
 */
_peripherals_err_t spi_master_polling_transaction(const spi_device_handle_t device_handle ,spi_transaction_ext_t* spi_expand_transaction);



#endif