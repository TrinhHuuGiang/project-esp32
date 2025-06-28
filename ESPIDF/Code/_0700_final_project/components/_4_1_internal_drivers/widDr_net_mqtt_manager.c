/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
 * **********************************************************
 *  Definition
 * **********************************************************
 */
#include "widDr_net_mqtt_manager.h"

#define WIDDR_MQTT_TAG "mqtt"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */

static node_mqtt_recv_mes_t* g_recv_container = NULL; // << container receive mqtt, linked list
static node_mqtt_send_mes_t* g_send_container = NULL; // << container send mqtt, linked list

extern __task_sync_t* g_task_sync_tools; // get sync tools 


static SemaphoreHandle_t g_recv_mutex = NULL; // Mutex to protect receive container
static SemaphoreHandle_t g_send_mutex = NULL; // Mutex to protect send container


/**
 * **********************************************************
 * Prototypes
 * **********************************************************
 */
/**
 * @brief  loop: scan receive/send containers
*/ 
static void widDr_net_mqtt_manager_task(void* arg);

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

uint8_t widDr_net_mqtt_manager_start_task(void)
{

    // init mutex
    g_recv_mutex = xSemaphoreCreateMutex();
    if (!g_recv_mutex) return 1;
    g_send_mutex = xSemaphoreCreateMutex();

    if (!g_send_mutex) return 2;

    // init task
    BaseType_t check_task = xTaskCreate(
        widDr_net_mqtt_manager_task,
        "mqttManagerTask",
        WIDDR_NET_MQTT_TASK_STACK,         // Stack size
        NULL,
        WIDDR_NET_MQTT_TASK_PRIO,            // Priority
        NULL
    );

    return (check_task == pdPASS) ? 0 : 3;
}

/**
 * @brief Task loop: scan receive/send containers
 */
static void widDr_net_mqtt_manager_task(void* arg)
{
    uint8_t  err_catch = 0;

    int count_reconn = 0; // start will reconnect

    while (!err_catch)
    {

        ESP_LOGE(WIDDR_MQTT_TAG, "mqtt task loop");





        //==========================================================
        //========================== Reconnect phase======
        //Task will run before WIDDR_NET_MQTT_RE_SUBCRIBE_TIME reach
        // then reconnect and resubcribe topic (because i turn on clean session in mqtt_pubsub_handle.h)
        //    + sure that all topic subcribed
        //    + maintain connection
        //    + sleep driver when disconnect (cooldown CPU) 
        // Note: resubcribe time donot quite short, resubcribe will take
        //      amount of throughput of broker (HIVEMQ broker limit 10GB traffic a month)
        //==========================================================

        if(count_reconn > 0)
        {
            count_reconn --;
        }
        else 
        {
            // reset countdown
            count_reconn = WIDDR_NET_MQTT_RE_SUBCRIBE_COUNTDOWN;


            // try reconnect
            mqtt_client_command_state_t busy;
            while(mqtt_client_handle_client_reconnect(&busy))
            {
                vTaskDelay(WIDDR_NET_MQTT_TRY_RECONNECT_TIME_DELAY);
                ESP_LOGE(WIDDR_MQTT_TAG,"Try reconnect...");
            }


            // resubcribe all topic in container
            ESP_LOGI(WIDDR_MQTT_TAG, "Require reconnect successfully. Resubscribing topics...");
            


            // Resubscribe all topics in receive 
            
            {



                xSemaphoreTake(g_recv_mutex, portMAX_DELAY);    // <<<<<<<<<<<<<<<<<<<<<<<<<< take



                node_mqtt_recv_mes_t* slot;
                LL_FOREACH(g_recv_container, slot)
                {
                    uint8_t retry_num = 5;

                    while(mqtt_client_handle_client_subcribe_topic(slot->topic, WIDDR_NET_MQTT_RECV_MES_QOS))
                    {
                        // maybe reconnect not already success
                        // mqtt_client_handle_client_subcribe_topic is a blocking function, 
                        //      so if it return !0 -> subcribe fail maybe by not reconnect success

                        ESP_LOGE(WIDDR_MQTT_TAG,"Retry subcribe...%d",retry_num);
                        vTaskDelay(1000);

                        retry_num --;
                        if(!retry_num) // retry = 0 -> need reconnect
                        {
                            ESP_LOGE(WIDDR_MQTT_TAG,"Try reconnect...");
                            // goto mqtt_reconnnect; <== wrong solution :) stack maybe not auto delete
                            // => stack overflow inthe future if jump to reconnect here

                            count_reconn=0; // reconnect



                            xSemaphoreGive(g_recv_mutex); // <<<<<<<<<<<<<<<<<<<<<<<<<< give




                            goto mqttdriver_next_loop;// << solution jump to next loop by goto to end
                                                    // because LL_FOREACH + while is 2 loop
                        }
                    }

                }


                xSemaphoreGive(g_recv_mutex);  // <<<<<<<<<<<<<<<<<<<<<<<<<< give

            }
            ESP_LOGI(WIDDR_MQTT_TAG, "All topics resubscribed.");
        }    





        //==========================================================
        //========================== Check new msg received phase ======
        // loop check receive message container
        // + write new message by search topic
        // + free old message then strup new message
        // + set flag read_f = 1
        // + clean receive data
        //==========================================================

        // check new message
        mqtt_data_node_t* msg = mqtt_client_handle_read_next_data();
        if (msg != NULL)
        {


            xSemaphoreTake(g_recv_mutex, portMAX_DELAY); // <<<<<<<<<<<<<<<<<<<<<<<<<< take


            node_mqtt_recv_mes_t* slot;
            LL_FOREACH(g_recv_container, slot)
            {
                // :) strcmp topic is my best solution for driver, because
                // we only know where to write new message by topic
                // 
                // the user use api dont need strcmp, because they have slot pointer to right topic
                if (strcmp(slot->topic, msg->topic) == 0)
                {
                    if (slot->message) free(slot->message);
                    slot->message = strdup(msg->data);
                    slot->read_f = 1;
                    break;
                }
            }


            xSemaphoreGive(g_recv_mutex);// <<<<<<<<<<<<<<<<<<<<<<<<<< give


            free(msg->topic);
            free(msg->data);
            free(msg);
        }







        //==========================================================
        //========================== Check new send phase ======
        // loop the container and find any sent_f is set
        // publish message
        // clear when send successful (other wise try at next loop)
        //==========================================================


        xSemaphoreTake(g_send_mutex, portMAX_DELAY);

        node_mqtt_send_mes_t* s;
        LL_FOREACH(g_send_container, s)
        {
            if (s->sent_f)
            {
                int ret = mqtt_client_handle_client_publish_data(s->topic, WIDDR_NET_MQTT_SEND_MES_QOS, 
                    s->message, strlen(s->message), WIDDR_NET_MQTT_SEND_MES_RETAIN);

                if (ret == 0) s->sent_f = 0;
            }
        }


        xSemaphoreGive(g_send_mutex);


        // Sleep to yield CPU and avoid busy looping
        vTaskDelay(pdMS_TO_TICKS(WIDDR_NET_MQTT_TASK_SLEEP)); // ms delay
    
mqttdriver_next_loop:
        continue;// :)) i get a err: [ error: label at end of compound statement ] because nothing after tag
    }
    

    // some thing wrong
    g_task_sync_tools->err_flag = 1 ; // error 
    vTaskDelete(NULL);
    return;
}



