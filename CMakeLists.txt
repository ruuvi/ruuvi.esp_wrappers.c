cmake_minimum_required(VERSION 3.5)

set(RUUVI_ESP_WRAPPERS_SRC
        include/os_malloc.h
        include/attribs.h
        include/esp_type_wrapper.h
        include/log.h
        include/mac_addr.h
        include/os_mkgmtime.h
        include/os_mutex.h
        include/os_mutex_recursive.h
        include/os_sema.h
        include/os_signal.h
        include/os_str.h
        include/os_task.h
        include/os_time.h
        include/os_timer.h
        include/os_timer_sig.h
        include/os_wrapper_types.h
        include/time_units.h
        include/str_buf.h
        include/wrap_esp_err_to_name_r.h
        src/log_dump.c
        src/mac_addr.c
        src/os_mkgmtime.c
        src/os_malloc.c
        src/os_mutex.c
        src/os_mutex_recursive.c
        src/os_sema.c
        src/os_signal.c
        src/os_str.c
        src/os_task.c
        src/os_task_delay.c
        src/os_time.c
        src/os_timer.c
        src/os_timer_sig.c
        src/str_buf.c
        src/wrap_esp_err_to_name_r.c
)

set(RUUVI_ESP_WRAPPERS_INC
        include
)

if(${ESP_PLATFORM})

    idf_component_register(
        SRCS
            ${RUUVI_ESP_WRAPPERS_SRC}
        INCLUDE_DIRS
            ${RUUVI_ESP_WRAPPERS_INC}
        PRIV_REQUIRES
            esp-tls
    )

    target_compile_options(__idf_ruuvi.esp_wrappers.c PRIVATE -Wall -Werror -Wextra -Wno-error=nonnull-compare)

else()

    project(ruuvi_esp_wrappers)
    set(ProjectId ruuvi_esp_wrappers)

    add_library(${ProjectId} STATIC ${RUUVI_ESP_WRAPPERS_SRC})

    target_include_directories(${ProjectId} PUBLIC ${RUUVI_ESP_WRAPPERS_INC})

    set_target_properties(${ProjectId} PROPERTIES
            C_STANDARD 11
            CXX_STANDARD 14
    )

    target_compile_options(${ProjectId} PRIVATE -Wall -Werror -Wextra -Wno-error=nonnull-compare)

endif()
