idf_component_register(
    SRCS
    "spi_sd_mount_to_fs.c"
    INCLUDE_DIRS
    "."
    REQUIRES
    fatfs
    
    # project components
    _1_task_sync_tools
    _2_1_internal_periphs
    gpio_74HC595)