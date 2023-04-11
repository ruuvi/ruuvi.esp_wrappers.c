/**
 * @file snprintf_with_esp_err_desc.c
 * @author TheSomeMan
 * @date 2023-04-09
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "snprintf_with_esp_err_desc.h"
#include <string.h>
#include <esp_err.h>
#include "os_malloc.h"
#include "str_buf.h"
#include "wrap_esp_err_to_name_r.h"

#define ERR_DESC_SIZE (120)

str_buf_t
esp_err_to_name_with_alloc_str_buf(const esp_err_t esp_err_code)
{
    char* p_err_desc_buf = os_malloc(ERR_DESC_SIZE);
    if (NULL == p_err_desc_buf)
    {
        return str_buf_init_null();
    }
    str_buf_t str_buf = STR_BUF_INIT(p_err_desc_buf, ERR_DESC_SIZE);
    wrap_esp_err_to_name_r(esp_err_code, p_err_desc_buf, ERR_DESC_SIZE);
    str_buf.idx = strlen(p_err_desc_buf);
    return str_buf;
}

int
snprintf_with_esp_err_desc(
    const esp_err_t   esp_err_code,
    char* const       p_buf,
    const size_t      buf_size,
    const char* const p_fmt,
    ...)
{
    int idx = 0;
    if (NULL != p_fmt)
    {
        va_list args;
        va_start(args, p_fmt);
        idx = vsnprintf(p_buf, buf_size, p_fmt, args);
        va_end(args);
    }
    if (idx < 0)
    {
        return idx;
    }
    char* const       p_extra_buf      = (NULL != p_buf) ? &p_buf[idx] : NULL;
    const size_t      remain_len       = ((size_t)idx < buf_size) ? buf_size - (size_t)idx : 0;
    const char* const p_delimiter      = (0 != idx) ? ", " : "";
    str_buf_t         str_buf_err_desc = esp_err_to_name_with_alloc_str_buf(esp_err_code);
    if (NULL != str_buf_err_desc.buf)
    {
        idx += snprintf(p_extra_buf, remain_len, "%serror %d (%s)", p_delimiter, esp_err_code, str_buf_err_desc.buf);
    }
    else
    {
        idx += snprintf(p_extra_buf, remain_len, "%serror %d", p_delimiter, esp_err_code);
    }
    if (NULL != str_buf_err_desc.buf)
    {
        str_buf_free_buf(&str_buf_err_desc);
    }
    return idx;
}
