/**
 * @file str_buf.c
 * @author TheSomeMan
 * @date 2020-08-23
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "str_buf.h"
#include <stdio.h>
#include "os_malloc.h"
#include "attribs.h"

ATTR_NONNULL(1)
str_buf_t
str_buf_init(char* const p_buf, const str_buf_size_t buf_size)
{
    const str_buf_t str_buf = STR_BUF_INIT(p_buf, buf_size);
    return str_buf;
}

str_buf_t
str_buf_init_null(void)
{
    const str_buf_t str_buf = STR_BUF_INIT_NULL();
    return str_buf;
}

ATTR_NONNULL(1)
bool
str_buf_init_with_alloc(str_buf_t* const p_str_buf)
{
    const size_t buf_size = str_buf_get_len(p_str_buf) + 1;
    char*        p_buf    = os_malloc(buf_size);
    if (NULL == p_buf)
    {
        return false;
    }
    *p_str_buf = str_buf_init(p_buf, buf_size);
    return true;
}

ATTR_NONNULL(1)
ATTR_PURE
str_buf_size_t
str_buf_get_len(const str_buf_t* const p_str_buf)
{
    return p_str_buf->idx;
}

ATTR_NONNULL(1)
ATTR_PURE
bool
str_buf_is_overflow(const str_buf_t* const p_str_buf)
{
    if (0 == p_str_buf->size)
    {
        return false;
    }
    if (p_str_buf->idx >= p_str_buf->size)
    {
        return true;
    }
    return false;
}

ATTR_NONNULL(1, 2)
bool
str_buf_vprintf(str_buf_t* const p_str_buf, const char* const fmt, va_list args)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
    if (NULL == p_str_buf)
    {
        return false;
    }
#pragma GCC diagnostic pop
    if ((NULL == p_str_buf->buf) && (0 != p_str_buf->size))
    {
        return false;
    }
    if ((0 == p_str_buf->size) && (NULL != p_str_buf->buf))
    {
        return false;
    }
    if (0 != p_str_buf->size)
    {
        if (p_str_buf->idx >= p_str_buf->size)
        {
            return false;
        }
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
    if (NULL == fmt)
    {
        return false;
    }
#pragma GCC diagnostic pop
    char*        p_buf   = (NULL != p_str_buf->buf) ? &p_str_buf->buf[p_str_buf->idx] : NULL;
    const size_t max_len = (0 != p_str_buf->size) ? (p_str_buf->size - p_str_buf->idx) : 0;

    const int len = vsnprintf(p_buf, max_len, fmt, args);
    if (len < 0)
    {
        p_str_buf->idx = p_str_buf->size;
        return false;
    }
    p_str_buf->idx += len;
    if (0 != p_str_buf->size)
    {
        if (p_str_buf->idx >= p_str_buf->size)
        {
            p_str_buf->idx = p_str_buf->size;
            return false;
        }
    }
    return true;
}

ATTR_PRINTF(2, 3)
ATTR_NONNULL(1, 2)
bool
str_buf_printf(str_buf_t* const p_str_buf, const char* const fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const bool res = str_buf_vprintf(p_str_buf, fmt, args);
    va_end(args);
    return res;
}

ATTR_NONNULL(1)
str_buf_t
str_buf_vprintf_with_alloc(const char* const fmt, va_list args)
{
    str_buf_t str_buf = str_buf_init_null();
    va_list   args2;
    va_copy(args2, args);
    const bool res = str_buf_vprintf(&str_buf, fmt, args2);
    va_end(args2);
    if (!res)
    {
        return str_buf_init_null();
    }
    if (!str_buf_init_with_alloc(&str_buf))
    {
        return str_buf_init_null();
    }
    str_buf_vprintf(&str_buf, fmt, args);
    return str_buf;
}

ATTR_PRINTF(1, 2)
ATTR_NONNULL(1)
str_buf_t
str_buf_printf_with_alloc(const char* const fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const str_buf_t str_buf = str_buf_vprintf_with_alloc(fmt, args);
    va_end(args);
    return str_buf;
}

ATTR_NONNULL(1, 2)
bool
str_buf_bin_to_hex(str_buf_t* const p_str_buf, const uint8_t* const p_input_buf, const size_t input_buf_size)
{
    static const char hex[] = "0123456789abcdef";
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
    if (NULL == p_str_buf)
    {
        return false;
    }
#pragma GCC diagnostic pop
    if ((NULL == p_str_buf->buf) && (0 != p_str_buf->size))
    {
        return false;
    }
    if ((0 == p_str_buf->size) && (NULL != p_str_buf->buf))
    {
        return false;
    }
    if (0 != p_str_buf->size)
    {
        if (p_str_buf->idx >= p_str_buf->size)
        {
            return false;
        }
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
    if (NULL == p_input_buf)
    {
        return false;
    }
#pragma GCC diagnostic pop
    char*        p_buf   = (NULL != p_str_buf->buf) ? &p_str_buf->buf[p_str_buf->idx] : NULL;
    const size_t max_len = (0 != p_str_buf->size) ? (p_str_buf->size - p_str_buf->idx) : 0;

    bool   flag_overflow = false;
    size_t len           = 0;
    if (NULL == p_buf)
    {
        len = input_buf_size * 2;
    }
    else
    {
        for (size_t i = 0; i < input_buf_size; ++i)
        {
            const uint8_t byte_val = p_input_buf[i];

            if ((len + 2) >= max_len)
            {
                flag_overflow = true;
                break;
            }
            p_buf[len++] = hex[(uint8_t)(byte_val >> 4U) & 0x0FU];
            p_buf[len++] = hex[byte_val & 0x0FU];
        }
        p_buf[len] = '\0';
    }

    p_str_buf->idx += len;
    if (0 != p_str_buf->size)
    {
        if (p_str_buf->idx >= p_str_buf->size)
        {
            p_str_buf->idx = p_str_buf->size;
            return false;
        }
        if (flag_overflow)
        {
            return false;
        }
    }
    return true;
}

ATTR_NONNULL(1)
str_buf_t
str_buf_bin_to_hex_with_alloc(const uint8_t* const p_input_buf, const size_t input_buf_size)
{
    str_buf_t  str_buf = str_buf_init_null();
    const bool res     = str_buf_bin_to_hex(&str_buf, p_input_buf, input_buf_size);
    if (!res)
    {
        return str_buf_init_null();
    }
    if (!str_buf_init_with_alloc(&str_buf))
    {
        return str_buf_init_null();
    }
    str_buf_bin_to_hex(&str_buf, p_input_buf, input_buf_size);
    return str_buf;
}

ATTR_NONNULL(1)
void
str_buf_free_buf(str_buf_t* const p_str_buf)
{
    if (NULL != p_str_buf->buf)
    {
        os_free(p_str_buf->buf);
    }
    p_str_buf->size = 0;
    p_str_buf->idx  = 0;
    p_str_buf->buf  = NULL;
}
