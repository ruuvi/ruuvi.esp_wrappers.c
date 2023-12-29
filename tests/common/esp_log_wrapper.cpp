/**
 * @file esp_log_wrapper.cpp
 * @author TheSomeMan
 * @date 2020-08-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include <cstdarg>
#include <cassert>
#include "esp_log.h"
#include "TQueue.hpp"
#include "esp_log_wrapper.hpp"

static TQueue<LogRecord>* gp_esp_log_queue;

void
esp_log_wrapper_init()
{
    gp_esp_log_queue = new TQueue<LogRecord>;
}

void
esp_log_wrapper_deinit()
{
    if (nullptr != gp_esp_log_queue)
    {
        delete gp_esp_log_queue;
        gp_esp_log_queue = nullptr;
    }
}

void
esp_log_write(esp_log_level_t level, const char* tag, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogRecord log_record = LogRecord(level, tag, fmt, args);
    va_end(args);
    assert(nullptr != gp_esp_log_queue);
    gp_esp_log_queue->push(log_record);
}

bool
esp_log_wrapper_is_empty()
{
    assert(nullptr != gp_esp_log_queue);
    return gp_esp_log_queue->is_empty();
}

LogRecord
esp_log_wrapper_pop()
{
    assert(nullptr != gp_esp_log_queue);
    return gp_esp_log_queue->pop();
}

void
esp_log_wrapper_clear()
{
    assert(nullptr != gp_esp_log_queue);
    gp_esp_log_queue->clear();
}

string
esp_log_wrapper_get_logs()
{
    string result = "";
    while (!esp_log_wrapper_is_empty())
    {
        const LogRecord log_record = esp_log_wrapper_pop();
        string          prefix     = "";
        switch (log_record.level)
        {
            case ESP_LOG_NONE:
                break;
            case ESP_LOG_ERROR:
                prefix = "E";
                break;
            case ESP_LOG_WARN:
                prefix = "W";
                break;
            case ESP_LOG_INFO:
                prefix = "I";
                break;
            case ESP_LOG_DEBUG:
                prefix = "D";
                break;
            case ESP_LOG_VERBOSE:
                prefix = "V";
                break;
        }
        if (prefix == "")
        {
            continue;
        }
        result += prefix + " " + log_record.tag + ": " + log_record.parsed.msg + "\n";
    }
    return result;
}
