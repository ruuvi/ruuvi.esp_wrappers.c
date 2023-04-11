/**
 * @file wrap_esp_err_to_name_r.h
 * @author TheSomeMan
 * @date 2023-04-09
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef RUUVI_WRAP_ESP_ERR_TO_NAME_R_H
#define RUUVI_WRAP_ESP_ERR_TO_NAME_R_H

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

const char*
wrap_esp_err_to_name_r(const esp_err_t code, char* const p_buf, const size_t buf_len);

#ifdef __cplusplus
}
#endif

#endif // RUUVI_WRAP_ESP_ERR_TO_NAME_R_H
