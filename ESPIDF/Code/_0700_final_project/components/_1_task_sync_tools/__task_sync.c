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

#include "__task_sync.h"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
__task_sync_t* g_task_sync_tools = NULL;


/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// mutex:
// + xSemaphoreCreateMutex()
// +  xSemaphoreTake()
// + xSemaphoreGive()
// +  vSemaphoreDelete()


// init tools
init_tasksync_t task_sync_tools_init()
{
    g_task_sync_tools = calloc(1, sizeof(__task_sync_t));
    if(g_task_sync_tools == NULL) return TSYNC_ALLOC_STRUCT_FAILED;


    // --------------> Layer 2.1 "internal pheriph"
    // ADC
    g_task_sync_tools->adc_setup_handle_mutex = xSemaphoreCreateMutex(); 
    if(g_task_sync_tools->adc_setup_handle_mutex == NULL) return TSYNC_MU_ADC_FAILED;
    // i2c master
    g_task_sync_tools->i2c_master_handle_mutex = xSemaphoreCreateMutex();
    if(g_task_sync_tools->i2c_master_handle_mutex == NULL) return TSYNC_MU_I2C_M_FAILED;
    // spi master
    g_task_sync_tools->spi_master_handle_mutex = xSemaphoreCreateMutex();
    if(g_task_sync_tools->spi_master_handle_mutex == NULL) return TSYNC_MU_SPI_M_FAILED;
    // gpio
    g_task_sync_tools->gpio_setup_handle_mutex = xSemaphoreCreateMutex(); 
    if(g_task_sync_tools->gpio_setup_handle_mutex == NULL) return TSYNC_MU_GPIO_FAILED;
    // ledc
    g_task_sync_tools->ledc_setup_handle_mutex = xSemaphoreCreateMutex(); 
    if(g_task_sync_tools->ledc_setup_handle_mutex == NULL) return TSYNC_MU_LEDC_FAILED;
    // mcpwm
    g_task_sync_tools->mcpwm_setup_handle_mutex = xSemaphoreCreateMutex(); 
    if(g_task_sync_tools->mcpwm_setup_handle_mutex == NULL) return TSYNC_MU_MCPWM_FAILED;
    // i2s master output
    g_task_sync_tools->i2s_master_output_mutex = xSemaphoreCreateMutex(); 
    if(g_task_sync_tools->i2s_master_output_mutex == NULL) return TSYNC_MU_I2S_MO_FAILED;

    
    // wifi
    g_task_sync_tools->wifi_state_mutex = xSemaphoreCreateMutex(); // wifi state
    if(g_task_sync_tools->wifi_state_mutex == NULL) return TSYNC_MU_WIFI_STATE_FAILED;

    g_task_sync_tools->wifi_manager_apsta_mutex = xSemaphoreCreateMutex(); // wifi apsta manager
    if(g_task_sync_tools->wifi_manager_apsta_mutex == NULL) return TSYNC_MU_WIFI_MAN_APSTA_FAILED;
    

    
    // --------------> Layer 2.2 "external peripherals" 
    g_task_sync_tools->adc_74hc4067_mutex = xSemaphoreCreateMutex(); // wifi state
    if(g_task_sync_tools->adc_74hc4067_mutex == NULL) return TSYNC_MU_ADC_74HC4067_FAILED;


    g_task_sync_tools->gpio_74hc595_cs_spi_mutex = xSemaphoreCreateMutex(); // wifi state
    if(g_task_sync_tools->gpio_74hc595_cs_spi_mutex == NULL) return TSYNC_MU_GPIO_74HC595_CS_SPI_FAILED;



    // --------------> Layer 2.3 "internal devices" 


    // --------------> Layer 2.4 "network protocols" 
    // mqtt
    g_task_sync_tools->mqtt_state_mutex = xSemaphoreCreateMutex();  // mqtt state
    if(g_task_sync_tools->mqtt_state_mutex == NULL) return TSYNC_MU_MQTT_STATE_FAILED;


    //ok
    return 0;
}
