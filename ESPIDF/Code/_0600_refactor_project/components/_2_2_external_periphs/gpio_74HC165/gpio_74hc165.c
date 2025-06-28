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
#include "gpio_74hc165.h"

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
 * @brief set flag captured input events when interrupt occur
 */
static IRAM_ATTR void set_flag_capture_random_events(void* arg);


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */
// Initial input gate
uint8_t gpio_74HC165_init_input_gate(gpio_74hc165_data_t** data_struct)
{
    // input not NULL pointer
    if((*data_struct) != NULL)
    {
        return 1;
    }

    // alloc
    *data_struct = (gpio_74hc165_data_t*) calloc (1, sizeof(gpio_74hc165_data_t));

    if((*data_struct) == NULL)
    {
        return 2;
    }

    // init
    (*data_struct)->capture_flags = GPIO_74HC165_NOT_SET_FLAG;
    (*data_struct)->reg_state = 0;

    // init ledc pwm channel
    if(ledc_setup_timer_config(GPIO_74HC165_LEDC_TIMER_CLOCK,GPIO_74HC165_LEDC_FREQ_CLOCK,
        GPIO_74HC165_LEDC_RESOLUTION_CLOCK)) return 3;
    if(ledc_setup_channel_config_and_start(GPIO_74HC165_CLOCK_PIN, GPIO_74HC165_LEDC_CHANNEL_CLOCK,
        GPIO_74HC165_LEDC_TIMER_CLOCK, GPIO_74HC165_LEDC_RESOLUTION_CLOCK, GPIO_74HC165_LEDC_DUTY_CLOCK)) return 3;
    if(ledc_setup_change_pwm_duty(GPIO_74HC165_LEDC_CHANNEL_CLOCK, GPIO_74HC165_LEDC_DUTY_CLOCK,
        GPIO_74HC165_LEDC_RESOLUTION_CLOCK)) return 3;


    if(ledc_setup_timer_config(GPIO_74HC165_LEDC_TIMER_LATCH,GPIO_74HC165_LEDC_FREQ_LATCH,
        GPIO_74HC165_LEDC_RESOLUTION_LATCH)) return 4;
    if(ledc_setup_channel_config_and_start(GPIO_74HC165_LATCH_PIN, GPIO_74HC165_LEDC_CHANNEL_LATCH,
        GPIO_74HC165_LEDC_TIMER_LATCH,GPIO_74HC165_LEDC_RESOLUTION_LATCH, GPIO_74HC165_LEDC_DUTY_LATCH)) return 4; 
    if(ledc_setup_change_pwm_duty(GPIO_74HC165_LEDC_CHANNEL_LATCH, GPIO_74HC165_LEDC_DUTY_LATCH,
        GPIO_74HC165_LEDC_RESOLUTION_LATCH)) return 4;

    // init GPIO input
    gpio_setup_reset_pin_to_origin(GPIO_74HC165_DATA_SHIFT_IN_PIN);
    if(gpio_setup_io_direction(GPIO_74HC165_DATA_SHIFT_IN_PIN, GPIO_MODE_INPUT)) return 5;
    if(gpio_setup_pull_res(GPIO_74HC165_DATA_SHIFT_IN_PIN, GPIO_FLOATING)) return 5; // using external pull

    // add handler function when interrupt reached from input value and preset mode active interrupt
    if(gpio_setup_add_handler_for_pin(GPIO_74HC165_DATA_SHIFT_IN_PIN, set_flag_capture_random_events ,&((*data_struct)->capture_flags))) return 6;
    if(gpio_setup_isr_active_type_for_pin(GPIO_74HC165_DATA_SHIFT_IN_PIN, GPIO_74HC165_DATA_SHIFT_IN_INTERRUPT_TYPE)) return 6;

    // ok
    return 0;
}


// de-Initial input gate
uint8_t gpio_74HC165_de_init_input_gate(gpio_74hc165_data_t** data_struct)
{
    // input NULL pointer
    if((*data_struct) != NULL)
    {
        free(*data_struct);
        *data_struct = NULL;
    }

    // stop ledc,
    if(ledc_setup_stop_channel(GPIO_74HC165_LEDC_CHANNEL_CLOCK, 0)) return 1;
    if(ledc_setup_stop_channel(GPIO_74HC165_LEDC_CHANNEL_LATCH, 0)) return 1;

    // remove interrupt function
    if(gpio_setup_remove_handler_for_pin(GPIO_74HC165_DATA_SHIFT_IN_PIN)) return 2;

    // ok
    return 0;
}


//Start capture event
uint8_t gpio_74HC165_start_capture_random_events()
{
    // enable interrupt
    if(gpio_setup_enable_interrupt_for_pin(GPIO_74HC165_DATA_SHIFT_IN_PIN)) return 1;

    // ok
    return 0;
}

