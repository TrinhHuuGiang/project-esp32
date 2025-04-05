/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "main.h"

#define GPIO_RGB_1_LED_R_CONTROL (32)
#define GPIO_RGB_1_LED_G_CONTROL (33)
#define GPIO_RGB_1_LED_B_CONTROL (26)


#define GPIO_RGB_2_LED_R_CONTROL (27)
#define GPIO_RGB_2_LED_G_CONTROL (14)
#define GPIO_RGB_2_LED_B_CONTROL (13)

#define GPIO_RGB_1_LED_R (4)
#define GPIO_RGB_1_LED_G (2)
#define GPIO_RGB_1_LED_B (15)

#define GPIO_RGB_2_LED_R (0)
#define GPIO_RGB_2_LED_G (16)
#define GPIO_RGB_2_LED_B (17)


#define BUZZER_PIN_NEGATIVE (25)

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
static uint8_t led1_state = 0, led1_old_state = 0;  //0b0000 0BGR
static uint8_t led2_state = 0, led2_old_state = 0;

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */

// GPIO interrupt function
// toggle led
static void IRAM_ATTR toggle_RGB_led_red_1(void* arg);
static void IRAM_ATTR toggle_RGB_led_green_1(void* arg);
static void IRAM_ATTR toggle_RGB_led_blue_1(void* arg);

static void IRAM_ATTR toggle_RGB_led_red_2(void* arg);
static void IRAM_ATTR toggle_RGB_led_green_2(void* arg);
static void IRAM_ATTR toggle_RGB_led_blue_2(void* arg);

static void toggle_led_handler(void* arg);

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

void app_main(void)
{
    // install isr service for GPIO
    if(gpio_setup_install_isr_service_for_gpio_system()) return;

    if(gpio_setup_reset_pin_to_origin(GPIO_RGB_2_LED_B)) return;

    // gpio in out
    if(gpio_setup_io_direction(GPIO_RGB_1_LED_R_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_1_LED_G_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_1_LED_B_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_R_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_G_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_B_CONTROL,GPIO_MODE_INPUT)) return;

    if(gpio_setup_io_direction(GPIO_RGB_1_LED_R,GPIO_MODE_OUTPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_1_LED_G,GPIO_MODE_OUTPUT)) return;

    if(gpio_setup_reset_pin_to_origin(GPIO_RGB_1_LED_B)) return;
    if(gpio_setup_io_direction(GPIO_RGB_1_LED_B,GPIO_MODE_OUTPUT)) return;
    
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_R,GPIO_MODE_OUTPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_G,GPIO_MODE_OUTPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_B,GPIO_MODE_OUTPUT)) return;
    
    
    

    if(gpio_setup_io_direction(BUZZER_PIN_NEGATIVE,GPIO_MODE_OUTPUT)) return;

    // set pull
    if(gpio_setup_pull_res(GPIO_RGB_1_LED_R_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_1_LED_G_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_1_LED_B_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_2_LED_R_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_2_LED_G_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_2_LED_B_CONTROL,GPIO_PULLUP_ONLY)) return;


    // add isr
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_1_LED_R_CONTROL, toggle_RGB_led_red_1, NULL)) return;
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_1_LED_G_CONTROL, toggle_RGB_led_green_1, NULL)) return;
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_1_LED_B_CONTROL, toggle_RGB_led_blue_1, NULL)) return;

    if(gpio_setup_add_handler_for_pin(GPIO_RGB_2_LED_R_CONTROL, toggle_RGB_led_red_2, NULL)) return;
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_2_LED_G_CONTROL, toggle_RGB_led_green_2, NULL)) return;
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_2_LED_B_CONTROL, toggle_RGB_led_blue_2, NULL)) return;

    // isr active type rising edge
    if(gpio_setup_isr_active_type_for_pin(GPIO_RGB_1_LED_R_CONTROL, GPIO_INTR_POSEDGE)) return;
    if(gpio_setup_isr_active_type_for_pin(GPIO_RGB_1_LED_G_CONTROL, GPIO_INTR_POSEDGE)) return;
    if(gpio_setup_isr_active_type_for_pin(GPIO_RGB_1_LED_B_CONTROL, GPIO_INTR_POSEDGE)) return;

    if(gpio_setup_isr_active_type_for_pin(GPIO_RGB_2_LED_R_CONTROL, GPIO_INTR_POSEDGE)) return;
    if(gpio_setup_isr_active_type_for_pin(GPIO_RGB_2_LED_G_CONTROL, GPIO_INTR_POSEDGE)) return;
    if(gpio_setup_isr_active_type_for_pin(GPIO_RGB_2_LED_B_CONTROL, GPIO_INTR_POSEDGE)) return;

    // start isr
    if(gpio_setup_enable_interrupt_for_pin(GPIO_RGB_1_LED_R_CONTROL)) return;
    if(gpio_setup_enable_interrupt_for_pin(GPIO_RGB_1_LED_G_CONTROL)) return;
    if(gpio_setup_enable_interrupt_for_pin(GPIO_RGB_1_LED_B_CONTROL)) return;

    if(gpio_setup_enable_interrupt_for_pin(GPIO_RGB_2_LED_R_CONTROL)) return;
    if(gpio_setup_enable_interrupt_for_pin(GPIO_RGB_2_LED_G_CONTROL)) return;
    if(gpio_setup_enable_interrupt_for_pin(GPIO_RGB_2_LED_B_CONTROL)) return;


    // create led handle task
    int ret = 0;

    xTaskCreate(toggle_led_handler, "Toggle RGB Task", 2048, &ret, 5, NULL);

    while(1)
    {
        if(gpio_setup_output_logic_level(BUZZER_PIN_NEGATIVE,0)) return;
        vTaskDelay(pdMS_TO_TICKS(100));

        if(gpio_setup_output_logic_level(BUZZER_PIN_NEGATIVE,1)) return;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return;
}


