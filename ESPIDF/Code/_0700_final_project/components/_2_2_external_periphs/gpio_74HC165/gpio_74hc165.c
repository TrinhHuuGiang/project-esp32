/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "gpio_74hc165.h"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */

static gpio_74hc165_data_t* data_struct = NULL;
static TaskHandle_t gpio_74HC165_task_handle = NULL;
static uint8_t volatile driver_started = 0; // no CPU cache, task alway read from RAM
static uint8_t volatile driver_stopped = 0; // no cache
    // flag check input driver has started or be stopping
    // Note: only set by driver   -> driver task started
    // Note: only clear after set -> driver task will stop in the future
    // check driver_stopped sure task ended
/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
static void gpio_74HC165_renew_register_driver(void* param);
 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */
// Initial input gate
uint8_t gpio_74HC165_init_input_gate()
{
    // input not NULL pointer
    if(data_struct != NULL)
    {
        return 1;
    }

    // alloc
    data_struct = (gpio_74hc165_data_t*) calloc (1, sizeof(gpio_74hc165_data_t));

    if(data_struct == NULL)
    {
        return 2;
    }

    // init
    data_struct->reg_state = 0;

    // init GPIO input
    gpio_setup_reset_pin_to_origin(GPIO_74HC165_DATA_SHIFT_IN_PIN);
    if(gpio_setup_io_direction(GPIO_74HC165_DATA_SHIFT_IN_PIN, GPIO_MODE_INPUT)) return 5;
    if(gpio_setup_pull_res(GPIO_74HC165_DATA_SHIFT_IN_PIN, GPIO_FLOATING)) return 5; // using external pull

    // init GPIO output
    if(gpio_setup_reset_pin_to_origin(GPIO_74HC165_CLOCK_PIN)) return 6;
    if(gpio_setup_reset_pin_to_origin(GPIO_74HC165_LATCH_PIN)) return 6;

    if(gpio_setup_pull_res(GPIO_74HC165_CLOCK_PIN, GPIO_FLOATING)) return 6;
    if(gpio_setup_pull_res(GPIO_74HC165_LATCH_PIN, GPIO_FLOATING)) return 6;

    if(gpio_setup_io_direction(GPIO_74HC165_CLOCK_PIN, GPIO_MODE_OUTPUT)) return 6;
    if(gpio_setup_io_direction(GPIO_74HC165_LATCH_PIN, GPIO_MODE_OUTPUT)) return 6;
    
    if(gpio_setup_output_logic_level(GPIO_74HC165_CLOCK_PIN, 0)) return 6;
    if(gpio_setup_output_logic_level(GPIO_74HC165_LATCH_PIN, 0)) return 6;

    // create driver
    BaseType_t res = xTaskCreate(
        gpio_74HC165_renew_register_driver,
        "gpio_74hc165_input_task",
        GPIO_74HC165_DRIVER_STACK,    // stack overflow at TASK_STACK_SIZE_LOW
        NULL,
        GPIO_74HC165_DRIVER_PRIO,
        &gpio_74HC165_task_handle
    );

    if (res != pdPASS)
    {
        free(data_struct);
        data_struct = NULL;
        return 7;
    }

    // ok
    return 0;
}


// de-Initial input gate
uint8_t gpio_74HC165_de_init_input_gate()
{
    // wait task stopped
    gpio_74HC165_task_handle = NULL;
    driver_started = 0;

    while(!driver_stopped)
    {vTaskDelay(pdMS_TO_TICKS(10));}

    driver_stopped = 0;

    // input NULL pointer
    if(data_struct != NULL)
    {
        free(data_struct);
        data_struct = NULL;
    }

    // ok
    return 0;
}



// check flag and set value
static void gpio_74HC165_renew_register_driver(void* param)
{    
    // start driver
    if(!(driver_started))
    {
        driver_started = 1;
    }

    // loop until driver_started set to 0
    while(driver_started)
    {
        // delay to stable data
        vTaskDelay(1);

        // handle renew register
        gpio_setup_output_logic_level(GPIO_74HC165_LATCH_PIN, 1);

        uint8_t logic_level = 0;
        for(int i = 0; i<GPIO_74HC165_DATA_SHIFT_IN_LEN ; i++)
        {
            // delay to stable data
            vTaskDelay(1);

            // capture data immediate after latch
            gpio_setup_get_logic_level(GPIO_74HC165_DATA_SHIFT_IN_PIN, &logic_level);

            if(logic_level) (data_struct->reg_state) |= (1<<i); 
            else (data_struct->reg_state) &=~(1<<i);

            // ok if loop 32 time
            if(i == (int)(GPIO_74HC165_DATA_SHIFT_IN_LEN-1)) break; // the last loop will pulse trash so continue :)

            // pulse high then pulse low to shift next data

            gpio_setup_output_logic_level(GPIO_74HC165_CLOCK_PIN, 1);

            vTaskDelay(1);

            gpio_setup_output_logic_level(GPIO_74HC165_CLOCK_PIN, 0);

            // total time : __|--| ~> 2tick for shift 1 data -> 'for' loop need 64tick
        }
    }

    driver_stopped = 1; 
    vTaskDelete(NULL); // delete task
}


// get current input reg
// after get reg, AND with (1<< gpio_74hc165_in_num_t)
uint32_t gpio_74HC165_current_input_reg()
{
    return reverse_bits_32(data_struct->reg_state);
}