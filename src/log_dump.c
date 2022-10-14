/**
 * @file log_dump.c
 * @author TheSomeMan
 * @date 2021-02-07
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "str_buf.h"
#include "esp_type_wrapper.h"

#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG
#include "log.h"

#define LOG_DUMP_LINE_BUF       (120U)
#define LOG_DUMP_BYTES_PER_LINE (16U)

static void
print_ascii_char(str_buf_t* p_str_buf, const char ch)
{
    str_buf_printf(p_str_buf, "%c", (char)(isprint((int)(unsigned char)ch) ? ch : '.'));
}

static void
print_bytes(str_buf_t* p_str_buf, const uint8_t* p_buf, const size_t buf_size)
{
    for (size_t j = 0; j < buf_size; ++j, p_buf++)
    {
        str_buf_printf(p_str_buf, "%02X ", (printf_uint_t)*p_buf);
    }
}

static void
print_chars(str_buf_t* p_str_buf, const uint8_t* p_buf, const size_t buf_size)
{
    for (size_t j = 0; j < buf_size; j++)
    {
        print_ascii_char(p_str_buf, *p_buf++);
    }
}

void
log_print_dump(
    esp_log_level_t level,
    const char*     p_tag,
    const char*     p_log_prefix,
    const uint8_t*  p_buf,
    const uint32_t  buf_size)
{
    char log_dump_line_buf[LOG_DUMP_LINE_BUF];

    const uint32_t ascii_column_num = ((buf_size <= UINT16_MAX) ? 4 : 8) + 2 + LOG_DUMP_BYTES_PER_LINE * 3;

    for (uint32_t offset = 0; offset < buf_size; offset += LOG_DUMP_BYTES_PER_LINE)
    {
        str_buf_t str_buf = STR_BUF_INIT(log_dump_line_buf, sizeof(log_dump_line_buf));
        if (buf_size <= UINT16_MAX)
        {
            str_buf_printf(&str_buf, "%04X: ", (printf_uint_t)offset);
        }
        else
        {
            str_buf_printf(&str_buf, "%08X: ", (printf_uint_t)offset);
        }
        const uint32_t rem_bytes = buf_size - offset;
        if (rem_bytes >= LOG_DUMP_BYTES_PER_LINE)
        {
            print_bytes(&str_buf, &p_buf[offset], LOG_DUMP_BYTES_PER_LINE);
            str_buf_printf(&str_buf, "| ");
            print_chars(&str_buf, &p_buf[offset], LOG_DUMP_BYTES_PER_LINE);
        }
        else
        {
            print_bytes(&str_buf, &p_buf[offset], rem_bytes);
            str_buf_printf(&str_buf, "%*s| ", (printf_int_t)(ascii_column_num - str_buf_get_len(&str_buf)), "");
            print_chars(&str_buf, &p_buf[offset], rem_bytes);
        }
        esp_log_write(
            level,
            p_tag,
            "%s (%d) %s: [%s/%d] %s\n",
            p_log_prefix,
            esp_log_timestamp(),
            p_tag,
            os_task_get_name(),
            (printf_int_t)os_task_get_priority(),
            log_dump_line_buf);
    }
}
