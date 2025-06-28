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
#include "_peripherals_err.h"


 /**
 * **********************************************************
 * Codes
 * **********************************************************
 */
// print err
// ESP_LOGE (err), ESP_LOGW (warn), ESP_LOGI (info), ESP_LOGD (debug), ESP_LOGV (verbose)
void send_peripheral_err_location(_peripherals_err_t code, char* file, int line,const char* comment)
{
    if(code == PERIPH_OK)
    {
        ESP_LOGD("PHERIPH","\n[Err code - %d] [%s] [%d] [%s]\n", code, file, line, comment);
    }
    else
    {
        ESP_LOGW("PHERIPH","\n[Err code - %d] [%s] [%d] [%s]\n", code, file, line, comment);
    }
}