idf_component_register(
    SRCS
    "widDr_time_realtime.c"
    "widDr_batterycheck.c"
    "widDr_display_performance.c"

    INCLUDE_DIRS
    "."
    REQUIRES


    # project components
    _1_reference_libs
    _1_task_sync_tools
    _2_1_internal_periphs

    adc_74HC4067 i2s_max98357 #2.2
    
    i2c_ds3231 i2c_ssd1306 spi_sd_card #2.3

    SNTP_rtc #2.4
    )
    