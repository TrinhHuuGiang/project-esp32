/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "i2c_ds3231.h"

 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */

 // --------------------Init & de-init----------------------
//  init struct register
uint8_t i2c_ds3231_init_reg_table(ds3231_reg_t* ds3231_reg_table)
{
    ds3231_reg_t** ds3231_reg_table_addr = ds3231_reg_table;
    *ds3231_reg_table_addr = (ds3231_reg_t*)calloc(1, sizeof(ds3231_reg_t));

    if(*ds3231_reg_table_addr == NULL)
    {
        return 1;
    }

    // return ok
    return 0;
}

//de init struct register
void i2c_ds3231_clear_reg_table(ds3231_reg_t* ds3231_reg_table)
{
    ds3231_reg_t** ds3231_reg_table_addr = ds3231_reg_table;

    if(*ds3231_reg_table_addr != NULL)
    {
        free(*ds3231_reg_table_addr);
        *ds3231_reg_table_addr = NULL;
    }
}

// --------------------Read time functions----------------------
// read all 16 reg
uint8_t i2c_ds3231_read_full_reg( ds3231_reg_t* ds3231_reg_table)
{
    if(ds3231_reg_table == NULL)
    {return 1;}

    i2c_master_list_t get_reg_table;

    get_reg_table.list_size = I2C_DS3231_MAX_ADDRESS; // 16 address data
    get_reg_table.list_ptr = (uint8_t*)calloc(get_reg_table.list_size,sizeof(uint8_t));

    if(get_reg_table.list_ptr == NULL)
    {
        return 2;
    }

    // set address to get data
    get_reg_table.list_size = 1;
    get_reg_table.list_ptr[0] = I2C_DS3231_REG_SEC_ADDR; // shift point register of ds3231 to 00H
    if(i2c_master_send_command_to_7bit_addr(I2C_DS3231_UNIQUE_ADDRESS,get_reg_table,I2C_DS3231_READ_DATA_TIMEOUT)
    == I2C_MASTER_SEND_COMMAND_FAILED)
    {
        free(get_reg_table.list_ptr);
        return 3;
    }

    // get data
    get_reg_table.list_size = I2C_DS3231_MAX_ADDRESS;
    if(i2c_master_get_data_from_7bit_addr(I2C_DS3231_UNIQUE_ADDRESS,&get_reg_table,I2C_DS3231_READ_DATA_TIMEOUT)
    == I2C_MASTER_GET_DATA_FAILED)
    {
        free(get_reg_table.list_ptr);
        return 4;
    }

    // set data
    memcpy(ds3231_reg_table->reg, get_reg_table.list_ptr, get_reg_table.list_size);

    // ok
    free(get_reg_table.list_ptr);
    return 0;
}

