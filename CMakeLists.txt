cmake_minimum_required(VERSION 3.5)

set(RUUVI_ESP_WRAPPERS_SRC
        include/app_malloc.h
        include/app_str.h
        include/attribs.h
        include/esp_type_wrapper.h
        include/log.h
        include/os_mutex.h
        include/os_task.h
        include/os_time.h
        include/time_units.h
        include/str_buf.h
        src/app_malloc.c
        src/app_str.c
        src/os_mutex.c
        src/os_task.c
        src/os_task_delay.c
        src/os_time.c
        src/str_buf.c
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
