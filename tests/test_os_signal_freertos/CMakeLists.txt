cmake_minimum_required(VERSION 3.7)

project(ruuvi_esp_wrappers-test-os_signal_freertos)
set(ProjectId ruuvi_esp_wrappers-test-os_signal_freertos)

add_executable(${ProjectId}
        test_os_signal_freertos.cpp
        ../../src/os_signal.c
        ../../src/app_malloc.c
        ../../src/os_task.c
        ../../include/os_signal.h
)

set_target_properties(${ProjectId} PROPERTIES
        C_STANDARD 11
        CXX_STANDARD 14
)

target_include_directories(${ProjectId} PUBLIC
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ../../include
)

target_compile_definitions(${ProjectId} PUBLIC
        RUUVI_TESTS_OS_SIGNAL_FREERTOS=1
)

target_compile_options(${ProjectId} PUBLIC
        -g3
        -ggdb
        -fprofile-arcs
        -ftest-coverage
        --coverage
)

# CMake has a target_link_options starting from version 3.13
#target_link_options(${ProjectId} PUBLIC
#        --coverage
#)

target_link_libraries(${ProjectId}
        gtest
        gtest_main
        FreeRTOS_Posix
        esp_simul
        gcov
        ruuvi_esp_wrappers-common_test_funcs
        --coverage
)
