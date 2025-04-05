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


// set timer, pwm for led RGB 1 and 2
#define LEDC_RGB_1_TIMER LEDC_TIMER_0
#define LEDC_RGB_2_TIMER LEDC_TIMER_1

#define LEDC_COMMON_FREQ_FOR_2_TIMER LEDC_SETUP_COMMON_FREQUENCY_5KHZ

// set channel
#define LEDC_RGB_1_CHANNEL_R LEDC_CHANNEL_0
#define LEDC_RGB_1_CHANNEL_G LEDC_CHANNEL_1
#define LEDC_RGB_1_CHANNEL_B LEDC_CHANNEL_2

#define LEDC_RGB_2_CHANNEL_R LEDC_CHANNEL_3
#define LEDC_RGB_2_CHANNEL_G LEDC_CHANNEL_4
#define LEDC_RGB_2_CHANNEL_B LEDC_CHANNEL_5
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
static void IRAM_ATTR set_mode_increase_duty_RGB_led_red_1(void* arg);
static void IRAM_ATTR set_mode_increase_duty_RGB_led_green_1(void* arg);
static void IRAM_ATTR set_mode_increase_duty_RGB_led_blue_1(void* arg);

static void IRAM_ATTR set_mode_increase_duty_RGB_led_red_2(void* arg);
static void IRAM_ATTR set_mode_increase_duty_RGB_led_green_2(void* arg);
static void IRAM_ATTR set_mode_increase_duty_RGB_led_blue_2(void* arg);

static void set_mode_increase_duty_led_handler(void* arg);

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

    // gpio in
    if(gpio_setup_io_direction(GPIO_RGB_1_LED_R_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_1_LED_G_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_1_LED_B_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_R_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_G_CONTROL,GPIO_MODE_INPUT)) return;
    if(gpio_setup_io_direction(GPIO_RGB_2_LED_B_CONTROL,GPIO_MODE_INPUT)) return;

    // set pull
    if(gpio_setup_pull_res(GPIO_RGB_1_LED_R_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_1_LED_G_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_1_LED_B_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_2_LED_R_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_2_LED_G_CONTROL,GPIO_PULLUP_ONLY)) return;
    if(gpio_setup_pull_res(GPIO_RGB_2_LED_B_CONTROL,GPIO_PULLUP_ONLY)) return;
    

    // ledc pwm setup

    if(ledc_setup_timer_config(LEDC_RGB_1_TIMER, LEDC_COMMON_FREQ_FOR_2_TIMER)) return;
    if(ledc_setup_timer_config(LEDC_RGB_2_TIMER, LEDC_COMMON_FREQ_FOR_2_TIMER)) return;

    if(ledc_setup_channel_config_and_start(GPIO_RGB_1_LED_R,LEDC_RGB_1_CHANNEL_R, LEDC_RGB_1_TIMER, 0)) return;
    if(ledc_setup_channel_config_and_start(GPIO_RGB_1_LED_G,LEDC_RGB_1_CHANNEL_G, LEDC_RGB_1_TIMER, 0)) return;
    if(ledc_setup_channel_config_and_start(GPIO_RGB_1_LED_B,LEDC_RGB_1_CHANNEL_B, LEDC_RGB_1_TIMER, 0)) return;
    if(ledc_setup_channel_config_and_start(GPIO_RGB_2_LED_R,LEDC_RGB_2_CHANNEL_R, LEDC_RGB_2_TIMER, 0)) return;
    if(ledc_setup_channel_config_and_start(GPIO_RGB_2_LED_G,LEDC_RGB_2_CHANNEL_G, LEDC_RGB_2_TIMER, 0)) return;
    if(ledc_setup_channel_config_and_start(GPIO_RGB_2_LED_B,LEDC_RGB_2_CHANNEL_B, LEDC_RGB_2_TIMER, 0)) return;

    // install fade function
    if(ledc_setup_install_hardware_auto_pwm_duty_driver()) return;

    // add isr
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_1_LED_R_CONTROL, set_mode_increase_duty_RGB_led_red_1, NULL)) return;
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_1_LED_G_CONTROL, set_mode_increase_duty_RGB_led_green_1, NULL)) return;
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_1_LED_B_CONTROL, set_mode_increase_duty_RGB_led_blue_1, NULL)) return;

    if(gpio_setup_add_handler_for_pin(GPIO_RGB_2_LED_R_CONTROL, set_mode_increase_duty_RGB_led_red_2, NULL)) return;
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_2_LED_G_CONTROL, set_mode_increase_duty_RGB_led_green_2, NULL)) return;
    if(gpio_setup_add_handler_for_pin(GPIO_RGB_2_LED_B_CONTROL, set_mode_increase_duty_RGB_led_blue_2, NULL)) return;

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

    xTaskCreate(set_mode_increase_duty_led_handler, "Handle pwm duty RGB Task", 2048, &ret, 5, NULL);

    while(1)
    {
        ESP_LOGI("main loop", "--- wait 1000ms");
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("main loop", "___ done");
    }

    return;
}