// Translate data to decimal uint16_t
uint8_t i2c_ds3231_translate_reg_to_time(ds3231_reg_t ds3231_reg_table, uint16_t type_time, uint16_t* return_value)
{
    switch (type_time)
    {
    case I2C_DS3231_REG_SEC:
        /* second from 0-59 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_SEC_ADDR] & 0x0F) + 
        10*(ds3231_reg_table.reg[I2C_DS3231_REG_SEC_ADDR] >> 4);
        break;
    case I2C_DS3231_REG_MIN:
        /* minute from 0-59 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_MIN_ADDR] & 0x0F) + 
        10*(ds3231_reg_table.reg[I2C_DS3231_REG_MIN_ADDR] >> 4);
        break;
    case I2C_DS3231_REG_HOUR:
        /* hour 1-12 or 0 - 23 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_HOUR_ADDR] & 0x0F);
        if((ds3231_reg_table.reg[I2C_DS3231_REG_HOUR_ADDR] & (0b01000000)) && 
        (ds3231_reg_table.reg[I2C_DS3231_REG_HOUR_ADDR] & (0b00100000))) //12hour and afternoon
        {
            *return_value += 10*((ds3231_reg_table.reg[I2C_DS3231_REG_HOUR_ADDR]&0b00010000) >> 4)  + 12;
            if(*return_value == (uint16_t)24){*return_value = 0;}
        }
        else // 24hour or 12hour morning
        {
            *return_value += 10*((ds3231_reg_table.reg[I2C_DS3231_REG_HOUR_ADDR]&0b00110000) >> 4);
        }
        break;
    case I2C_DS3231_REG_DAY:
        /* day 1-7 */
        *return_value = ds3231_reg_table.reg[I2C_DS3231_REG_DAY_ADDR];
        break;
    case I2C_DS3231_REG_DATE:
        /* 01-31 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_DATE_ADDR] & 0x0F) + 
        10*(ds3231_reg_table.reg[I2C_DS3231_REG_DATE_ADDR] >> 4);
        break;
    case I2C_DS3231_REG_MONTH:
        /* 5 LSB bit of register I2C_DS3231_REG_MONTH_CENT_ADDR */
        {
            uint8_t month_bit = ds3231_reg_table.reg[I2C_DS3231_REG_MONTH_CENT_ADDR] & 0x1F;
            *return_value = (month_bit & 0x0F) + 10*(month_bit >> 4);
            break;
        }
    case I2C_DS3231_REG_YEAR:
        /* year depend on  I2C_DS3231_REG_MONTH_CENT_ADDR[7] and I2C_DS3231_REG_YEAR_ADDR*/
        if(!(ds3231_reg_table.reg[I2C_DS3231_REG_MONTH_CENT_ADDR]&0x80)) // 20 century (1900U) + (0-99)
        {
            *return_value = I2C_DS3231_LOWEST_OFFSET_YEAR +
            (ds3231_reg_table.reg[I2C_DS3231_REG_YEAR_ADDR] & 0x0F) + 
            10*(ds3231_reg_table.reg[I2C_DS3231_REG_YEAR_ADDR] >> 4);
        }
        else // 21 century (2000U) + (0-99)
        {
            *return_value = I2C_DS3231_HIGHEST_OFFSET_YEAR +
            (ds3231_reg_table.reg[I2C_DS3231_REG_YEAR_ADDR] & 0x0F) + 
            10*(ds3231_reg_table.reg[I2C_DS3231_REG_YEAR_ADDR] >> 4);
        }
        break;
    case I2C_DS3231_REG_A1_SEC:
        /* ignore A1M1 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_A1_SEC_ADDR] & 0x0F) + 
        10*((ds3231_reg_table.reg[I2C_DS3231_REG_A1_SEC_ADDR] & (0x7F)) >> 4);
        break; 
    case I2C_DS3231_REG_A1_MIN:
        /* ignore A1M2 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_A1_MIN_ADDR] & 0x0F) + 
        10*((ds3231_reg_table.reg[I2C_DS3231_REG_A1_MIN_ADDR] & (0x7F)) >> 4);
        break;
    case I2C_DS3231_REG_A1_HOUR:
        /* ignore A1M3 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_A1_HOUR_ADDR] & 0x0F);
        if(ds3231_reg_table.reg[I2C_DS3231_REG_A1_HOUR_ADDR] & (0b01000000)  && 
        (ds3231_reg_table.reg[I2C_DS3231_REG_A1_HOUR_ADDR] & (0b00100000))) //12hour and after noon
        {
            *return_value += 10*((ds3231_reg_table.reg[I2C_DS3231_REG_A1_HOUR_ADDR]&0b00010000) >> 4);
            if(*return_value == (uint16_t)24){*return_value = 0;}
        }
        else // 24hour and 12hour morning
        {
            *return_value += 10*((ds3231_reg_table.reg[I2C_DS3231_REG_A1_HOUR_ADDR]&0b00110000) >> 4);
        }
        break;
    case I2C_DS3231_REG_A1_DAY_DATE:
        /* ignore A1M4 */
        if(ds3231_reg_table.reg[I2C_DS3231_REG_A1_DAY_DATE_ADDR] & 0b01000000) // Day mode 1-7
        {
            *return_value = ds3231_reg_table.reg[I2C_DS3231_REG_A1_DAY_DATE_ADDR] & 0x0F;
        }
        else //date mode 1-31
        {
            *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_A1_DAY_DATE_ADDR] & 0x0F) + 
            10*((ds3231_reg_table.reg[I2C_DS3231_REG_A1_DAY_DATE_ADDR] & (0x3F)) >> 4);
        }
        break;
    case I2C_DS3231_REG_A2_MIN:
        /* ignore A2M2 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_A2_MIN_ADDR] & 0x0F) + 
        10*((ds3231_reg_table.reg[I2C_DS3231_REG_A2_MIN_ADDR] & (0x7F)) >> 4);
        break;
    case I2C_DS3231_REG_A2_HOUR:
        /* ignore A2M3 */
        *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_A2_HOUR_ADDR] & 0x0F);
        if(ds3231_reg_table.reg[I2C_DS3231_REG_A2_HOUR_ADDR] & (0b01000000)  && 
        (ds3231_reg_table.reg[I2C_DS3231_REG_A2_HOUR_ADDR] & (0b00100000))) //12hour and after noon
        {
            *return_value += 10*((ds3231_reg_table.reg[I2C_DS3231_REG_A2_HOUR_ADDR]&0b00010000) >> 4);
            if(*return_value == (uint16_t)24){*return_value = 0;}
        }
        else // 24hour and 12hour morning
        {
            *return_value += 10*((ds3231_reg_table.reg[I2C_DS3231_REG_A2_HOUR_ADDR]&0b00110000) >> 4);
        }
        break;
    case I2C_DS3231_REG_A2_DAY_DATE:
        /* ignore A1M4 */
        if(ds3231_reg_table.reg[I2C_DS3231_REG_A2_DAY_DATE_ADDR] & 0b01000000) // Day mode 1-7
        {
            *return_value = ds3231_reg_table.reg[I2C_DS3231_REG_A2_DAY_DATE_ADDR] & 0x0F;
        }
        else //date mode 1-31
        {
            *return_value = (ds3231_reg_table.reg[I2C_DS3231_REG_A2_DAY_DATE_ADDR] & 0x0F) + 
            10*((ds3231_reg_table.reg[I2C_DS3231_REG_A2_DAY_DATE_ADDR] & (0x3F)) >> 4);
        }
        break;
    default:
        // other value
        return 1;
    }
    // ok
    return 0;
}

