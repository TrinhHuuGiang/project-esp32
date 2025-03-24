/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "main.h"

/**
 * **********************************************************
 * Variables
 * **********************************************************
 */
// component
static ds3231_reg_t* s_ds3231_reg_table = NULL;
static int s_ds3231_day_status = 0; // day from 1-7

static spi_device_handle_t s_spi_mcp41010_handle = NULL;
static spi_transaction_ext_t* s_spi_mcp41010_transaction_template = NULL;
static uint8_t s_spi_mcp41010_level = 0; // 0 - 255

// main function

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
/**
 * @brief Initial peripherals and components
 */
static uint8_t Init_Components();

/**
 * @brief Display clock 
 * @note read ds3231 -> write ssd1306
 */
static uint8_t display_clock();

/**
 * @brief Set potentiometer level up value and display
 * @note setvalue ++ -> mcp41010 -> print value
 */
static uint8_t potentiometer_up_level_and_display();

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */
void app_main(void)
{
    uint8_t ret = 0;

    // check enable debug
    #if CONFIG_DEBUG_ENABLE != 0
    fprintf(stderr,"[Debug mode]\n");
    #else
    fprintf(stderr,"[No-debug mode]\n");
    #endif

    ESP_LOGI("MAIN", "Wait for initial components");

    // init
    ret = Init_Components();
    if(ret)
    {
        ESP_LOGE("MAIN","\n[Err code - %d] [%s] [%d] [%s]\n", ret, __FILE__, __LINE__, "Init components failed");
        return;
    }



    // toogle display variable
    int flip_ = 0, rev_ = 0;

    int loop_time = 50; // ms

    // delay always >0
    int clock_delay_count = 18, clock_count = clock_delay_count;  // 900ms
    int potent_delay_count = 1, potent_count= potent_delay_count; // 50ms

    // loop
    while(1)
    {   

        // clock
        // ssd1306 page 3->7
        if(!clock_count)
        {
            clock_count = clock_delay_count;

            if(display_clock()) return;

            // flip light
            if(++flip_ > 10)
            {
                if(i2c_ssd1306_reverse_light_display(SSD1306_ADDR, rev_)) return;
                rev_=1-rev_;
                flip_ = 0;
            }
        }

        // mcp level
        // ssd1306 page 0->1
        if(!potent_count)
        {
            potent_count= potent_delay_count;

            if(potentiometer_up_level_and_display()) return;
        }
    

        // Task delay
        clock_count--; potent_count--;

        vTaskDelay(pdMS_TO_TICKS(loop_time));
    }

    // exit ok
    ESP_LOGE("MAIN","\n[Err code - %d] [%s] [%d] [%s]\n", ret, __FILE__, __LINE__, "End loop");

    return 0;
}

// init peripherals and components
static uint8_t Init_Components()
{

    // ======================= Peripherals
    // [I2C]
    // varable
    i2c_config_t* i2c_master_conf = NULL;

    if(i2c_master_init_config(&i2c_master_conf) != PERIPH_OK) return 1;
    if(i2c_master_setup_hardware(i2c_master_conf) != PERIPH_OK) return 1;
    if(i2c_master_install_driver(i2c_master_conf) != PERIPH_OK) return 1;
    i2c_master_free_config(&i2c_master_conf);

    // [SPI]
    // varable
    spi_bus_config_t* spi_master_bus_conf = NULL;

    if(spi_master_init_bus_config(&spi_master_bus_conf) != PERIPH_OK) return 2;
    if(spi_master_install_bus_config(spi_master_bus_conf) != PERIPH_OK) return 2;
    spi_master_free_bus_config(&spi_master_bus_conf);


    // ======================= Components
    // [ssd1306]
    // clear anh choose Page mode
    if(i2c_ssd1306_clear_screen(SSD1306_ADDR, I2C_SSD1306_PAGE_ADDR_MODE)) return 255;
    if(i2c_ssd1306_on_off_screen(SSD1306_ADDR, 1)) return 255;
    
    // [ds3231]
    if(i2c_ds3231_init_reg_table(&s_ds3231_reg_table)) return 254;

    // [mcp41010]
    // varable
    spi_device_interface_config_t* spi_mcp41010_conf = NULL;

    if(spi_mcp41010_register_and_get_handle(&spi_mcp41010_conf,MCP41010_CS_PIN,&s_spi_mcp41010_handle)) return 200;
    if(spi_mcp41010_create_transaction_template(&s_spi_mcp41010_transaction_template)) return 200;


    // ======================= ok
    return 0;
}

