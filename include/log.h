/**
 * @file log.h
 * @author TheSomeMan
 * @date 2020-10-24
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef RUUVI_LOG_H
#define RUUVI_LOG_H

#include "esp_log.h"
#include "os_task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_LEVEL_NONE    0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4
#define LOG_LEVEL_VERBOSE 5

#ifndef __cplusplus
_Static_assert(LOG_LEVEL_NONE == ESP_LOG_NONE, "LOG_LEVEL_NONE == ESP_LOG_NONE");
_Static_assert(LOG_LEVEL_ERROR == ESP_LOG_ERROR, "LOG_LEVEL_ERROR == ESP_LOG_ERROR");
_Static_assert(LOG_LEVEL_WARN == ESP_LOG_WARN, "LOG_LEVEL_WARN == ESP_LOG_WARN");
_Static_assert(LOG_LEVEL_INFO == ESP_LOG_INFO, "LOG_LEVEL_INFO == ESP_LOG_INFO");
_Static_assert(LOG_LEVEL_DEBUG == ESP_LOG_DEBUG, "LOG_LEVEL_DEBUG == ESP_LOG_DEBUG");
_Static_assert(LOG_LEVEL_VERBOSE == ESP_LOG_VERBOSE, "LOG_LEVEL_VERBOSE == ESP_LOG_VERBOSE");
#endif // __cplusplus

#if !defined(LOG_LOCAL_LEVEL)
#error Macro LOG_LOCAL_LEVEL must be defined before including "log.h"
#endif

#if defined(LOG_LOCAL_DISABLED)
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL LOG_LEVEL_NONE
#endif

#if !defined(LOG_LOCAL_DISABLED) && !((LOG_LOCAL_LEVEL >= LOG_LEVEL_ERROR) && (LOG_LOCAL_LEVEL <= LOG_LEVEL_VERBOSE))
#error Macro LOG_LOCAL_LEVEL should be defined as one of LOG_LEVEL_ERROR, LOG_LEVEL_WARN, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_VERBOSE or LOG_LOCAL_DISABLED should be defined to disable logging
#endif

extern uint32_t
esp_log_timestamp(void);

extern void
log_print_dump(
    esp_log_level_t level,
    const char *    p_tag,
    const char *    p_log_prefix,
    const uint8_t * p_buf,
    const uint32_t  buf_size);

#if LOG_LOCAL_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERR(fmt, ...) \
    esp_log_write( \
        ESP_LOG_ERROR, \
        TAG, \
        LOG_FORMAT(E, "[%s/%d] %s:%d {%s}: " fmt), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        uxTaskPriorityGet(NULL), \
        __FILE__, \
        __LINE__, \
        __func__, \
        ##__VA_ARGS__)

#define LOG_ERR_ESP(err, fmt, ...) \
    esp_log_write( \
        ESP_LOG_ERROR, \
        TAG, \
        LOG_FORMAT(E, "[%s/%d] %s:%d {%s}: " fmt ", err=%d (%s)"), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        uxTaskPriorityGet(NULL), \
        __FILE__, \
        __LINE__, \
        __func__, \
        ##__VA_ARGS__, \
        err, \
        esp_err_to_name(err))

#define LOG_ERR_VAL(err, fmt, ...) \
    esp_log_write( \
        ESP_LOG_ERROR, \
        TAG, \
        LOG_FORMAT(E, "[%s/%d] %s:%d {%s}: " fmt ", err=%d"), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        uxTaskPriorityGet(NULL), \
        __FILE__, \
        __LINE__, \
        __func__, \
        ##__VA_ARGS__, \
        err)

#define LOG_DUMP_ERR(p_buf, buf_size, fmt, ...) \
    do \
    { \
        esp_log_write( \
            ESP_LOG_ERROR, \
            TAG, \
            LOG_FORMAT(E, "[%s/%d] %s:%d {%s}: " fmt ":"), \
            esp_log_timestamp(), \
            TAG, \
            os_task_get_name(), \
            uxTaskPriorityGet(NULL), \
            __FILE__, \
            __LINE__, \
            __func__, \
            ##__VA_ARGS__); \
        log_print_dump(ESP_LOG_ERROR, TAG, "E", p_buf, buf_size); \
    } while (0)

#else
#define LOG_ERR(fmt, ...)                       (void)0
#define LOG_ERR_ESP(err, fmt, ...)              (void)0
#define LOG_ERR_VAL(err, fmt, ...)              (void)0
#define LOG_DUMP_ERR(p_buf, buf_size, fmt, ...) (void)0
#endif

#if LOG_LOCAL_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(fmt, ...) \
    esp_log_write( \
        ESP_LOG_WARN, \
        TAG, \
        LOG_FORMAT(W, "[%s/%d] " fmt), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        uxTaskPriorityGet(NULL), \
        ##__VA_ARGS__)

#define LOG_WARN_ESP(err, fmt, ...) \
    esp_log_write( \
        ESP_LOG_WARN, \
        TAG, \
        LOG_FORMAT(W, "[%s/%d] " fmt ", err=%d (%s)"), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        uxTaskPriorityGet(NULL), \
        ##__VA_ARGS__, \
        err, \
        esp_err_to_name(err))

#define LOG_WARN_VAL(err, fmt, ...) \
    esp_log_write( \
        ESP_LOG_WARN, \
        TAG, \
        LOG_FORMAT(W, "[%s/%d] " fmt ", err=%d"), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        uxTaskPriorityGet(NULL), \
        ##__VA_ARGS__, \
        err)

#define LOG_DUMP_WARN(p_buf, buf_size, fmt, ...) \
    do \
    { \
        esp_log_write( \
            ESP_LOG_WARN, \
            TAG, \
            LOG_FORMAT(W, "[%s/%d] " fmt ":"), \
            esp_log_timestamp(), \
            TAG, \
            os_task_get_name(), \
            uxTaskPriorityGet(NULL), \
            ##__VA_ARGS__); \
        log_print_dump(ESP_LOG_WARN, TAG, "W", p_buf, buf_size); \
    } while (0)
#else
#define LOG_WARN(fmt, ...)                       (void)0
#define LOG_WARN_ESP(err, fmt, ...)              (void)0
#define LOG_WARN_VAL(err, fmt, ...)              (void)0
#define LOG_DUMP_WARN(p_buf, buf_size, fmt, ...) (void)0
#endif

#if LOG_LOCAL_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...) \
    esp_log_write( \
        ESP_LOG_INFO, \
        TAG, \
        LOG_FORMAT(I, "[%s/%d] " fmt), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        uxTaskPriorityGet(NULL), \
        ##__VA_ARGS__)

#define LOG_DUMP_INFO(p_buf, buf_size, fmt, ...) \
    do \
    { \
        esp_log_write( \
            ESP_LOG_INFO, \
            TAG, \
            LOG_FORMAT(I, "[%s/%d] " fmt ":"), \
            esp_log_timestamp(), \
            TAG, \
            os_task_get_name(), \
            uxTaskPriorityGet(NULL), \
            ##__VA_ARGS__); \
        log_print_dump(ESP_LOG_INFO, TAG, "I", p_buf, buf_size); \
    } while (0)
#else
#define LOG_INFO(fmt, ...)                       (void)0
#define LOG_DUMP_INFO(p_buf, buf_size, fmt, ...) (void)0
#endif

#if LOG_LOCAL_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DBG(fmt, ...) \
    esp_log_write( \
        ESP_LOG_DEBUG, \
        TAG, \
        LOG_FORMAT(D, "[%s/%d] %s:%d {%s}: " fmt), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        uxTaskPriorityGet(NULL), \
        __FILE__, \
        __LINE__, \
        __func__, \
        ##__VA_ARGS__)

#define LOG_DUMP_DBG(p_buf, buf_size, fmt, ...) \
    do \
    { \
        esp_log_write( \
            ESP_LOG_DEBUG, \
            TAG, \
            LOG_FORMAT(D, "[%s/%d] %s:%d {%s}: " fmt ":"), \
            esp_log_timestamp(), \
            TAG, \
            os_task_get_name(), \
            uxTaskPriorityGet(NULL), \
            __FILE__, \
            __LINE__, \
            __func__, \
            ##__VA_ARGS__); \
        log_print_dump(ESP_LOG_DEBUG, TAG, "D", p_buf, buf_size); \
    } while (0)
#else
#define LOG_DBG(fmt, ...)                       (void)0
#define LOG_DUMP_DBG(p_buf, buf_size, fmt, ...) (void)0
#endif

#if LOG_LOCAL_LEVEL >= LOG_LEVEL_VERBOSE
#define LOG_VERBOSE(fmt, ...) \
    esp_log_write( \
        ESP_LOG_VERBOSE, \
        TAG, \
        LOG_FORMAT(V, "[%s] " fmt), \
        esp_log_timestamp(), \
        TAG, \
        os_task_get_name(), \
        ##__VA_ARGS__)

#define LOG_DUMP_VERBOSE(p_buf, buf_size, fmt, ...) \
    do \
    { \
        esp_log_write( \
            ESP_LOG_VERBOSE, \
            TAG, \
            LOG_FORMAT(V, "[%s] " fmt ":"), \
            esp_log_timestamp(), \
            TAG, \
            os_task_get_name(), \
            ##__VA_ARGS__); \
        log_print_dump(ESP_LOG_VERBOSE, TAG, "V", p_buf, buf_size); \
    } while (0)
#else
#define LOG_VERBOSE(fmt, ...)                       (void)0
#define LOG_DUMP_VERBOSE(p_buf, buf_size, fmt, ...) (void)0
#endif

#ifdef __cplusplus
}
#endif

#endif // RUUVI_LOG_H
