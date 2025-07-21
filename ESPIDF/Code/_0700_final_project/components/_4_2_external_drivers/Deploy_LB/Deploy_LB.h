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

#ifndef _DEPLOY_LOGIC_BLOCK_H_
#define _DEPLOY_LOGIC_BLOCK_H_


// read and parse "/lgb_init.dat" by cJSON
// then implement logic block exist here
//      = by send a copy config cJSON to Logic block by each init logic block api
//      = this only an API read and send to destination, then delete main cJSON from root
//          logic block driver will handle inform

/**
 * **********************************************************
 * Libraries
 * **********************************************************
 */

// --------------> C common libs
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


// --------------> C & POSIX file system
#include <stdio.h>    // file:   interract with file
#include <sys/stat.h> // folder: check folder info
                        // mkdir: make new folder
#include <unistd.h>   // folder: delete empty folder 
#include <cJSON.h>    // parse json file

// --------------> esp libs
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//user define
#include "utlist.h"
#include "_priority.h"


#define PATH_TO_FILE_CONFIG_LOGIC_BLOCK  "/sd_card0/sys_fd/lgb_init.dat"


/**
 * **********************************************************
 * Deployable Logic block
 * **********************************************************
 */


// TEST
#include "Logic_Test_design_port.h"


// MAIN DESIGN



/**
 * **********************************************************
 * Structure to follow when writing logic block driver
 * **********************************************************
 */

// 1. init drivers have to follow this function pointer format

    typedef uint8_t (*logic_block_init_fptr)(cJSON* config);

    // example: uint8_t Logic_block_test_init(cJSON* config);

// 2. register with array function pointer in "Deloy_LB.c" 
//      the initialization driver function pointer


// 3. Register in the correct "logic block code" order in the "logic_block_code" table 
//      because it is only inform help APIs check what driver will init when parse config file

// note:
//  + all the init functions have  to run itself in 
//      parallel task and return error code if failed
//  + all the init functions have to set global error flag !0 if get error
//      to notify main loop handle in the future


/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

/**
 * @brief this APIS only call 1 time after reset
 * 
 * @return !0 if failed
 * 
 *  */ 
uint8_t deploy_logic_block_from_file_config();







#endif