// display clock
static uint8_t display_clock()
{
    int hh = 0,mm = 0,ss = 0,d = 0,dt = 0,mth = 0,yyyy = 0;

    if(i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 3)) return 1;
    if(i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0)) return 1;
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "hh:mm:ss", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;

    if(i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 5)) return 1;
    if(i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0)) return 1;
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "dd:mm:yyyy", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;

    // read ds3231 register
    if(i2c_ds3231_read_full_reg(s_ds3231_reg_table)) return 1;

    // translate to hh:mm:ss , dt:mth:yyyy
    if(i2c_ds3231_translate_reg_to_time(*s_ds3231_reg_table, I2C_DS3231_REG_HOUR, &hh)) return 1;
    if(i2c_ds3231_translate_reg_to_time(*s_ds3231_reg_table, I2C_DS3231_REG_MIN, &mm)) return 1;
    if(i2c_ds3231_translate_reg_to_time(*s_ds3231_reg_table, I2C_DS3231_REG_SEC, &ss)) return 1;
    if(i2c_ds3231_translate_reg_to_time(*s_ds3231_reg_table, I2C_DS3231_REG_DAY, &d)) return 1;
    if(i2c_ds3231_translate_reg_to_time(*s_ds3231_reg_table, I2C_DS3231_REG_DATE, &dt)) return 1;
    if(i2c_ds3231_translate_reg_to_time(*s_ds3231_reg_table, I2C_DS3231_REG_MONTH, &mth)) return 1;
    if(i2c_ds3231_translate_reg_to_time(*s_ds3231_reg_table, I2C_DS3231_REG_YEAR, &yyyy)) return 1;

    // push to screen
    // hh:mm:ss
    if(i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 4)) return 1;
    if(i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0)) return 1;
    if(hh<10) 
    {if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, " ", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;}
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, &hh, I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32)) return 1;
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, ":", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
    if(mm<10) 
    {if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, " ", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;}
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, &mm, I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32)) return 1;
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, ":", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
    if(ss<10) 
    {if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, " ", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;}
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, &ss, I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32)) return 1;  

    // dd:mm:yyyy
    if(i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 6)) return 1;
    if(i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0)) return 1;
    if(dt<10) 
    {if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, " ", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;}
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, &dt, I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32)) return 1;
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, ":", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
    if(mth<10) 
    {if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, " ", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;}
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, &mth, I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32)) return 1;
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, ":", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, &yyyy, I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32)) return 1;

    // date
    if(d != s_ds3231_day_status)
    {
        // update
        s_ds3231_day_status = d;

        if(i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 7)) return 1;
        if(i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0)) return 1;
        if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, I2C_SSD1306_EMPTY_PAGE, I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
        if(i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 7)) return 1;
        if(i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0)) return 1;
        if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Today: ", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
        switch (d)
        {
            case 1: if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Monday", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
            break;
            case 2: if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Tuesday", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
            break;
            case 3: if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Wednesday", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
            break;
            case 4: if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Thursday", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
            break;
            case 5: if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Friday", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
            break;
            case 6: if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Saturday", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
            break;
            case 7: if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Sunday", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
            break;
            
            default:
            if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "Unknown", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;
            break;
        }
    }

    // ok
    return 0;
}


// potentiometer
static uint8_t potentiometer_up_level_and_display()
{
    // send level
    if(spi_mcp41010_send_potentiometer_level(s_spi_mcp41010_handle, s_spi_mcp41010_transaction_template,s_spi_mcp41010_level)) return 1;

    // display
    if(i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 0)) return 2;
    if(i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0)) return 2;
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, "MCP41010 level:", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 1;

    if(i2c_ssd1306_page_addr_choose_page(SSD1306_ADDR, 1)) return 2;
    if(i2c_ssd1306_page_addr_choose_start_segment(SSD1306_ADDR,0)) return 2;
    if(s_spi_mcp41010_level<10) 
    {if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, " ", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 2;}
    if(s_spi_mcp41010_level<100) 
    {if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, " ", I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING)) return 2;}

    int mcp41010_level = s_spi_mcp41010_level; // 'i2c_ssd1306_convert_and_print_ASCII_bitmap' only accept int32, string, float
    if(i2c_ssd1306_convert_and_print_ASCII_bitmap(SSD1306_ADDR, &mcp41010_level, I2C_SSD1306_DATA_TYPE_TO_PRINT_INT32)) return 2;
    
    // uplevel
    s_spi_mcp41010_level++; // up to 255 then overflow to 0
    return 0;
}