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


#ifndef _SYSDR_HTTP_UPDATE_PATH_H_
#define _SYSDR_HTTP_UPDATE_PATH_H_


/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

// check connection
#define HTTP_PATH_GET_BOARD_TYPE "/get_board_type"


// update logic block
#define HTTP_PATH_POST_JSON_CONFIG_LOGIC_BLOCK "/post_json_config_lgb"


// update MQTT config 
#define HTTP_PATH_POST_JSON_CONFIG_MQTT "/post_json_config_mqtt"


// update wifi AP config 
#define HTTP_PATH_POST_JSON_CONFIG_WIFI_AP "/post_json_config_ap"


// update wifi STA config 
#define HTTP_PATH_POST_JSON_CONFIG_WIFI_STA "/post_json_config_sta"



#endif