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

#include "widDr_display_performance.h"



extern __task_sync_t* g_task_sync_tools; // get sync tools 


/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
// internal function: create and clear screen
static uint8_t widDr_display_create_handle(void);

// update one line based on enum
static uint8_t widDr_display_update_line(widdr_display_line_num_t line);

// Pad ' ' to the right if string is shorter than total_len.
// If longer, truncate at total_len.
// Note: char[] buffer must be at least total_len + 1.
static void pad_right(char* str, size_t total_len);

// update 6 first lines
static uint8_t widDr_display_update_all();

// task handle renew content 6 line first
static void widDr_display_task(void* pvParameters);




/**
 * **********************************************************
 * Code
 * **********************************************************
 */

// internal function: create and clear screen
static uint8_t widDr_display_create_handle(void)
{
    if (i2c_ssd1306_clear_screen(WIDDR_DISPLAY_SSD1306_ADDR, I2C_SSD1306_PAGE_ADDR_MODE))
        return 1;
    
    if (i2c_ssd1306_on_off_screen(WIDDR_DISPLAY_SSD1306_ADDR, 1))
        return 2;

    return 0;
}

// support update line
static void pad_right(char* str, size_t total_len)
{
    size_t len = strlen(str);
    if (len >= total_len) {
        str[total_len] = '\0';  // truncate
        return;
    }

    memset(str + len, ' ', total_len - len);
    str[total_len] = '\0';
}


// update one line based on enum
static uint8_t widDr_display_update_line(widdr_display_line_num_t line)
{
    const uint8_t buf_size =63;
    char content[64] = {0};
    
    int battery_mv = 0;
    ds3231_reg_t* rtc_table = NULL;
    uint16_t year, mon, date, hour, min, sec;

    if (i2c_ssd1306_page_addr_choose_page(WIDDR_DISPLAY_SSD1306_ADDR, line))
        return 1;

    if (i2c_ssd1306_page_addr_choose_start_segment(WIDDR_DISPLAY_SSD1306_ADDR, 0))
        return 2;

    switch (line)
    {
    case WIDDR_LINE_0:
    {
        int total_heap = (int)(heap_caps_get_total_size(MALLOC_CAP_DEFAULT)/1024.0);

        snprintf(content, buf_size, "Heap: %dk/%dk",
        total_heap-(int)(esp_get_free_heap_size()/1024.0),
        total_heap);
        pad_right(content, 16);
        break;
    }

    case WIDDR_LINE_1:
        if (winDr_get_battery_mV(&battery_mv))
            snprintf(content, 17, "Battery: ---");
        else
            snprintf(content, 17, "Battery: %.2fV", battery_mv / 1000.0f);
        
        pad_right(content, 16);
        break;

    case WIDDR_LINE_3:
        if (i2c_ds3231_init_reg_table((ds3231_reg_t*)(&rtc_table))) return 3;
        if (i2c_ds3231_read_full_reg(rtc_table)) return 4;

        i2c_ds3231_translate_reg_to_time(*rtc_table, I2C_DS3231_REG_YEAR, &year);
        i2c_ds3231_translate_reg_to_time(*rtc_table, I2C_DS3231_REG_MONTH, &mon);
        i2c_ds3231_translate_reg_to_time(*rtc_table, I2C_DS3231_REG_DATE, &date);
        
        snprintf(content, buf_size, "DATE: %04d-%02d-%02d", year, mon, date);
        pad_right(content, 16);
        i2c_ds3231_clear_reg_table((ds3231_reg_t*)(&rtc_table));
        break;

    case WIDDR_LINE_4:
        if (i2c_ds3231_init_reg_table((ds3231_reg_t*)(&rtc_table))) return 5;
        if (i2c_ds3231_read_full_reg(rtc_table)) return 6;
        
        i2c_ds3231_translate_reg_to_time(*rtc_table, I2C_DS3231_REG_HOUR, &hour);
        i2c_ds3231_translate_reg_to_time(*rtc_table, I2C_DS3231_REG_MIN, &min);
        i2c_ds3231_translate_reg_to_time(*rtc_table, I2C_DS3231_REG_SEC, &sec);
        snprintf(content, buf_size, "TIME: %02d:%02d:%02d", hour, min, sec);
        pad_right(content, 16);
        i2c_ds3231_clear_reg_table((ds3231_reg_t*)(&rtc_table));
        break;

    case WIDDR_LINE_2:
    case WIDDR_LINE_5:
        strcpy(content, "----------------");
        break;

    default:
        return 7;
    }

    return i2c_ssd1306_convert_and_print_ASCII_bitmap(WIDDR_DISPLAY_SSD1306_ADDR, content, I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING);
}


// update 6 first lines
static uint8_t widDr_display_update_all()
{
    for (int line = WIDDR_LINE_0; line <= WIDDR_LINE_5; line++)
    {
        if (widDr_display_update_line((widdr_display_line_num_t)line))
            return 1 + line;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return 0;
}


// task update 6 line
static void widDr_display_task(void* pvParameters)
{
    int ret = 0;
    while (!ret)
    {
        ret = widDr_display_update_all();
        vTaskDelay(pdMS_TO_TICKS(WIDDR_DISPLAY_SSD1306_RENEW_TIME)); // update time
    }

    // set flag
    #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_UNKNOWN, 
            __FILE__, __LINE__, "display fail" );
        #endif

    g_task_sync_tools->err_flag = 1 ; // error 

    // some thing wrong
    vTaskDelete(NULL);

}




// public function: init and run background task
uint8_t widDr_display_init_and_run(void)
{
    if (widDr_display_create_handle() != 0)
    {
        printf("Display init failed\n");
        return 1;
    }

    BaseType_t ret = xTaskCreate(
            widDr_display_task,
            "widDrDispTask",
            WIDDR_DISPLAY_SSD1306_TASK_STACK,
            NULL,
            WIDDR_DISPLAY_SSD1306_TASK_PRIO,
            NULL
        );
    
    return (ret == pdPASS) ? 0 : 2;

}







// update 2 last log lines (not sync)
uint8_t widDr_display_write_log_line(const char* topic, const char* content)
{
    if (!topic || !content)
        return 1;

    char short_topic[17] = {0};   // 16 + 1 null-terminator
    char short_content[17] = {0};

    strncpy(short_topic, topic, 16);
    strncpy(short_content, content, 16);
    
    pad_right(short_topic, 16);
    pad_right(short_content, 16);

    // line 6: log topic
    if (i2c_ssd1306_page_addr_choose_page(WIDDR_DISPLAY_SSD1306_ADDR, WIDDR_LINE_6))
        return 2;
    if (i2c_ssd1306_page_addr_choose_start_segment(WIDDR_DISPLAY_SSD1306_ADDR, 0))
        return 3;
    if (i2c_ssd1306_convert_and_print_ASCII_bitmap(WIDDR_DISPLAY_SSD1306_ADDR, short_topic, I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING))
        return 4;

    // line 7: log content
    if (i2c_ssd1306_page_addr_choose_page(WIDDR_DISPLAY_SSD1306_ADDR, WIDDR_LINE_7))
        return 5;
    if (i2c_ssd1306_page_addr_choose_start_segment(WIDDR_DISPLAY_SSD1306_ADDR, 0))
        return 6;
    if (i2c_ssd1306_convert_and_print_ASCII_bitmap(WIDDR_DISPLAY_SSD1306_ADDR, short_content, I2C_SSD1306_DATA_TYPE_TO_PRINT_STRING))
        return 7;

    return 0;
}

