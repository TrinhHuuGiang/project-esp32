idf_component_register(
    SRCS
    "spi_sd_mount_to_fs.c"
    INCLUDE_DIRS
    "."
    "${PROJECT_DIR}/components/_peripherals_handle/"
    REQUIRES
    fatfs)