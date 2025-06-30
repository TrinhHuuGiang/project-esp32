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
#include "Deploy_LB.h"



/**
 * **********************************************************
 * Variables
 * - const global in ESP32 keep table in flash
 *          otherwise if miss const table will copy to data area in ram
 * - include : logic block funtion pointer table, logic block code table
 * **********************************************************
 */

const logic_block_init_fptr logic_block_init_table[]=
{
    // test lb driver ========================================





    // default service lb driver =============================
    logic_block_test_get_battery_voltage,
    logic_block_test_play_default_audio

};




const char* logic_block_code_table[]=
{
    // test lb code   ========================================




    // default service lb code ================================
    LB_TEST_CODE_BATTERY_CHECK,
    LB_TEST_CODE_AUDIO_TEST
};






/**
 * **********************************************************
 * Definitions design general port
 * **********************************************************
 */







/**
 * **********************************************************
 * Code designed device // widget
 * **********************************************************
 */
/**
 * @brief this APIS only call 1 time after reset
 * 
 * @return !0 if failed
 * 
 *  */ 
uint8_t deploy_logic_block_from_file_config()
{
    // count number logic block
    const uint32_t logic_block_count = sizeof(logic_block_code_table) / sizeof(char*);

    

    return 0;
}
