/*
 * Copyright (C) 2025 Giang Trinh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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