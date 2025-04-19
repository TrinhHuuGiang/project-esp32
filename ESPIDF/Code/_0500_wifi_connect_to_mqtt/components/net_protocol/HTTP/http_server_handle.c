/**
 * **********************************************************
 * Definitions
 * **********************************************************
 */
#include "http_server_handle.h"

/**
 * **********************************************************
 * Variable
 * **********************************************************
 */
static http_server_manager_t* s_http_server_manager = NULL;

/**
 * **********************************************************
 * Codes
 * **********************************************************
 */

// http server start
_peripherals_err_t http_server_handle_start_server()
{
    s_http_server_manager = calloc(1, sizeof(http_server_manager_t));

    if(s_http_server_manager == NULL)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(HTTP_SERVER_ALLOC_MANAGER_FAILED, __FILE__, __LINE__, "HTTP manager alloc failed");
        #endif
        return HTTP_SERVER_ALLOC_MANAGER_FAILED;
    }

    // http start
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    esp_err_t ret = httpd_start(&(s_http_server_manager->server_handle) , &config );

    if(ret != ESP_OK)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(HTTP_SERVER_START_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return HTTP_SERVER_START_FAILED;
    }

    // ok
    return PERIPH_OK;

}

// http linked list (uri - function handler)
_peripherals_err_t http_server_handle_append_uri_struct_to_list(const char* uri, httpd_method_t method, 
    http_uri_function_handler_t function_handler, void* user_ctx, int context_size_byte)
{
    int uri_len =  strlen(uri) + 1; // include \0
    char* temp_uri = calloc(1, uri_len + 1);

    if(temp_uri == NULL)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(HTTP_SERVER_ALLOC_URI_FAILED, __FILE__, __LINE__, "Uri alloc failed");
        #endif
        return HTTP_SERVER_ALLOC_URI_FAILED;
    }

    memcpy(temp_uri, uri, uri_len);

    void* temp_user_ctx = NULL;
    if(context_size_byte > 0)
    {
        temp_user_ctx =  calloc(context_size_byte, sizeof(uint8_t)) ;

        if(temp_user_ctx == NULL)
        {
            // check NULL -> failed
            #if CONFIG_DEBUG_ENABLE !=0
            send_peripheral_err_location(HTTP_SERVER_ALLOC_CTX_FAILED, __FILE__, __LINE__, "Context alloc failed");
            #endif

            // return
            free(temp_uri);
            return HTTP_SERVER_ALLOC_CTX_FAILED;
        }

        memcpy(temp_user_ctx,user_ctx,context_size_byte);
    }


    http_server_uri_list_t* http_uri_inf = calloc(1, sizeof(http_server_uri_list_t));
    if(http_uri_inf == NULL)
    {
        // check NULL -> failed
        #if CONFIG_DEBUG_ENABLE !=0
        send_peripheral_err_location(HTTP_SERVER_ALLOC_HTTP_URI_INF_FAILED, __FILE__, __LINE__, "Context alloc failed");
        #endif

        // return
        free(temp_uri);
        if(temp_user_ctx != NULL) free(temp_user_ctx);
        return HTTP_SERVER_ALLOC_HTTP_URI_INF_FAILED;
    }
    

    http_uri_inf->httpd_uri_inform.uri = temp_uri;
    http_uri_inf->httpd_uri_inform.method = method;
    http_uri_inf->httpd_uri_inform.handler = function_handler;
    http_uri_inf->httpd_uri_inform.user_ctx = temp_user_ctx;

    LL_PREPEND(s_http_server_manager->uri_list_registed, http_uri_inf);

    // ok
    return PERIPH_OK;
}

// http server register list uri to handler
_peripherals_err_t http_server_handle_regist_uri_list_to_handler()
{
    if (s_http_server_manager == NULL || s_http_server_manager->server_handle == NULL)
    {
        #if CONFIG_DEBUG_ENABLE != 0
        send_peripheral_err_location(HTTP_SERVER_NULL_HANDLE, __FILE__, __LINE__, "HTTP server handle is NULL");
        #endif
        return HTTP_SERVER_NULL_HANDLE;
    }

    http_server_uri_list_t* current_uri;
    LL_FOREACH(s_http_server_manager->uri_list_registed, current_uri)
    {
        esp_err_t ret = httpd_register_uri_handler(s_http_server_manager->server_handle, &(current_uri->httpd_uri_inform));
        if (ret != ESP_OK)
        {
            #if CONFIG_DEBUG_ENABLE != 0
            send_peripheral_err_location(HTTP_SERVER_REGISTER_URI_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
            #endif
            return HTTP_SERVER_REGISTER_URI_FAILED;
        }
    }

    return PERIPH_OK;
}

// http server stop
_peripherals_err_t http_server_handle_stop_server()
{
    if (s_http_server_manager == NULL || s_http_server_manager->server_handle == NULL)
    {
        #if CONFIG_DEBUG_ENABLE != 0
        send_peripheral_err_location(HTTP_SERVER_NULL_HANDLE, __FILE__, __LINE__, "HTTP server handle is NULL");
        #endif
        return HTTP_SERVER_NULL_HANDLE;
    }

    esp_err_t ret = httpd_stop(s_http_server_manager->server_handle);
    if (ret != ESP_OK)
    {
        #if CONFIG_DEBUG_ENABLE != 0
        send_peripheral_err_location(HTTP_SERVER_STOP_FAILED, __FILE__, __LINE__, esp_err_to_name(ret));
        #endif
        return HTTP_SERVER_STOP_FAILED;
    }

    // Giải phóng danh sách URI
    http_server_uri_list_t* element;
    while (s_http_server_manager->uri_list_registed != NULL)
    {
        element = s_http_server_manager->uri_list_registed;
        LL_DELETE(s_http_server_manager->uri_list_registed, element);

        if (element->httpd_uri_inform.uri) free((void*)element->httpd_uri_inform.uri);
        if (element->httpd_uri_inform.user_ctx) free(element->httpd_uri_inform.user_ctx);
        free(element);
    }

    // Giải phóng manager
    free(s_http_server_manager);
    s_http_server_manager = NULL;

    return PERIPH_OK;
}
