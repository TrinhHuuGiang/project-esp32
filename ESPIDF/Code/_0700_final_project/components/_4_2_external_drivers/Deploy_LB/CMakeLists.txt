idf_component_register(
    SRCS
    "Deploy_LB.c"

    INCLUDE_DIRS
    "."

    REQUIRES
    _1_reference_libs
    _1_task_sync_tools
    _2_1_internal_periphs

    LB_main_design LB_test
    )
    