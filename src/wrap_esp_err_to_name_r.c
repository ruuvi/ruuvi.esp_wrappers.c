/**
 * @file wrap_esp_err_to_name_r.c
 * @author TheSomeMan
 * @date 2023-04-09
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "wrap_esp_err_to_name_r.h"
#include <esp_err.h>

#define HAS_MBED_TLS 0

#if defined __has_include
#if __has_include("mbedtls/error.h")
#undef HAS_MBED_TLS
#define HAS_MBED_TLS 1
#endif
#endif

#if HAS_MBED_TLS
#include "mbedtls/error.h"
#include "esp_tls.h"
#endif

const char*
wrap_esp_err_to_name_r(const esp_err_t code, char* const p_buf, const size_t buf_len)
{
    static const char* g_esp_unknown_msg = NULL;
    if (NULL == g_esp_unknown_msg)
    {
        const esp_err_t unknown_esp_err_code = 1;

        g_esp_unknown_msg = esp_err_to_name(unknown_esp_err_code);
    }
    const char* p_err_desc = esp_err_to_name(code);
    if (g_esp_unknown_msg != p_err_desc)
    {
        (void)snprintf(p_buf, buf_len, "%s", p_err_desc);
        return p_buf;
    }

    if ((strerror_r(code, p_buf, buf_len) != NULL) && ('\0' != p_buf[0]))
    {
        return p_buf;
    }

#if HAS_MBED_TLS
    mbedtls_strerror(code, p_buf, buf_len);

    const char* const p_unknown_error_code_prefix = "UNKNOWN ERROR CODE (";
    if (0 == strncmp(p_unknown_error_code_prefix, p_buf, strlen(p_unknown_error_code_prefix)))
    {
        (void)snprintf(p_buf, buf_len, "%s 0x%x(%d)", g_esp_unknown_msg, code, code);
    }
#else
    (void)snprintf(p_buf, buf_len, "%s 0x%x(%d)", g_esp_unknown_msg, code, code);
#endif

    return p_buf;
}
