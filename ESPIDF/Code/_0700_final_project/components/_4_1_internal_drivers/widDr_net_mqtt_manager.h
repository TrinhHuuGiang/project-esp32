/// Function
// 
// - 1 driver runtime and check 2 container
//      + container "get mqtt" , node node_mqtt_recv_mes_t 
//          + compare topic message get from broker, update it into container if exist
//          + set flag "read_f" to notify "new message"
//      + container "send mqtt", node node_mqtt_send_mes_t
//          + loop check new order "sent_f" then handle send by topic then clear when success
//          otherwhise continue check other node
//      + maintain: resubcribe topic 
// - API handle check data get from broker in containter "get mqtt"
//      - register a slot and get pointer to slot, then subcribe with broker
//      - check flag "new message" then get data and clear flag otherwise ignore 
//  
// - API handle set data want send to broker in container "sent mqtt"
//      - register a slot and get pointer to slot
//      - write new message on pointer "pointer use instead of topic, topic only handle by driver"
//          then set flag "sent_f"
//
// - Note: not supply a API delete slot, container only get
//      bigger when register new timer
//
// - Advantage: Prevent heap fragmentation
// - Disadvantage: 
//      The container can get big and slow down because it uses linked list traversal.
//      When there is a new update from the system, you have to reset to reload from the beginning.
//
// - Note: To simplify the system I will not provide api to modify the file system
//      So after reset all container slot will clear
//
// - QOS i will set default is QOS2
// - Recover mode: retain message == true and re subcribe topic after time setup 



// race condition problem:
//  - receive container:
//          + when new message not clear
//          + driver can write new message, it can free, alloc new message
//          + but client can read in this time -> read wrong message, read deleted address
//  - send container:
//          + when new sent not clear
//          + driver can read message deleted
//          + client can write new message, free oldmessage, alloc new message (strdup)
//
// Solution: mutex lock access 
//      + 1. mutex for each slot: too many mutexes, faster but heavy memory
//      + 2. mutex for each container: ok but increase queing delay (task and api take mutex)
//  -> solution 2

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

#ifndef _WIDDR_NET_MQTT_MANAGER_H_
#define _WIDDR_NET_MQTT_MANAGER_H_


//c
#include <stdlib.h> // Dynamic allocation
#include <stdint.h> // type int
#include <string.h>
#include <stdbool.h>

// esp32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

// user define
#include "_peripherals_err.h"
#include "__task_sync.h"
#include "_priority.h"
#include "container.h"


#include "mqtt_pubsub_handle.h"


// refer lib
#include "utlist.h"

#define WIDDR_NET_MQTT_TASK_STACK  TASK_STACK_SIZE_HIGH
                                    // handle mqtt task, manage 2 container linked list
#define WIDDR_NET_MQTT_TASK_PRIO   TASK_PRIO_IMPORTANT
#define WIDDR_NET_MQTT_TASK_SLEEP  500 // 500ms, cooldown cpu, reduce if lots of request

// QOS

#define WIDDR_NET_MQTT_SEND_MES_QOS      MQTT_PUBSUB_QOS2  // set subcriber QOS2
#define WIDDR_NET_MQTT_SEND_MES_RETAIN   MQTT_PUBSUB_RETAIN_MSG

#define WIDDR_NET_MQTT_RECV_MES_QOS      MQTT_PUBSUB_QOS2  // set broker maximum QOS2

#define WIDDR_NET_MQTT_RE_SUBCRIBE_TIME  120000 // re subcribe after 2 minute

#define WIDDR_NET_MQTT_RE_SUBCRIBE_COUNTDOWN  \
                ((int)((float)WIDDR_NET_MQTT_RE_SUBCRIBE_TIME / WIDDR_NET_MQTT_TASK_SLEEP))
                                    // re subcribe after 1 minute
                    
#define WIDDR_NET_MQTT_TRY_RECONNECT_TIME_DELAY 2000 // 2s then reconnect if lost connect
                    

/**
 * **********************************************************
 * APIS
 * **********************************************************
 */

/**
 * @brief Start the MQTT manager task, return !0 is fail
 * @note only call 1 time  and always init
 * */ 
uint8_t widDr_net_mqtt_manager_start_task(void);


/**
 * @brief Regist slot to receive data from a topic
 * @note Do not supply a function unregist slot, restart chip instead
 * 
 * @return NULL if fail (calloc fail)
 *  */ 
node_mqtt_recv_mes_t* widDr_net_mqtt_manager_register_receive_slot(const char* topic);




/**
 * @brief Check and get message from a slot, return 1 if new message, 0 if nothing
 * @param out_message return a copy of message -> free by hand else leak memory
 * 
 * 
 * @retval true if a new_message then have a copy
 * @retval false if not have a new message/ or you input slot unavailable
 */
bool widDr_net_mqtt_manager_check_and_get_message(node_mqtt_recv_mes_t* slot, char** out_message);




/**
 * @brief Register slot to send message to a topic
 * @note Do not supply a function unregist slot, restart chip instead
 * @return NULL if fail (calloc fail), then get new slot
 */
node_mqtt_send_mes_t* widDr_net_mqtt_manager_register_send_slot(const char* topic);




/**
 * @brief Write and set send flag
 * 
 * 
 * @retval true is success, other false (may be input slot unavailable)
 *  */ 
bool widDr_net_mqtt_manager_write_and_request_send(node_mqtt_send_mes_t* slot, const char* message);

#endif
