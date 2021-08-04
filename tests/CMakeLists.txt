cmake_minimum_required(VERSION 3.5)

project(ruuvi_esp_wrappers-tests)

add_definitions(
        -DRUUVI_ESP_WRAPPERS_TESTS=1
)

enable_testing()
add_subdirectory(googletest)
add_subdirectory(posix_gcc_simulator)

include_directories(
        "posix_gcc_simulator/FreeRTOS_Posix"
        "posix_gcc_simulator/FreeRTOS_Posix/FreeRTOS_Kernel/include"
        "posix_gcc_simulator/FreeRTOS_Posix/FreeRTOS_Kernel/include/freertos"
        "posix_gcc_simulator/FreeRTOS_Posix/FreeRTOS_Kernel/portable/GCC/Posix"
)

set(RUUVI_ESP_WRAPPERS ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(RUUVI_ESP_WRAPPERS_SRC ${RUUVI_ESP_WRAPPERS}/src)
set(RUUVI_ESP_WRAPPERS_INC ${RUUVI_ESP_WRAPPERS}/include)

set(RUUVI_ESP_WRAPPERS_TESTS_COMMON ${RUUVI_ESP_WRAPPERS}/tests/common)
set(RUUVI_ESP_WRAPPERS_TESTS_COMMON_INC ${RUUVI_ESP_WRAPPERS_TESTS_COMMON}/include)
set(RUUVI_ESP_WRAPPERS_TESTS_COMMON_SRC ${RUUVI_ESP_WRAPPERS_TESTS_COMMON})

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -Wno-error=nonnull-compare")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-error=nonnull-compare")

add_subdirectory(${RUUVI_ESP_WRAPPERS_TESTS_COMMON})

add_subdirectory(esp_simul)

add_subdirectory(test_log_dump)
add_subdirectory(test_mac_addr)
add_subdirectory(test_os_malloc)
add_subdirectory(test_os_mkgmtime)
add_subdirectory(test_os_mutex)
add_subdirectory(test_os_sema)
add_subdirectory(test_os_signal_freertos)
add_subdirectory(test_os_str)
add_subdirectory(test_os_task)
add_subdirectory(test_os_task_freertos)
add_subdirectory(test_os_timer_freertos)
add_subdirectory(test_os_timer_sig_freertos)
add_subdirectory(test_str_buf)
add_subdirectory(test_time_units)

add_test(NAME test_log_dump
        COMMAND ruuvi_esp_wrappers-test-log_dump
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-log_dump>/gtestresults.xml
)

add_test(NAME test_mac_addr
        COMMAND ruuvi_esp_wrappers-test-mac_addr
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-mac_addr>/gtestresults.xml
)

add_test(NAME test_os_malloc
        COMMAND ruuvi_esp_wrappers-test-os_malloc
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_malloc>/gtestresults.xml
)

add_test(NAME test_os_mkgmtime
        COMMAND ruuvi_esp_wrappers-test-os_mkgmtime
        --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_mkgmtime>/gtestresults.xml
)

add_test(NAME test_os_mutex
        COMMAND ruuvi_esp_wrappers-test-os_mutex
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_mutex>/gtestresults.xml
)

add_test(NAME test_os_sema
        COMMAND ruuvi_esp_wrappers-test-os_sema
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_sema>/gtestresults.xml
)

add_test(NAME test_os_signal_freertos
        COMMAND ruuvi_esp_wrappers-test-os_signal_freertos
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_signal_freertos>/gtestresults.xml
)

add_test(NAME test_os_str
        COMMAND ruuvi_esp_wrappers-test-os_str
        --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_str>/gtestresults.xml
)

add_test(NAME test_os_task
        COMMAND ruuvi_esp_wrappers-test-os_task
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_task>/gtestresults.xml
)

add_test(NAME test_os_task_freertos
        COMMAND ruuvi_esp_wrappers-test-os_task_freertos
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_task_freertos>/gtestresults.xml
)

add_test(NAME test_os_timer_freertos
        COMMAND ruuvi_esp_wrappers-test-os_timer_freertos
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_timer_freertos>/gtestresults.xml
)

add_test(NAME test_os_timer_sig_freertos
        COMMAND ruuvi_esp_wrappers-test-os_timer_sig_freertos
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-os_timer_sig_freertos>/gtestresults.xml
)

add_test(NAME test_str_buf
        COMMAND ruuvi_esp_wrappers-test-str_buf
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-str_buf>/gtestresults.xml
)

add_test(NAME test_time_units
        COMMAND ruuvi_esp_wrappers-test-time_units
            --gtest_output=xml:$<TARGET_FILE_DIR:ruuvi_esp_wrappers-test-time_units>/gtestresults.xml
)
