cmake_minimum_required(VERSION 3.7)

project(ruuvi_esp_wrappers-common_test_funcs)
set(ProjectId ruuvi_esp_wrappers-common_test_funcs)

set(RUUVI_ESP_WRAPPERS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${ProjectId} STATIC
        ${RUUVI_ESP_WRAPPERS_INCLUDE}/esp_log.h
        ${RUUVI_ESP_WRAPPERS_INCLUDE}/esp_err.h
        ${RUUVI_ESP_WRAPPERS_INCLUDE}/TQueue.hpp
        ${RUUVI_ESP_WRAPPERS_INCLUDE}/esp_log_wrapper.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/esp_log_wrapper.cpp
)

target_include_directories(${ProjectId} PUBLIC ${RUUVI_ESP_WRAPPERS_INCLUDE})

set_target_properties(${ProjectId} PROPERTIES
        C_STANDARD 11
        CXX_STANDARD 14
)
