cmake_minimum_required(VERSION 3.7)

project(ruuvi_esp_wrappers-test-os_task)
set(ProjectId ruuvi_esp_wrappers-test-os_task)

add_executable(${ProjectId}
        test_os_task.hpp
        test_os_task.cpp
        test_os_task_create.cpp
        test_os_task_create_with_const_param.cpp
        test_os_task_create_without_param.cpp
        test_os_task_create_finite.cpp
        test_os_task_create_finite_with_const_param.cpp
        test_os_task_create_finite_without_param.cpp
        test_os_task_create_static.cpp
        test_os_task_create_static_with_const_param.cpp
        test_os_task_create_static_without_param.cpp
        test_os_task_create_static_finite.cpp
        test_os_task_create_static_finite_with_const_param.cpp
        test_os_task_create_static_finite_without_param.cpp
        stubs.cpp
        ../../src/os_task.c
        ../../include/os_task.h
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
        RUUVI_TESTS_OS_TASK=1
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
        esp_simul
        --coverage
)