// toggle led
static void IRAM_ATTR set_mode_increase_duty_RGB_led_red_1(void* arg)
{
    led1_state ^= (1);
}

static void IRAM_ATTR set_mode_increase_duty_RGB_led_green_1(void* arg)
{
    led1_state ^= (1<<1);
}

static void IRAM_ATTR set_mode_increase_duty_RGB_led_blue_1(void* arg)
{
    led1_state ^= (1<<2);
}

static void IRAM_ATTR set_mode_increase_duty_RGB_led_red_2(void* arg)
{
    led2_state ^= (1);
}

static void IRAM_ATTR set_mode_increase_duty_RGB_led_green_2(void* arg)
{
    led2_state ^= (1<<1);
}

static void IRAM_ATTR set_mode_increase_duty_RGB_led_blue_2(void* arg)
{
    led2_state ^= (1<<2);
}

static void set_mode_increase_duty_led_handler(void* arg)
{
    int* ret = (int*)arg;

    uint32_t target_duty = (1<<LEDC_SETUP_FIX_COMMON_RESOLUTION)-1;

    int max_time_fade = 1000; // ms

    while(! (*ret))
    {
        if(led1_state != led1_old_state)
        {
            // led1 R
            if(led1_state & (1))
            {
                if(ledc_setup_auto_fade_pwm_duty_by_hardware(LEDC_RGB_1_CHANNEL_R,target_duty,max_time_fade)) (*ret) = 1;
            }
            else
            {
                // if only stop ledc but old fade value still exist
                // so before stop, set duty = 0
                if(ledc_setup_change_pwm_duty(LEDC_RGB_1_CHANNEL_R,0)) (*ret) = 1;
                if(ledc_setup_stop_channel(LEDC_RGB_1_CHANNEL_R , 0)) (*ret) = 1; 
            }
            // led1 G
            if(led1_state & (1<<1))
            {
                if(ledc_setup_auto_fade_pwm_duty_by_hardware(LEDC_RGB_1_CHANNEL_G,target_duty,max_time_fade)) (*ret) = 1;
            }
            else
            {
                if(ledc_setup_change_pwm_duty(LEDC_RGB_1_CHANNEL_G,0)) (*ret) = 1;
                if(ledc_setup_stop_channel(LEDC_RGB_1_CHANNEL_G , 0)) (*ret) = 1; 
            }
            // led1 B
            if(led1_state & (1<<2))
            {
                if(ledc_setup_auto_fade_pwm_duty_by_hardware(LEDC_RGB_1_CHANNEL_B,target_duty,max_time_fade)) (*ret) = 1;
            }
            else
            {
                if(ledc_setup_change_pwm_duty(LEDC_RGB_1_CHANNEL_B,0)) (*ret) = 1;
                if(ledc_setup_stop_channel(LEDC_RGB_1_CHANNEL_B , 0)) (*ret) = 1; 
            }

            // save old state
            led1_old_state = led1_state;
        }

        if(led2_state != led2_old_state)
        {
            // led2 R
            if(led2_state & (1))
            {
                if(ledc_setup_auto_fade_pwm_duty_by_hardware(LEDC_RGB_2_CHANNEL_R,target_duty,max_time_fade)) (*ret) = 1;
            }
            else
            {
                if(ledc_setup_change_pwm_duty(LEDC_RGB_2_CHANNEL_R,0)) (*ret) = 1;
                if(ledc_setup_stop_channel(LEDC_RGB_2_CHANNEL_R , 0)) (*ret) = 1; 
            }
            // led2 G
            if(led2_state & (1<<1))
            {
                if(ledc_setup_auto_fade_pwm_duty_by_hardware(LEDC_RGB_2_CHANNEL_G,target_duty,max_time_fade)) (*ret) = 1;
            }
            else
            {
                if(ledc_setup_change_pwm_duty(LEDC_RGB_2_CHANNEL_G,0)) (*ret) = 1;
                if(ledc_setup_stop_channel(LEDC_RGB_2_CHANNEL_G , 0)) (*ret) = 1; 
            }
            // led2 B
            if(led2_state & (1<<2))
            {
                if(ledc_setup_auto_fade_pwm_duty_by_hardware(LEDC_RGB_2_CHANNEL_B,target_duty,max_time_fade)) (*ret) = 1;
            }
            else
            {
                if(ledc_setup_change_pwm_duty(LEDC_RGB_2_CHANNEL_B,0)) (*ret) = 1;
                if(ledc_setup_stop_channel(LEDC_RGB_2_CHANNEL_B , 0)) (*ret) = 1; 
            }

            // save old state
            led2_old_state = led2_state;
        }

        //delay
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    // exit
    vTaskDelete(NULL);
}