/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "mqtt_pubsub_handle.h"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */
extern __task_sync_t* g_task_sync_tools; // get sync tools 

static mqtt_client_manager_t* s_mqtt_client_manager = NULL;

/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */

static void mqtt_client_handle_mqtt_event_handler
(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */


// =================================== Init mqtt funtions          ===================================
// =================================== ==========================  ===================================

// initial client before start mqtt
_peripherals_err_t mqtt_client_handle_client_init(const char* broker_uri, const char* cert_arr_string, int cert_len_byte,
const char* username, const char* password)
{
    // create struct manager
    s_mqtt_client_manager = calloc(1, sizeof(mqtt_client_manager_t));

    if(s_mqtt_client_manager == NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_ALLOC_CLIENT_MANAGER_FAILED, __FILE__, __LINE__, "Alloc mqtt client manager failed");
        #endif
        return MQTT_CLIENT_ALLOC_CLIENT_MANAGER_FAILED;
    }

    // init client
    esp_mqtt_client_config_t mqtt_client_config =
    {
        .uri = broker_uri,              // << shouldn't alloc heap :v left for system handle it
        .cert_pem = cert_arr_string,    // PEM string format or DER array format. Note PEM require '\0' at the end
        .cert_len = cert_len_byte,      // if DER format we need size of cert because strlen can't distinguish and think 0x00 is NULL 
                                        // else write 0 if PEM format
        .username = username,
        .password = password,
        .client_id = NULL,               // default using 3byte last of MAC address
        .disable_clean_session = false   // false -> always clean session when disconnect 
                                        // true -> keep session when disconnect -> warn leak memory broker if change another client_id
                                        // session == subcribe topic, publish data, etc
                                        
    };

    s_mqtt_client_manager->client_handle  = esp_mqtt_client_init(&mqtt_client_config);

    if(s_mqtt_client_manager->client_handle == NULL)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_CLIENT_CONFIG_FAILED, __FILE__, __LINE__, "mqtt init return NULL");
        #endif

        // free manager and return
        free(s_mqtt_client_manager);
        s_mqtt_client_manager = NULL;
        return MQTT_CLIENT_CLIENT_CONFIG_FAILED;
    }

    // ok
    return PERIPH_OK;
}

// register receive event from default event loop
static void mqtt_client_handle_mqtt_event_handler
(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    // check client manager struct
    if(s_mqtt_client_manager == NULL)
    {
        return;
    }

    // check event
    switch (event_id)
    {
    case MQTT_EVENT_BEFORE_CONNECT:
        // dont care
        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);

        break;

    case MQTT_EVENT_CONNECTED:
        // connected to broker server
        // started mqtt client
        SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_STARTED);
        SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTED);

        // clear connecting state
        CLR_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTING);
        
        break;
    case MQTT_EVENT_DISCONNECTED:
        // clear connected state
        CLR_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTED);
        
        // clear disconnecting state
        CLR_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_DISCONNECTING);

        break;
    
    case MQTT_EVENT_SUBSCRIBED:
        // subcribed topic
        SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_SUBCRIBED);

        // clear subcribing state
        CLR_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_SUBCRIBING);
        
        break;
    
    case MQTT_EVENT_UNSUBSCRIBED:
        // clear un subcribing state
        CLR_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_UN_SUBCRIBING);
        
        break;
    
    case MQTT_EVENT_PUBLISHED:
        // clear publishing state
        CLR_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_PUBLISHING);
        
        break;
    
    case MQTT_EVENT_DATA:
        // new data from topic
        SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_DATA_COME);

        // queing data to linked list
        mqtt_data_node_t* new_data = calloc(1, sizeof(mqtt_data_node_t));

        if(new_data == NULL)
        {
            #if CONFIG_DEBUG_ENABLE !=0
            send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "Failed alloc new mqtt data node");
            #endif
            break;
        }

        // copy topic
        esp_mqtt_event_handle_t event = event_data;
        new_data->topic = calloc(event->topic_len + 1, sizeof(char));

        if(new_data->topic == NULL)
        {
            free(new_data);
            break;
        }

        memcpy(new_data->topic, event->topic, event->topic_len);
        new_data->topic_len = event->topic_len;

        // copy data
        new_data->data = calloc(event->data_len + 1, sizeof(char));
        if(new_data->data == NULL)
        {
            free(new_data->topic);
            free(new_data);
            break;
        }
        memcpy(new_data->data, event->data, event->data_len);
        new_data->data_len = event->data_len;

        // add to head of linked list
        LL_PREPEND(s_mqtt_client_manager->incoming_data_list, new_data);

        break;
    
    case MQTT_EVENT_ERROR:
        // error message
        SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_ERROR);
        
        // :) comming soon

        break;
    
    default:
        // don't care
        break;
    }
    
    // unlock after done task
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
}

