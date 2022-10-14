/**
 * @file esp_log.h
 * @author TheSomeMan
 * @date 2020-08-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef ESP_LOG_H
#define ESP_LOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_FORMAT(letter, format) #letter " (%d) %s: " format

typedef enum
{
    ESP_LOG_NONE,
    ESP_LOG_ERROR,
    ESP_LOG_WARN,
    ESP_LOG_INFO,
    ESP_LOG_DEBUG,
    ESP_LOG_VERBOSE
} esp_log_level_t;

void
esp_log_write(esp_log_level_t level, const char* tag, const char* fmt, ...);

static inline uint32_t
esp_log_timestamp(void)
{
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // ESP_LOG_H
