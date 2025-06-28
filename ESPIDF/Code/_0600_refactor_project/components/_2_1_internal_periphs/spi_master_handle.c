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
#include "spi_master_handle.h"


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// bus config
_peripherals_err_t spi_master_init_bus_config( spi_bus_config_t* master_conf)
{
    spi_bus_config_t** master_conf_addr = (spi_bus_config_t**)master_conf;

    *master_conf_addr = (spi_bus_config_t*)calloc(1, sizeof(spi_bus_config_t));

    if(*master_conf_addr == NULL)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_ALLOC_FAILED, __FILE__, __LINE__, "Alloc failed");
        #endif
        return SPI_MASTER_ALLOC_FAILED;
    }

    //set config

    // basic
    (*master_conf_addr)->mosi_io_num = SPI_MASTER_MOSI_PIN;
    (*master_conf_addr)->miso_io_num = SPI_MASTER_MISO_PIN;
    (*master_conf_addr)->sclk_io_num = SPI_MASTER_SCLK_PIN;

    // unused
    (*master_conf_addr)->quadhd_io_num = -1;
    (*master_conf_addr)->quadwp_io_num = -1;
    (*master_conf_addr)->data4_io_num = -1;
    (*master_conf_addr)->data5_io_num = -1;
    (*master_conf_addr)->data6_io_num = -1;
    (*master_conf_addr)->data7_io_num = -1;

    // special
    (*master_conf_addr)->max_transfer_sz = 0; // if 0 , the buffer size default maximum 4096 when using DMA.
                                              // and be 'SOC_SPI_MAXIMUM_BUFFER_SIZE' if not using DMA.
                                              // buffer size split transaction if it too long
    (*master_conf_addr)->flags =    SPICOMMON_BUSFLAG_MASTER|    // Initialize I/O in master mode
                                    SPICOMMON_BUSFLAG_GPIO_PINS| // Using GPIO matrix, it slower than IOMUX but sure any pin miso,mosi,sck can change by user
                                    SPICOMMON_BUSFLAG_SCLK|      // Check existing of SCLK pin. Or indicates CLK line initialized.
                                    SPICOMMON_BUSFLAG_MISO|      // Check existing of MISO pin. Or indicates MISO line initialized.
                                    SPICOMMON_BUSFLAG_MOSI;      // Check existing of MOSI pin. Or indicates MOSI line initialized.
    (*master_conf_addr)->intr_flags = 0; // using pooling mode so do not use interrupt

    //ok
    return PERIPH_OK;
}

// device config
_peripherals_err_t spi_master_init_device_config( spi_device_interface_config_t* device_conf,
uint8_t command_len, uint8_t address_len, uint8_t dummy_len,
uint8_t mode, int clk_speed, int CS_pin,
uint16_t cs_ena_pretrans, uint8_t cs_ena_posttrans, uint16_t duty_cycle_pos,
int input_delay_ns, uint32_t flags
)
{
    spi_device_interface_config_t** device_conf_addr = (spi_device_interface_config_t**)device_conf;

    *device_conf_addr = (spi_device_interface_config_t*)calloc(1, sizeof(spi_device_interface_config_t));

    if(*device_conf_addr == NULL)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_ALLOC_FAILED, __FILE__, __LINE__, "Alloc failed");
        #endif
        return SPI_MASTER_ALLOC_FAILED;
    }

    // setup

    if(clk_speed > CONFIG_SPI_MASTER_LIMIT_FREQ_GPIO_MATRIX)
    {
        // over max frequency
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_DEVICE_CLK_OVER_FREQ, __FILE__, __LINE__, "over maximum spi frequency");
        #endif

        free(*device_conf_addr);
        *device_conf_addr = NULL;
        return SPI_MASTER_DEVICE_CLK_OVER_FREQ;
    }

    // admin setup
    (*device_conf_addr)->command_bits = command_len;
    (*device_conf_addr)->address_bits = address_len;
    (*device_conf_addr)->dummy_bits = dummy_len;

    (*device_conf_addr)->mode = mode;                         //
    (*device_conf_addr)->clock_speed_hz = clk_speed;          //
    (*device_conf_addr)->spics_io_num = CS_pin;               // using GPIO maxtrix, maximum freq is 26.6Mhz if input delay 0ns
                                                              // 10Mhz if input delay 50ns

    (*device_conf_addr)->cs_ena_pretrans = cs_ena_pretrans;   // some device need a number of cycle before get data
    (*device_conf_addr)->cs_ena_posttrans = cs_ena_posttrans; // some device need a number of cycle for handle data received before CS high
    (*device_conf_addr)->duty_cycle_pos = duty_cycle_pos;     // some device need special duty cycle. 0 for 50/50, 1-256 to set by specific

    (*device_conf_addr)->input_delay_ns = input_delay_ns;     // add a delay time before master get data from slave, sometime slave need a gap time for response

    // special mode
    (*device_conf_addr)->flags = flags; // almost flag is special mode, only use master send add normal

    // fixed setup for pooling
    (*device_conf_addr)->pre_cb = 0; //no callback  
    (*device_conf_addr)->post_cb= 0; //no callback

    (*device_conf_addr)->queue_size = 1; // 1 transaction at a time

    //ok
    return PERIPH_OK;
}


