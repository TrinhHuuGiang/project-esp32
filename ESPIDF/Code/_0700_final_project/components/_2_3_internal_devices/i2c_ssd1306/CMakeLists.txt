idf_component_register(
    SRCS
    "i2c_ssd1306.c"
    "i2c_ssd1306_bitmap.c"
    INCLUDE_DIRS
    "."
    "${PROJECT_DIR}/components/_1_task_sync_tools/"
    "${PROJECT_DIR}/components/_2_1_internal_periphs/")
