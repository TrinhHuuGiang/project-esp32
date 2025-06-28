/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _TASK_SYNC_PRIORITY_
#define _TASK_SYNC_PRIORITY_



// configMAX_PRIORITIES is ( 25 ), go to FreeRTOSConfig.h
// here i set some priority for easy to use
#define TASK_PRIO_NOT_IMPORTANT   0
#define TASK_PRIO_LOW             1
#define TASK_PRIO_NORMAL          2
#define TASK_PRIO_IMPORTANT       3
#define TASK_PRIO_REALTIME        4



// use uxTaskGetStackHighWaterMark(NULL) to see the rest of free task stack
// then reduce
// Stack sizes in bytes
#define TASK_STACK_SIZE_LOW       512
#define TASK_STACK_SIZE_MEDIUM    1024
#define TASK_STACK_SIZE_HIGH      2048
#define TASK_STACK_SIZE_EXTREM    4096
#define TASK_STACK_SIZE_GIANT     5120 // :) no stack now must use this

#endif