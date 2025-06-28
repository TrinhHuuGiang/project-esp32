/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef _SPI_MCP41010_H_
#define _SPI_MCP41010_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
// c
#include <stdint.h>
#include <stdlib.h>

// defined
#include "spi_master_handle.h"
#include "_peripherals_err.h"


// setup
#define SPI_MCP41010_CLK_SPEED    SPI_MASTER_CLOCK_FREQ_1M // maximum 10MHz, read datasheet

#define SPI_MCP41010_MODE_0       SPI_MASTER_CLOCK_MODE_0  // support 2 mode: 0 (0,0) and 3 (1,1)

#define SPI_MCP41010_COMMAND_LEN      (8)  //bit
#define SPI_MCP41010_ADDRESS_LEN      (0)
#define SPI_MCP41010_DUMMY_LEN        (0)
#define SPI_MCP41010_TX_BIT_DATA_LEN  (8)


// command
#define SPI_MCP41010_COMMAND_WRITE_DATA   (0x11) // XX 01 XX 01 <- write mode and potentiometer 0

// limit
#define SPI_MCP41010_MIN_LEVEL    (0x00) // mcp41010 support 256 taps by 8bit data register
#define SPI_MCP41010_MAX_LEVEL    (0xFF)

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

//============================== Register & unregister spi      ==============================
//============================== ============================== ==============================
// register and get handle
uint8_t spi_mcp41010_register_and_get_handle(int CS_pin, spi_device_handle_t* device_handle);

// un-register
uint8_t spi_mcp41010_un_register_and_delete_handle(spi_device_handle_t device_handle);





//============================== Get & delete transaction template ===========================
//============================== ============================== ==============================
// get transaction template
uint8_t spi_mcp41010_create_transaction_template(spi_transaction_ext_t* spi_expand_transaction);

// admin add data to transaction (no code :) add by hand)

// delete transaction template
uint8_t spi_mcp41010_delete_transaction_template(spi_transaction_ext_t *spi_expand_transaction);





//============================== master communicates with slave ==============================
//============================== ============================== ==============================
// start transaction
// set level
uint8_t spi_mcp41010_send_potentiometer_level(const spi_device_handle_t device_handle, spi_transaction_ext_t *spi_expand_transaction,
    uint8_t level);



#endif