// sub function : interrupt set flags
static IRAM_ATTR void set_flag_capture_random_events(void* arg)
{
    uint8_t* cap_flag_ptr =(uint8_t*)arg;

    // set flag
    *cap_flag_ptr = GPIO_74HC165_SET_FLAG;

    // return
}


// stop interrupt capture events
uint8_t gpio_74HC165_stop_capture_random_events()
{
    // enable interrupt
    if(gpio_setup_disable_interrupt_for_pin(GPIO_74HC165_DATA_SHIFT_IN_PIN)) return 1;

    // ok
    return 0;
}

// check flag and set value
uint8_t gpio_74HC165_check_flag_and_renew_register(gpio_74hc165_data_t* data_struct, uint8_t* set_state)
{
    // check if not set will return
    if(data_struct->capture_flags == GPIO_74HC165_NOT_SET_FLAG)
    {
        *set_state = GPIO_74HC165_NOT_SET_FLAG;
        return 0;
    }

    *set_state = GPIO_74HC165_SET_FLAG;

    // if set
    // stop capture interrupt
    if(gpio_74HC165_stop_capture_random_events()) return 1;

    // reset flags
    data_struct->capture_flags = GPIO_74HC165_NOT_SET_FLAG;

    
    // stop ledc,
    if(ledc_setup_stop_channel(GPIO_74HC165_LEDC_CHANNEL_CLOCK, 0)) return 2;
    if(ledc_setup_stop_channel(GPIO_74HC165_LEDC_CHANNEL_LATCH, 0)) return 2;

    // set up output
    if(gpio_setup_reset_pin_to_origin(GPIO_74HC165_CLOCK_PIN)) return 2;
    if(gpio_setup_reset_pin_to_origin(GPIO_74HC165_LATCH_PIN)) return 2;

    if(gpio_setup_pull_res(GPIO_74HC165_CLOCK_PIN, GPIO_FLOATING)) return 2;
    if(gpio_setup_pull_res(GPIO_74HC165_LATCH_PIN, GPIO_FLOATING)) return 2;

    if(gpio_setup_io_direction(GPIO_74HC165_CLOCK_PIN, GPIO_MODE_OUTPUT)) return 2;
    if(gpio_setup_io_direction(GPIO_74HC165_LATCH_PIN, GPIO_MODE_OUTPUT)) return 2;
    
    if(gpio_setup_output_logic_level(GPIO_74HC165_CLOCK_PIN, 0)) return 2;
    if(gpio_setup_output_logic_level(GPIO_74HC165_LATCH_PIN, 0)) return 2;
    

    // delay to stable data
    esp_rom_delay_us(GPIO_74_HC165_DATA_SHIFT_DELAY_BY_HAND_US);

    // handle renew register
    if(gpio_setup_output_logic_level(GPIO_74HC165_LATCH_PIN, 1)) return 3;

    uint8_t logic_level = 0;
    for(int i = 0; i<32 ; i++)
    {
        // delay to stable data
        esp_rom_delay_us(GPIO_74_HC165_DATA_SHIFT_DELAY_BY_HAND_US);

        // capture data immediate after latch
        gpio_setup_get_logic_level(GPIO_74HC165_DATA_SHIFT_IN_PIN, &logic_level);

        if(logic_level) (data_struct->reg_state) |= (1<<i); 
        else (data_struct->reg_state) &=~(1<<i);

        // ok if loop 32 time
        if(i == 31) break; // the last loop will pulse trash so continue :)

        // pulse high then pulse low to shift next data

        if(gpio_setup_output_logic_level(GPIO_74HC165_CLOCK_PIN, 1)) return 4;

        esp_rom_delay_us(GPIO_74_HC165_DATA_SHIFT_DELAY_BY_HAND_US);

        if(gpio_setup_output_logic_level(GPIO_74HC165_CLOCK_PIN, 0)) return 4;

        // total time : __|--| ~> 4us for shift 1 data -> 'for' loop need 128us
    }

    // start ledc after done
    if(ledc_setup_channel_config_and_start(GPIO_74HC165_CLOCK_PIN, GPIO_74HC165_LEDC_CHANNEL_CLOCK,
        GPIO_74HC165_LEDC_TIMER_CLOCK, GPIO_74HC165_LEDC_RESOLUTION_CLOCK, GPIO_74HC165_LEDC_DUTY_CLOCK)) return 5;
    if(ledc_setup_channel_config_and_start(GPIO_74HC165_LATCH_PIN, GPIO_74HC165_LEDC_CHANNEL_LATCH,
        GPIO_74HC165_LEDC_TIMER_LATCH, GPIO_74HC165_LEDC_RESOLUTION_LATCH, GPIO_74HC165_LEDC_DUTY_LATCH)) return 6; 

    // start capture
    if(gpio_74HC165_start_capture_random_events()) return 7;

    // return 0
    return 0;

}