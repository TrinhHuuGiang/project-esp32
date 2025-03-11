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
// print err
void send_peripheral_err_location(_peripherals_err_t code, char* file, int line,const char* comment)
{
    fprintf(stderr, "\n[Err code - %d] [%s] [%d] [%s]\n", code, file, line, comment);
}