_peripherals_err_t mqtt_client_handle_regist_receive_event_task()
{
    esp_err_t ret = esp_mqtt_client_register_event(s_mqtt_client_manager->client_handle,
        ESP_EVENT_ANY_ID,
        mqtt_client_handle_mqtt_event_handler,
        NULL);
    
    // error
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_CLIENT_REGIST_EVENT_TASK_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MQTT_CLIENT_CLIENT_REGIST_EVENT_TASK_FAILED;
    }

    // ok
    return PERIPH_OK;
}



// start communicate mqtt from esp client to broker server
_peripherals_err_t mqtt_client_handle_client_start()
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    // check if connecting state / started state
    if(GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTING) ||
        GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_STARTED))
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "MQTT client start failed");
        #endif

        goto return_ok; // already connecting/started state
    }

    // set state connecting
    SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTING);

    // start mqtt client
    esp_err_t ret = esp_mqtt_client_start(s_mqtt_client_manager->client_handle);

    // error
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_CLIENT_START_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif

        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return MQTT_CLIENT_CLIENT_START_FAILED;
    }

    // ok
return_ok:
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return PERIPH_OK;
}






// =================================== Handle event mqtt funtions  ===================================
// =================================== ==========================  ===================================
//  
_peripherals_err_t mqtt_client_handle_client_reconnect()
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    // check if connecting / started / connected state
    if(GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTING) ||
        GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_STARTED) ||
        GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTED))
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "MQTT client reconnect failed");
        #endif

        goto return_ok; // already connecting/started/connected state
    }
    

    // set state connecting
    SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTING);

    // reconnect mqtt client
    esp_err_t ret = esp_mqtt_client_reconnect(s_mqtt_client_manager->client_handle);

    // error
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_CLIENT_RECONNECT_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif

        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return MQTT_CLIENT_CLIENT_RECONNECT_FAILED;
    }

    // ok
return_ok:
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return PERIPH_OK;
}



_peripherals_err_t mqtt_client_handle_client_disconnect()
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    // check if disconnecting / started / connected state
    if(GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_DISCONNECTING) ||
        GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_STARTED)      ||
        GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTING)   ||
        GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTED))
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "MQTT client disconnect failed");
        #endif

        goto return_ok; // already disconnecting/started/connected state
    }

    // set state disconnecting
    SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_DISCONNECTING);

    // disconnect mqtt client
    esp_err_t ret = esp_mqtt_client_disconnect(s_mqtt_client_manager->client_handle);

    // error
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_CLIENT_DISCONNECT_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif

        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return MQTT_CLIENT_CLIENT_DISCONNECT_FAILED;
    }

    // ok
return_ok:
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return PERIPH_OK;
}


int mqtt_client_handle_client_subcribe_topic(const char* topic, mqtt_qos_type_t qos)
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    // check if subcribing state
    if(GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_SUBCRIBING))
    {
        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return 1; // subcribing state
    }

    // set state subcribing
    SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_SUBCRIBING);

    // subcribe topic
    // qos = 0, 1, 2
    // get new message id subcribe
    s_mqtt_client_manager->msg_id_sub = esp_mqtt_client_subscribe(s_mqtt_client_manager->client_handle, topic, qos);

    // if failed return -1
    if(s_mqtt_client_manager->msg_id_sub == -1)
    {
        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return 2; // failed to subcribe topic
    }

    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return 0; // ok, subcribe topic
}


