idf_component_register(
    SRCS
    "rtc_sntp.c"
    INCLUDE_DIRS
    "."
    REQUIRES
    lwip
    
    # project components
    _1_reference_libs
    _1_task_sync_tools
    _2_1_internal_periphs)
