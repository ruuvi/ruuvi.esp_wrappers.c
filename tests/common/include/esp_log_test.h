/**
 * @file esp_log_test.h
 * @author TheSomeMan
 * @date 2020-08-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifdef ESP_LOG_H
#warning ESP_LOG_H already defined
#endif

#ifndef __ESP_LOG_H__
#define __ESP_LOG_H__

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

uint32_t
esp_log_timestamp(void);

static inline void
esp_log_level_set(__attribute__((unused)) const char* tag, __attribute__((unused)) esp_log_level_t level)
{
    (void)tag;
    (void)level;
}

#ifdef __cplusplus
}
#endif

#endif // __ESP_LOG_H__