int mqtt_client_handle_client_publish_data(const char* topic, mqtt_qos_type_t qos,
    const char* data, int len, mqtt_retain_type_t retain)
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    // check if publishing state
    if(GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_PUBLISHING))
    {
        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return 1; // publishing state
    }
    
    // set state publishing
    SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_PUBLISHING);

    // publish data
    // qos = 0, 1, 2
    s_mqtt_client_manager->msg_id_pub = 
        esp_mqtt_client_publish(s_mqtt_client_manager->client_handle, topic, data, len, qos, retain);

    // if failed return -1
    if(s_mqtt_client_manager->msg_id_pub == -1)
    {
        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return 2; // failed to publish data
    }

    // ok
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return 0;
}


int mqtt_client_handle_client_un_subcribe_topic(const char* topic)
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    // check if un subcribing state
    if(GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_UN_SUBCRIBING))
    {
        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return 1; // un subcribing state
    }

    // set state un subcribing
    SET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_UN_SUBCRIBING);

    // un subcribe topic
    s_mqtt_client_manager->msg_id_unsub = 
        esp_mqtt_client_unsubscribe(s_mqtt_client_manager->client_handle, topic);

    
    // if failed return -1
    if(s_mqtt_client_manager->msg_id_unsub == -1)
    {
        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return 2; // failed to un subcribe topic
    }

    // ok
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return 0;
}



mqtt_data_node_t* mqtt_client_handle_read_next_data()
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    if((s_mqtt_client_manager == NULL) || (s_mqtt_client_manager->incoming_data_list == NULL))
    {
        xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
        return NULL;
    }

    // lấy node đầu tiên
    mqtt_data_node_t* data_node = s_mqtt_client_manager->incoming_data_list;

    // bỏ ra khỏi linked list
    LL_DELETE(s_mqtt_client_manager->incoming_data_list, data_node);

    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);

    return data_node; // trả ra node, user phải free sau khi dùng xong
}


// =================================== De init      mqtt funtions  ===================================
// =================================== ==========================  ===================================


// this function is blocking until stop is done or error
_peripherals_err_t mqtt_client_handle_client_stop()
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    // check if disconnecting / started / connected state
    if( GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_DISCONNECTING) ||
        ! GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_STARTED)     ||
        GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTING)    ||
        GET_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_CONNECTED))
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(PERIPH_OK, __FILE__, __LINE__, "MQTT client stop failed");
        #endif

        goto return_ok; // already disconnecting/started/connected state
    }

    esp_err_t ret = esp_mqtt_client_stop(s_mqtt_client_manager->client_handle);

    // error
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_CLIENT_STOP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MQTT_CLIENT_CLIENT_STOP_FAILED;
    }

    // clear state started
    CLR_BIT(s_mqtt_client_manager->mqtt_client_state, MQTT_PUBSUB_FLAG_STATE_STARTED);

    // ok
return_ok:
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return PERIPH_OK;
}



_peripherals_err_t mqtt_client_handle_un_regist_receive_event_task()
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);
    
    // esp no supply a function for unregister event loop, but in funtion register , esp32 simple using function
    // esp_event_handler_register_with() so we unregister with function below

    esp_err_t ret = esp_event_handler_unregister(MQTT_EVENTS, ESP_EVENT_ANY_ID, s_mqtt_client_manager->client_handle);

    // error
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_CLIENT_UN_REGIST_EVENT_TASK_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MQTT_CLIENT_CLIENT_UN_REGIST_EVENT_TASK_FAILED;
    }

    // ok
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return PERIPH_OK;
}



_peripherals_err_t mqtt_client_handle_client_de_init()
{
    xSemaphoreTake(g_task_sync_tools->mqtt_state_mutex, portMAX_DELAY);

    esp_err_t ret =  esp_mqtt_client_destroy(s_mqtt_client_manager->client_handle);

    // error
    if(ret!=ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(MQTT_CLIENT_CLIENT_DESTROY_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return MQTT_CLIENT_CLIENT_DESTROY_FAILED;
    }

    
    // free all incoming data list
    mqtt_data_node_t *current, *tmp;
    LL_FOREACH_SAFE(s_mqtt_client_manager->incoming_data_list, current, tmp)
    {
        LL_DELETE(s_mqtt_client_manager->incoming_data_list, current);
        if(current->topic) free(current->topic);
        if(current->data) free(current->data);
        free(current);
    }

    // free manager and return
    free(s_mqtt_client_manager);
    s_mqtt_client_manager = NULL;

    // ok
    xSemaphoreGive(g_task_sync_tools->mqtt_state_mutex);
    return PERIPH_OK;

}
