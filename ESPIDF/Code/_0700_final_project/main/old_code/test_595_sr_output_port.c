/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */
#include "main_init_api.h"


/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */
#define main_tag "MAIN_LOG"


/**
 * **********************************************************
 * Main
 * **********************************************************
 */
void app_main(void)
{
    // return err code
    int ret  = 0;

    // task sync
    if(task_sync_tools_init()) goto main_log_restart;



    // [SPI]
    // varable
    spi_bus_config_t* spi_master_bus_conf = NULL;

    if(spi_master_init_bus_config(&spi_master_bus_conf) != PERIPH_OK) goto main_log_restart;

    if(spi_master_install_bus_config(spi_master_bus_conf) != PERIPH_OK) goto main_log_restart;
    
    spi_master_free_bus_config(&spi_master_bus_conf);

    // [595]
    if(gpio_74HC595_init_output_gate()) goto main_log_restart;

    // 0 111 111 0 |  00001111 | 11000011
    if(gpio_74HC595_set_output_bit(0xFFFFFFFF, 
    (1<<GPIO_74HC595_OUTPUT_NUM_1) |
    (1<<GPIO_74HC595_OUTPUT_NUM_2) |
    (1<<GPIO_74HC595_OUTPUT_NUM_3) |
    (1<<GPIO_74HC595_OUTPUT_NUM_4) |
    (1<<GPIO_74HC595_OUTPUT_NUM_5) |
    (1<<GPIO_74HC595_OUTPUT_NUM_6) |
    (1<<GPIO_74HC595_OUTPUT_NUM_12) |
    (1<<GPIO_74HC595_OUTPUT_NUM_13) |
    (1<<GPIO_74HC595_OUTPUT_NUM_14) |
    (1<<GPIO_74HC595_OUTPUT_NUM_15) |
    (1<<GPIO_74HC595_OUTPUT_NUM_16) |
    (1<<GPIO_74HC595_OUTPUT_NUM_17) |
    (1<<GPIO_74HC595_OUTPUT_NUM_22) |
    (1<<GPIO_74HC595_OUTPUT_NUM_23)
    )
    ) goto main_log_restart;

    // main loop
    while(1)
    {
        
        fprintf(stderr, "loop1s");
        // delay
        vTaskDelay(pdMS_TO_TICKS(1000)); // check system feedback
    }

// restart
main_log_restart:
    ESP_LOGW(main_tag, "Err code: %d" ,ret);
    // restart in 5s + log
    for(int i = 3; i>0;i--)
    {
        ESP_LOGW(main_tag, "Restart in: %d" ,i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // reset chip
    esp_restart();

    // ok
    return;
}
