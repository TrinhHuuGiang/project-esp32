/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "_peripherals_err.h"


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */
// reverse byte 32
uint32_t reverse_bytes_32(uint32_t value) {
    return ((value >> 24) & 0x000000FF) |
           ((value >> 8)  & 0x0000FF00) |
           ((value << 8)  & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
}
// reverse bit 32
uint32_t reverse_bits_32(uint32_t value)
{
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result <<= 1;
        result |= (value >> i) & 1;
    }
    return result;
}


// print err
// ESP_LOGE (err), ESP_LOGW (warn), ESP_LOGI (info), ESP_LOGD (debug), ESP_LOGV (verbose)
void send_peripheral_err_location(_peripherals_err_t code, char* file, int line,const char* comment)
{
    if(code == PERIPH_OK)
    {
        ESP_LOGW("PHERIPH","\n[Err code - %d] [%s] [%d] [%s]\n", code, file, line, comment);
    }
    else
    {
        ESP_LOGW("PHERIPH","\n[Err code - %d] [%s] [%d] [%s]\n", code, file, line, comment);
    }
}