// Register slot to receive data from a topic
node_mqtt_recv_mes_t* widDr_net_mqtt_manager_register_receive_slot(const char* topic)
{
    node_mqtt_recv_mes_t* slot = calloc(1, sizeof(node_mqtt_recv_mes_t));
    if (!slot) return NULL;

    slot->topic = strdup(topic);
    slot->message = NULL;
    slot->read_f = 0;


    xSemaphoreTake(g_recv_mutex, portMAX_DELAY);
    LL_APPEND(g_recv_container, slot);
    xSemaphoreGive(g_recv_mutex);


    return slot;
}



// Check and get message from a slot, return 1 if new message, 0 if nothing
bool widDr_net_mqtt_manager_check_and_get_message(node_mqtt_recv_mes_t* slot, char** out_message)
{
    bool result = false;
    
    xSemaphoreTake(g_recv_mutex, portMAX_DELAY);


    if(slot->read_f) // if new message
    {
        *out_message = strdup(slot->message);
        slot->read_f = 0;
        result = true;
    }
    
    
    xSemaphoreGive(g_recv_mutex);

    return result;

}



// Register slot to send message to a topic
node_mqtt_send_mes_t* widDr_net_mqtt_manager_register_send_slot(const char* topic)
{
    node_mqtt_send_mes_t* slot = calloc(1, sizeof(node_mqtt_send_mes_t));
    if (!slot) return NULL;

    slot->topic = strdup(topic);
    slot->message = NULL;
    slot->sent_f = 0;


    xSemaphoreTake(g_send_mutex, portMAX_DELAY);

    LL_APPEND(g_send_container, slot);


    xSemaphoreGive(g_send_mutex);

    return slot;
}



// Write and set send flag
bool widDr_net_mqtt_manager_write_and_request_send(node_mqtt_send_mes_t* slot, const char* message)
{
    // slot not available 
    if (!slot) return false;


    // update message

    xSemaphoreTake(g_send_mutex, portMAX_DELAY);

    if (slot->message) free(slot->message);
    
    slot->message = strdup(message);

    // set sent flag for driver handle
    slot->sent_f = 1;

    xSemaphoreGive(g_send_mutex);

    return true;
}