/**
 * @file str_buf.c
 * @author TheSomeMan
 * @date 2020-08-23
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "str_buf.h"
#include <stdio.h>
#include "os_malloc.h"

str_buf_t
str_buf_init(char *p_buf, const str_buf_size_t buf_size)
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

bool
str_buf_init_with_alloc(str_buf_t *p_str_buf)
{
    const size_t buf_size = str_buf_get_len(p_str_buf) + 1;
    char *       p_buf    = app_malloc(buf_size);
    if (NULL == p_buf)
    {
        return false;
    }
    *p_str_buf = str_buf_init(p_buf, buf_size);
    return true;
}

str_buf_size_t
str_buf_get_len(const str_buf_t *p_str_buf)
{
    return p_str_buf->idx;
}

bool
str_buf_is_overflow(const str_buf_t *p_str_buf)
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

bool
str_buf_vprintf(str_buf_t *p_str_buf, const char *fmt, va_list args)
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
    char *       p_buf   = (NULL != p_str_buf->buf) ? &p_str_buf->buf[p_str_buf->idx] : NULL;
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

bool
str_buf_printf(str_buf_t *p_str_buf, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const bool res = str_buf_vprintf(p_str_buf, fmt, args);
    va_end(args);
    return res;
}

str_buf_t
str_buf_vprintf_with_alloc(const char *fmt, va_list args)
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

str_buf_t
str_buf_printf_with_alloc(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const str_buf_t str_buf = str_buf_vprintf_with_alloc(fmt, args);
    va_end(args);
    return str_buf;
}

void
str_buf_free_buf(str_buf_t *p_str_buf)
{
    app_free_pptr((void **)&p_str_buf->buf);
    p_str_buf->size = 0;
    p_str_buf->idx  = 0;
    p_str_buf->buf  = NULL;
}
