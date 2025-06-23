idf_component_register(
    SRCS
    "widDr_time_realtime.c"
    "widDr_batterycheck.c"
    INCLUDE_DIRS
    "."
    "${PROJECT_DIR}/components/_1_reference_libs/"     # refer libraries
    "${PROJECT_DIR}/components/_1_task_sync_tools/"    # sync tools
    "${PROJECT_DIR}/components/_2_1_internal_periphs/" # periphs libraries

    "${PROJECT_DIR}/components/_2_2_external_periphs/adc_74HC4067"# widget battery
    "${PROJECT_DIR}/components/_2_2_external_periphs/gpio_74HC595"# widget battery
    "${PROJECT_DIR}/components/_2_2_external_periphs/i2s_max98357"# widget wav player

    "${PROJECT_DIR}/components/_2_3_internal_devices/i2c_ds3231"  # widget realtime, schedule
    "${PROJECT_DIR}/components/_2_3_internal_devices/i2c_ssd1306" # widget display
    "${PROJECT_DIR}/components/_2_3_internal_devices/spi_sd_card" # widget wav player, update logic blocks


    )
