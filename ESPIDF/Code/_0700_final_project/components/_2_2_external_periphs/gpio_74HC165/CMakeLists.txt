idf_component_register(
    SRCS
    "gpio_74hc165.c"
    INCLUDE_DIRS
    "."
    REQUIRES
    # project components
    _1_task_sync_tools
    _2_1_internal_periphs
    )
