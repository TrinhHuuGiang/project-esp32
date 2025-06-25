idf_component_register(
    SRCS
    "http_server_handle.c"
    INCLUDE_DIRS
    "."

    REQUIRES
    esp_http_server nvs_flash
    
    # project components
    _1_reference_libs
    _1_task_sync_tools
    _2_1_internal_periphs)