// free bus config
_peripherals_err_t spi_master_free_bus_config(spi_bus_config_t* master_conf)
{
    spi_bus_config_t** master_conf_addr = (spi_bus_config_t**) master_conf;

    if((*master_conf_addr) != NULL)
    {
        free(*master_conf_addr);
        *master_conf_addr = NULL;
    }

    return PERIPH_OK;
}

// free device config
_peripherals_err_t spi_master_free_device_config(spi_device_interface_config_t* device_conf)
{
    spi_device_interface_config_t** device_conf_addr = (spi_device_interface_config_t**) device_conf;

    if((*device_conf_addr) != NULL)
    {
        free(*device_conf_addr);
        *device_conf_addr = NULL;
    }

    return PERIPH_OK;
}


// install bus config
_peripherals_err_t spi_master_install_bus_config(const spi_bus_config_t* master_conf)
{
    esp_err_t ret = spi_bus_initialize(SPI_MASTER_PORT, master_conf, SPI_DMA_CH_AUTO);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_INS_CONFIG_BUS_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return SPI_MASTER_INS_CONFIG_BUS_FAILED;
    }

    return PERIPH_OK;
}


// register a device
_peripherals_err_t spi_master_register_device_config(const spi_device_interface_config_t* device_conf, 
    spi_device_handle_t* device_handle)
{
    esp_err_t ret = spi_bus_add_device(SPI_MASTER_PORT, device_conf, device_handle);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_REG_DEVICE_CONFIG_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return SPI_MASTER_REG_DEVICE_CONFIG_FAILED;
    }

    return PERIPH_OK;
}



// Un-register device
_peripherals_err_t spi_master_un_register_device_config(spi_device_handle_t device_handle)
{
    if(device_handle == NULL)
    {
        return PERIPH_OK;
    }

    esp_err_t ret = spi_bus_remove_device(device_handle); // << this funtion will remove device and free 'device_handle'

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_UN_REG_DEVICE_CONFIG_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return SPI_MASTER_UN_REG_DEVICE_CONFIG_FAILED;
    }

    device_handle = NULL; // if remove done

    return PERIPH_OK;

}


// Un-install spi bus (spi master port was chosen)
_peripherals_err_t spi_master_un_install_bus_config()
{
    esp_err_t ret = spi_bus_free(SPI_MASTER_PORT);

    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_UN_INS_CONFIG_BUS_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return SPI_MASTER_UN_INS_CONFIG_BUS_FAILED;
    }

    return PERIPH_OK;

}


