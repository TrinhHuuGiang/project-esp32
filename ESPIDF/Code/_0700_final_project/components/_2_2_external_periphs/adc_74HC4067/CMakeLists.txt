idf_component_register(
    SRCS
    "adc_74hc4067.c"
    INCLUDE_DIRS
    "."
    REQUIRES
    # project components
    _1_task_sync_tools
    _2_1_internal_periphs

    gpio_74HC595)