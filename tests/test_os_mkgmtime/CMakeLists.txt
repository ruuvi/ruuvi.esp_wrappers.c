cmake_minimum_required(VERSION 3.7)

project(ruuvi_esp_wrappers-test-os_mkgmtime)
set(ProjectId ruuvi_esp_wrappers-test-os_mkgmtime)

add_executable(${ProjectId}
        test_os_mkgmtime.cpp
        ../../src/os_mkgmtime.c
        ../../include/os_mkgmtime.h
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
        RUUVI_TESTS_OS_GMTIME=1
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
