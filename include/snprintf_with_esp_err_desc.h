/**
 * @file snprintf_with_esp_err_desc.h
 * @author TheSomeMan
 * @date 2023-04-09
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef RUUVI_SNPRINTF_WITH_ESP_ERR_DESC_H
#define RUUVI_SNPRINTF_WITH_ESP_ERR_DESC_H

#include <esp_err.h>
#include "str_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

str_buf_t
esp_err_to_name_with_alloc_str_buf(const esp_err_t esp_err_code);

ATTR_PRINTF(4, 5)
int
snprintf_with_esp_err_desc(
    const esp_err_t   esp_err_code,
    char* const       p_buf,
    const size_t      buf_size,
    const char* const p_fmt,
    ...);

#ifdef __cplusplus
}
#endif

#endif // RUUVI_SNPRINTF_WITH_ESP_ERR_DESC_H
