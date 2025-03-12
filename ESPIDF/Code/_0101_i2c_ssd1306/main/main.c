/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c_master_handle.h"

#include "i2c_ssd1306.h"

#define SSD1306_ADDR (0x3C)

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

void app_main(void)
{
    // check enable debug
    #if CONFIG_DEBUG_ENABLE != 0
    fprintf(stderr,"[Debug mode]\n");
    #else
    fprintf(stderr,"[No-debug mode]\n");
    #endif

    i2c_config_t* master_conf=NULL;

    int status = 1;

    // config i2c master

    i2c_master_init_config(&master_conf);

    i2c_master_setup_hardware(master_conf);

    i2c_master_install_driver(master_conf);
    
    i2c_master_free_config(&master_conf);

    // send to i2c ssd1306

    i2c_ssd1306_on_off_screen(SSD1306_ADDR, 1);

    while (1)
    {   
        status = 1- status;
        fprintf(stderr,"\nStatus: %d, %d\n", status, i2c_ssd1306_turn_on_all_led(SSD1306_ADDR, status));
        vTaskDelay(pdMS_TO_TICKS(1000)); // Nghỉ 1000ms (1 giây)
    }
}
