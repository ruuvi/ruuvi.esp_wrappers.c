/**
 * @file esp_log_wrapper.hpp
 * @author TheSomeMan
 * @date 2020-08-26
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef RUUVI_TESTS_ESP_LOG_WRAPPER_HPP
#define RUUVI_TESTS_ESP_LOG_WRAPPER_HPP

#include "esp_log.h"
#include <string>
#include <cstdarg>
#include <cstdio>
#include <regex>

using namespace std;

class LogRecordParsed
{
public:
    string log_level;
    string tag;
    string thread;
    int    priority;
    string file;
    int    line {};
    string func;
    string msg;
};

class LogRecord
{
private:
    void
    init_message(const char *fmt, va_list args)
    {
        va_list args2;
        va_copy(args2, args);
        const int len = vsnprintf(nullptr, 0, fmt, args);
        char *    buf { new char[len + 1] {} };
        vsnprintf(buf, len + 1, fmt, args2);
        va_end(args2);
        this->message = string(buf);
        delete[] buf;
        bool flag_skip_file_info = true;
        switch (this->message[0])
        {
            case 'E':
            case 'D':
                flag_skip_file_info = false;
                break;
        }
        this->parsed = this->parse(flag_skip_file_info);
    }

    LogRecordParsed
    parse(const bool flag_skip_file_info) const
    {
        LogRecordParsed msg_parsed = {};
        if (flag_skip_file_info)
        {
            // "I (0) SWD: [main/1] nRF52 SWD init"
            const std::regex msg_regex(
                R"(([EWIDV]) \([0-9]+\) ([^ ]+): \[([^ ]+)\/([0-9]+)\] (.*))",
                std::regex::extended);
            std::smatch match;
            std::regex_match(this->message, match, msg_regex);
            const size_t exp_num_regexp_matches = 6U;
            const size_t num_regexp_matched     = match.size();
            if (num_regexp_matched != exp_num_regexp_matches)
            {
                std::stringstream ss;
                ss << "Expected ";
                ss << exp_num_regexp_matches;
                ss << ", but found ";
                ss << num_regexp_matched;
                ss << " regex matches in log record: ";
                ss << this->message;
                throw std::runtime_error(ss.str());
            }
            msg_parsed.log_level = match[1].str();
            msg_parsed.tag       = match[2].str();
            msg_parsed.thread    = match[3].str();
            msg_parsed.priority  = std::stoi(match[4].str());
            msg_parsed.msg       = match[5].str();
        }
        else
        {
            // "E (0) SWD: [main/1] ruuvi.gateway_esp.c/main/nrf52swd.c:79 {nrf52swd_init_gpio_cfg_nreset}:
            // "E (0) test: [thread_name/priority] 0000: 30                                              | 0"
            const std::regex msg_regex(
                R"(([EWIDV]) \([0-9]+\) ([^ ]+): \[([^ ]+)\/([0-9]+)\] ([^ ]+):([0-9]+) \{([^ ]+)\}: (.*))",
                std::regex::extended);
            std::smatch match;
            std::regex_match(this->message, match, msg_regex);
            const size_t exp_num_regexp_matches = 9U;
            const size_t num_regexp_matched     = match.size();
            bool         flag_log_dump          = false;
            if (num_regexp_matched != exp_num_regexp_matches)
            {
                const std::regex msg_regex_log_dump(
                    R"(([EWIDV]) \([0-9]+\) ([^ ]+): \[([^ ]+)/([0-9]+)\] ([0-9A-F]+: [0-9A-F]+.*))",
                    std::regex::extended);
                std::regex_match(this->message, match, msg_regex_log_dump);
                if (6U == match.size())
                {
                    flag_log_dump = true;
                }
                else
                {
                    std::stringstream ss;
                    ss << "Expected ";
                    ss << exp_num_regexp_matches;
                    ss << ", but found ";
                    ss << num_regexp_matched;
                    ss << " regex matches in log record: ";
                    ss << this->message;
                    throw std::runtime_error(ss.str());
                }
            }
            if (!flag_log_dump)
            {
                msg_parsed.log_level = match[1].str();
                msg_parsed.tag       = match[2].str();
                msg_parsed.thread    = match[3].str();
                msg_parsed.priority  = std::stoi(match[4].str());
                msg_parsed.file      = match[5].str();
                msg_parsed.line      = std::stoi(match[6].str());
                msg_parsed.func      = match[7].str();
                msg_parsed.msg       = match[8].str();
            }
            else
            {
                msg_parsed.log_level = match[1].str();
                msg_parsed.tag       = match[2].str();
                msg_parsed.thread    = match[3].str();
                msg_parsed.priority  = std::stoi(match[4].str());
                msg_parsed.msg       = match[5].str();
            }
        }
        return msg_parsed;
    }

public:
    esp_log_level_t level;
    string          tag;
    string          message;
    LogRecordParsed parsed;

    LogRecord(esp_log_level_t level, const char *tag, const char *fmt, va_list args)
        : level(level)
        , tag(string(tag))
    {
        this->init_message(fmt, args);
    }

    LogRecord(esp_log_level_t level, const char *tag, const char *fmt, ...)
        : level(level)
        , tag(string(tag))
    {
        va_list args;
        va_start(args, fmt);
        this->init_message(fmt, args);
        va_end(args);
    }
};

void
esp_log_wrapper_init();

void
esp_log_wrapper_deinit();

bool
esp_log_wrapper_is_empty();

LogRecord
esp_log_wrapper_pop();

void
esp_log_wrapper_clear();

#define ESP_LOG_WRAPPER_TEST_CHECK_LOG_RECORD(tag_, level_, msg_) \
    do \
    { \
        ASSERT_FALSE(esp_log_wrapper_is_empty()); \
        const LogRecord log_record = esp_log_wrapper_pop(); \
        ASSERT_EQ(level_, log_record.level); \
        ASSERT_EQ(string(tag_), log_record.tag); \
        ASSERT_EQ(string(msg_), log_record.parsed.msg); \
    } while (0)

#define ESP_LOG_WRAPPER_TEST_CHECK_LOG_RECORD_WITH_FUNC(tag_, level_, func_, msg_) \
    do \
    { \
        ASSERT_FALSE(esp_log_wrapper_is_empty()); \
        const LogRecord log_record = esp_log_wrapper_pop(); \
        ASSERT_EQ(level_, log_record.level); \
        ASSERT_EQ(string(tag_), log_record.tag); \
        ASSERT_EQ(string(func_), log_record.parsed.func); \
        ASSERT_EQ(string(msg_), log_record.parsed.msg); \
    } while (0)

#define ESP_LOG_WRAPPER_TEST_CHECK_LOG_RECORD_WITH_THREAD(tag_, level_, thread_, priority_, msg_) \
    do \
    { \
        ASSERT_FALSE(esp_log_wrapper_is_empty()); \
        const LogRecord log_record = esp_log_wrapper_pop(); \
        ASSERT_EQ(level_, log_record.level); \
        ASSERT_EQ(string(tag_), log_record.tag); \
        ASSERT_EQ(string(thread_), log_record.parsed.thread); \
        ASSERT_EQ(priority_, log_record.parsed.priority); \
        ASSERT_EQ(string(msg_), log_record.parsed.msg); \
    } while (0)

#endif // RUUVI_TESTS_ESP_LOG_WRAPPER_HPP