// toggle led
static void IRAM_ATTR toggle_RGB_led_red_1(void* arg)
{
    led1_state ^= (1);
}

static void IRAM_ATTR toggle_RGB_led_green_1(void* arg)
{
    led1_state ^= (1<<1);
}

static void IRAM_ATTR toggle_RGB_led_blue_1(void* arg)
{
    led1_state ^= (1<<2);
}

static void IRAM_ATTR toggle_RGB_led_red_2(void* arg)
{
    led2_state ^= (1);
}

static void IRAM_ATTR toggle_RGB_led_green_2(void* arg)
{
    led2_state ^= (1<<1);
}

static void IRAM_ATTR toggle_RGB_led_blue_2(void* arg)
{
    led2_state ^= (1<<2);
}

static void toggle_led_handler(void* arg)
{
    int* ret = (int*)arg;

    while(! (*ret))
    {
        if(led1_state != led1_old_state)
        {
            if(gpio_setup_output_logic_level(GPIO_RGB_1_LED_R , (uint8_t)(led1_state & (1)))) (*ret) = 1;
            if(gpio_setup_output_logic_level(GPIO_RGB_1_LED_G , (uint8_t)((led1_state & (1<<1)) >> 1)))  (*ret) = 1;
            if(gpio_setup_output_logic_level(GPIO_RGB_1_LED_B , (uint8_t)((led1_state & (1<<2)) >> 2)))  (*ret) = 1;

            led1_old_state = led1_state;
        }

        if(led2_state != led2_old_state)
        {
            if(gpio_setup_output_logic_level(GPIO_RGB_2_LED_R , (uint8_t)(led2_state & (1))))  (*ret) = 1;
            if(gpio_setup_output_logic_level(GPIO_RGB_2_LED_G , (uint8_t)((led2_state & (1<<1)) >> 1)))  (*ret) = 1;
            if(gpio_setup_output_logic_level(GPIO_RGB_2_LED_B , (uint8_t)((led2_state & (1<<2)) >> 2)))  (*ret) = 1;

            led2_old_state = led2_state;
        }

        //delay
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    // exit
    vTaskDelete(NULL);
}