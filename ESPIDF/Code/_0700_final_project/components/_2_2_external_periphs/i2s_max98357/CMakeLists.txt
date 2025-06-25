idf_component_register(
    SRCS
    "i2s_max98357.c"
    INCLUDE_DIRS
    "."
    REQUIRES
    # project components
    _1_task_sync_tools
    _2_1_internal_periphs)