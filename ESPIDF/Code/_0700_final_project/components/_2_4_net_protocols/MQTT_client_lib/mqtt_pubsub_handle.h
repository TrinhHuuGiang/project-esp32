// althought we can create lot of mqtt client at a time
// this library only subport 1 client

// Note: 
// + Disable clean session == broker auto unsubcribe topic when disconnect
// + Enable clean session  == broker auto keep session when disconnect

// + QoS 0 == no guarantee message delivery
// + QoS 1 == at least once delivery
// + QoS 2 == exactly once delivery

#ifndef _MQTT_PUBSUB_HANDLE_H_
#define _MQTT_PUBSUB_HANDLE_H_

/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */

//c
#include <stdlib.h> // Dynamic allocation
#include <stdint.h> // type int
#include <string.h>
#include <stdbool.h>

//esp32
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#define MQTT_SUPPORTED_FEATURE_EVENT_LOOP // before use mqtt_client.h define it to use event loops
#include "mqtt_client.h"

//user
#include "_peripherals_err.h"
#include "__task_sync.h"

#include "utlist.h" // linked list


// list for data come
typedef struct mqtt_data_node_t
{
    char* topic;
    char* data;
    int topic_len;
    int data_len;
    struct mqtt_data_node_t* next;
} mqtt_data_node_t;


// struct manager client
typedef struct
{
    // mqtt client handle
    esp_mqtt_client_handle_t client_handle;
    
    // message id
    int msg_id_sub;
    int msg_id_unsub;
    int msg_id_pub;

    // linked list for data come
    mqtt_data_node_t* incoming_data_list;

    // register state
    uint32_t mqtt_client_state;
    
} mqtt_client_manager_t;

typedef enum 
{
    MQTT_PUBSUB_FLAG_STATE_STARTED = 0,    // started/ stopped

    MQTT_PUBSUB_FLAG_STATE_CONNECTING,  // starting/ reconnecting
    MQTT_PUBSUB_FLAG_STATE_CONNECTED,   // connected/ disconnected

    MQTT_PUBSUB_FLAG_STATE_SUBCRIBING,  // subcribing topic

    MQTT_PUBSUB_FLAG_STATE_UN_SUBCRIBING, // unsubcribing topic

    MQTT_PUBSUB_FLAG_STATE_PUBLISHING,    // publishing data

    MQTT_PUBSUB_FLAG_STATE_DATA_COME,     // new data from topic

    MQTT_PUBSUB_FLAG_STATE_ERROR,         // error message

    MQTT_PUBSUB_FLAG_STATE_DISCONNECTING  // disconnecting
    
} mqtt_flag_state_t;


typedef enum
{
    MQTT_PUBSUB_QOS0 = 0, // send without response
    MQTT_PUBSUB_QOS1,     // send untill broker feedback
    MQTT_PUBSUB_QOS2
} mqtt_qos_type_t;


typedef enum
{
    MQTT_PUBSUB_NO_RETAIN_MSG = 0,
    MQTT_PUBSUB_RETAIN_MSG
} mqtt_retain_type_t;


// user can know state machine is busy
typedef enum
{
    MQTT_CLIENT_COMMAND_REFUSED = 0,
    MQTT_CLIENT_COMMAND_EXECUTED = 1,
} mqtt_client_command_state_t;

/**
 * **********************************************************
 * APIs
 * **********************************************************
 */

// =================================== Init mqtt funtions          ===================================
// =================================== ==========================  ===================================
// initial client before start mqtt
_peripherals_err_t mqtt_client_handle_client_init(const char* broker_uri, const char* cert_arr_string, int cert_len_byte,
    const char* username, const char* password);


// register receive event from default event loop
_peripherals_err_t mqtt_client_handle_regist_receive_event_task();



// start communicate mqtt from esp client to broker server
_peripherals_err_t mqtt_client_handle_client_start(mqtt_client_command_state_t* busy);




// =================================== Handle event mqtt funtions  ===================================
// =================================== ==========================  ===================================
//  
_peripherals_err_t mqtt_client_handle_client_reconnect(mqtt_client_command_state_t* busy);



_peripherals_err_t mqtt_client_handle_client_disconnect(mqtt_client_command_state_t* busy);


int mqtt_client_handle_client_subcribe_topic(const char* topic, mqtt_qos_type_t qos);


int mqtt_client_handle_client_publish_data(const char* topic, mqtt_qos_type_t qos,
    const char* data, int len, mqtt_retain_type_t retain);


int mqtt_client_handle_client_un_subcribe_topic(const char* topic);

// read data from linked list
// user free data after use
// return NULL if no data in list
mqtt_data_node_t* mqtt_client_handle_read_next_data();


// =================================== De init      mqtt funtions  ===================================
// =================================== ==========================  ===================================



_peripherals_err_t mqtt_client_handle_client_stop(mqtt_client_command_state_t* busy);

_peripherals_err_t mqtt_client_handle_un_regist_receive_event_task();

_peripherals_err_t mqtt_client_handle_client_de_init();

#endif