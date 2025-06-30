idf_component_register(
    SRCS
    "Logic_Test_design_port.c"

    INCLUDE_DIRS
    "."

    REQUIRES
    _1_reference_libs
    _1_task_sync_tools

    gpio_74HC595

    _4_1_internal_drivers

    Deploy_LB
    )
    