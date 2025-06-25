idf_component_register(
    SRCS 
    "main.c"
    "main_init_api.c"
    INCLUDE_DIRS
    "."

    REQUIRES
    json 

    # project components
    _1_containers
    _1_reference_libs
    _1_task_sync_tools
    _2_1_internal_periphs
    
    adc_74HC4067 gpio_74HC165 gpio_74HC595 i2s_max98357

    i2c_ds3231 i2c_ssd1306 spi_sd_card

    HTTP_server_lib MQTT_client_lib

    _4_1_internal_drivers
    )
