# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)


# include component
set(EXTRA_COMPONENT_DIRS
    "${CMAKE_SOURCE_DIR}/components/_2_2_external_periphs"
    "${CMAKE_SOURCE_DIR}/components/_2_3_internal_devices"
    "${CMAKE_SOURCE_DIR}/components/_2_4_net_protocols"

    # "${CMAKE_SOURCE_DIR}/components/_3_external_devices"

    # "${CMAKE_SOURCE_DIR}/components/_4_1_internal_drivers"
    # "${CMAKE_SOURCE_DIR}/components/_4_2_external_driver"
    )


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(dev_board_home_iot)