// prepare transaction
_peripherals_err_t spi_master_prepare_transaction( spi_transaction_ext_t* spi_expand_transaction,
    uint8_t command_len, uint8_t address_len, uint8_t dummy_len,
    uint16_t command_data, uint64_t address_data, 
    size_t tx_data_len_by_bit, size_t rx_data_len_by_bit)
{
    spi_transaction_ext_t** spi_expand_transaction_addr = (spi_transaction_ext_t**)spi_expand_transaction;

    *spi_expand_transaction_addr = (spi_transaction_ext_t*)calloc(1, sizeof(spi_transaction_ext_t));

    if(*spi_expand_transaction_addr == NULL)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_ALLOC_FAILED, __FILE__, __LINE__, "Alloc failed");
        #endif
        return SPI_MASTER_ALLOC_FAILED;
    }


    // admin define
    (*spi_expand_transaction_addr)->command_bits = command_len;
    (*spi_expand_transaction_addr)->address_bits = address_len;
    (*spi_expand_transaction_addr)->dummy_bits = dummy_len;

    (*spi_expand_transaction_addr)->base.cmd = command_data;
    (*spi_expand_transaction_addr)->base.addr = address_data;

    (*spi_expand_transaction_addr)->base.length = tx_data_len_by_bit;
    (*spi_expand_transaction_addr)->base.rxlength = rx_data_len_by_bit;

    (*spi_expand_transaction_addr)->base.user = NULL; // no using this pointer :)

    // set tx buffer
    size_t remain_x = tx_data_len_by_bit % 32;
    size_t buf_size = (tx_data_len_by_bit + 32 - remain_x) / 8;

    (*spi_expand_transaction_addr)->base.tx_buffer = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);

    if(((*spi_expand_transaction_addr)->base.tx_buffer) == NULL)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_ALLOC_FAILED, __FILE__, __LINE__, "Alloc failed");
        #endif

        // free
        free(*spi_expand_transaction_addr);
        *spi_expand_transaction_addr = NULL;

        return SPI_MASTER_ALLOC_FAILED;
    }

    // set rx buffer
    remain_x = rx_data_len_by_bit % 32;
    buf_size = (rx_data_len_by_bit + 32 - remain_x) / 8; 

    (*spi_expand_transaction_addr)->base.rx_buffer = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);

    if(((*spi_expand_transaction_addr)->base.rx_buffer) == NULL)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_ALLOC_FAILED, __FILE__, __LINE__, "Alloc failed");
        #endif

        // free
        free((*spi_expand_transaction_addr)->base.tx_buffer);
        *spi_expand_transaction_addr = NULL;

        free(*spi_expand_transaction_addr);
        *spi_expand_transaction_addr = NULL;

        return SPI_MASTER_ALLOC_FAILED;
    }

    // special flags
    (*spi_expand_transaction_addr)->base.flags = SPI_TRANS_VARIABLE_CMD|
                                                SPI_TRANS_VARIABLE_ADDR|
                                                SPI_TRANS_VARIABLE_DUMMY;
                                                // do not set SPI_TRANS_USE_RXDATA and SPI_TRANS_USE_TXDATA
                                                // because using buffer define for DMA

    //ok
    return PERIPH_OK;
}


// delete transaction
_peripherals_err_t spi_master_delete_transaction( spi_transaction_ext_t* spi_expand_transaction)
{
    spi_transaction_ext_t** spi_expand_transaction_addr = (spi_transaction_ext_t**)spi_expand_transaction;

    if((*spi_expand_transaction_addr) != NULL)
    {
        if((*spi_expand_transaction_addr)->base.tx_buffer != NULL)
        {
            free((*spi_expand_transaction_addr)->base.tx_buffer);
            (*spi_expand_transaction_addr)->base.tx_buffer = NULL;
        }
        if((*spi_expand_transaction_addr)->base.rx_buffer!= NULL)
        {
            free((*spi_expand_transaction_addr)->base.rx_buffer);
            (*spi_expand_transaction_addr)->base.rx_buffer = NULL;
        }

        free(*spi_expand_transaction_addr);
        *spi_expand_transaction_addr = NULL;
    }

    return PERIPH_OK;
}


// start polling transaction
_peripherals_err_t spi_master_polling_transaction(const spi_device_handle_t device_handle ,spi_transaction_ext_t* spi_expand_transaction)
{
    esp_err_t ret = spi_device_polling_transmit(device_handle, spi_expand_transaction);
    
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(SPI_MASTER_POLLING_TRANSACTION_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return SPI_MASTER_POLLING_TRANSACTION_FAILED;
    }

    return PERIPH_OK;
}