// --------------------Write time functions----------------------

/**
 * @brief Write data decimal to ds3231 truct register table (ignore control, control/status)
 * 
 * @note Input decimal, and type of time from I2C_DS3231_REG_SEC to I2C_DS3231_REG_A2_DAY_DATE
 * 
 * @param type_time from I2C_DS3231_REG_SEC to I2C_DS3231_REG_A2_DAY_DATE
 * 
 * @retval 0 if ok, !0 if something wrong like : type_time not available
 */
uint8_t i2c_ds3231_set_time_to_reg_table(ds3231_reg_t* reg_table, uint16_t type_time, uint16_t dec_value)
{
    if (!reg_table) return 1;

    uint8_t bcd = ((dec_value / 10) << 4) | (dec_value % 10);

    switch (type_time) {
        case I2C_DS3231_REG_SEC:
            reg_table->reg[I2C_DS3231_REG_SEC_ADDR] = bcd;
            break;
        case I2C_DS3231_REG_MIN:
            reg_table->reg[I2C_DS3231_REG_MIN_ADDR] = bcd;
            break;
        case I2C_DS3231_REG_HOUR:
            // Xử lý 24h, không hỗ trợ 12h AM/PM ở đây
            reg_table->reg[I2C_DS3231_REG_HOUR_ADDR] = bcd;
            break;
        case I2C_DS3231_REG_DAY:
            reg_table->reg[I2C_DS3231_REG_DAY_ADDR] = dec_value & 0x07;
            break;
        case I2C_DS3231_REG_DATE:
            reg_table->reg[I2C_DS3231_REG_DATE_ADDR] = bcd;
            break;
        case I2C_DS3231_REG_MONTH:
            reg_table->reg[I2C_DS3231_REG_MONTH_CENT_ADDR] = (reg_table->reg[I2C_DS3231_REG_MONTH_CENT_ADDR] & 0xE0) | (bcd & 0x1F);
            break;
        case I2C_DS3231_REG_YEAR:
        {
            uint8_t year_val;
            if (dec_value >= 2000) {
                year_val = dec_value - 2000;
                // set bit 7 (century) của thanh ghi tháng
                reg_table->reg[I2C_DS3231_REG_MONTH_CENT_ADDR] |= 0x80;
            } else {
                year_val = dec_value - 1900;
                // clear bit 7 (century) của thanh ghi tháng
                reg_table->reg[I2C_DS3231_REG_MONTH_CENT_ADDR] &= ~0x80;
            }
            reg_table->reg[I2C_DS3231_REG_YEAR_ADDR] = ((year_val / 10) << 4) | (year_val % 10);
            break;
        }
        // Có thể bổ sung các trường hợp khác nếu cần
        default:
            return 2;
    }
    return 0;
}


