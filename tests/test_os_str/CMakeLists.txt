cmake_minimum_required(VERSION 3.7)

project(ruuvi_esp_wrappers-test-os_str)
set(ProjectId ruuvi_esp_wrappers-test-os_str)

add_executable(${ProjectId}
        test_os_str_to_int32.cpp
        test_os_str_to_uint32.cpp
        ../../src/os_str.c
        ../../include/os_str.h
)

set_target_properties(${ProjectId} PROPERTIES
        C_STANDARD 11
        CXX_STANDARD 14
)

target_include_directories(${ProjectId} PUBLIC
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ../../include
        include
)

target_compile_definitions(${ProjectId} PUBLIC
        RUUVI_TESTS_OS_STR=1
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
        gcov
        ruuvi_esp_wrappers-common_test_funcs
        --coverage
)
