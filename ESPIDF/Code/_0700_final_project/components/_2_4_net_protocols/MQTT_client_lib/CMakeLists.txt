idf_component_register(
    SRCS
    "mqtt_pubsub_handle.c"
    INCLUDE_DIRS
    "."
    REQUIRES
    mqtt
    
    # project components
    _1_reference_libs
    _1_task_sync_tools
    _2_1_internal_periphs)