/**
 * @brief Write data to ds3231
 * 
 * @note Using OR operator to select expected registers to write
 * @note Example (I2C_DS3231_REG_SEC | I2C_DS3231_REG_MIN) to write second and minute
 * 
 * @param request Each bit of type uint16_t corresponds to a flag that selects type of the 16 registers above
 * @retval 0 if ok, !0 if something wrong like : time out, can't alloc
 */
uint8_t i2c_ds3231_write_full_reg(ds3231_reg_t* reg_table)
{
    if (!reg_table) return 1;

    i2c_master_list_t send_reg_table;
    send_reg_table.list_size = I2C_DS3231_MAX_ADDRESS + 1; // 1 byte address + 16 byte data
    send_reg_table.list_ptr = (uint8_t*)calloc(send_reg_table.list_size, sizeof(uint8_t));
    if (!send_reg_table.list_ptr) return 2;

    send_reg_table.list_ptr[0] = I2C_DS3231_REG_SEC_ADDR; // start write byte
    memcpy(&send_reg_table.list_ptr[1], reg_table->reg, I2C_DS3231_MAX_ADDRESS);

    uint8_t ret = 0;
    if (i2c_master_send_command_to_7bit_addr(I2C_DS3231_UNIQUE_ADDRESS, send_reg_table, I2C_DS3231_READ_DATA_TIMEOUT)
        == I2C_MASTER_SEND_COMMAND_FAILED) {
        ret = 3;
    }

    free(send_reg_table.list_ptr);
    return ret;
}



// --------------------Control square wave functions----------------------
// Will disable square wave for using alarm mode

// --------------------Control Alarm functions----------------------
// We use default when powered by Vcc - this suit table with alarm mode
// Control register :
// - bit[7:3] = 0 == Vbat mode maintain oscillation [7] and disable SQW [6], no update temperature [5]
// - bit[2] = 1 == interrupt alarm mode
// - bit[1] = 0 disable alarm 2, bit[0] = 0 disable alarm 1

// Control/status register:
// - bit[7] : status of Vbat mode osillator
// - bit[6:4]: always 0
// - bit[3]  = 1 so enable 32kHz out of pin 32K, set 0 if not using this source
// - bit[2:1] is flag for bit[5] and bit[1:0]


// set thời gian và các bit chế độ báo thức trên các thanh thời gian và kích hoạt bit [1:0]
/**
 * @brief Enable or disable alarm
 * 
 * @note Enable: clear alarm flag (status), set alarm enable bit (control)
 * @note Disable: clear alarm flag (status), clear alarm enable bit (control)
 * @note Macro: 4 macro form I2C_DS3231_NO_ALARM to I2C_DS3231_ALARM_2_ONLY
 * 
 * @param alarm_num see alarm macro
 * 
 * @retval 0 if ok, !0 if something wrong like : time out, can't alloc
 */
uint8_t i2c_ds3231_enable_disable_alarm(uint8_t alarm_num)
{
    return 0;
}

/**
 * @brief check alarm flag
 * 
 * @note Check: alarm flag
 * @note alarm_flag return: 0 (no flag set), 1 (flag 1 is set), 2 (flag 2 is set), 3 (both flag are set)
 * @param alarm_n_flag create a uint8_t then input it pointer
 * 
 * @retval 0 if ok, !0 if something wrong like : time out, can't alloc
 */
uint8_t i2c_ds3231_check_alarm_status(uint8_t* alarm_flag)
{
    return 0;
}


