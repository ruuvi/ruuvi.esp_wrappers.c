/**
 * @file os_str.h
 * @author TheSomeMan
 * @date 2020-10-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef RUUVI_ESP_WRAPPERS_OS_STR_H
#define RUUVI_ESP_WRAPPERS_OS_STR_H

#include <stdint.h>
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int os_str2num_base_t;

/**
 * @brief This is a wrapper for stdlib strtoul which implements const-correctness and fixes portability issues (MISRA)
 * @param p_str - ptr to a const string
 * @param pp_end - if pp_end is not NULL, it stores the address of the first invalid character in *pp_end
 * @param base - should be in range 2..36 or 0 for auto detection
 * @return the result of the conversion
 */
ATTR_NONNULL(1)
uint32_t
os_str_to_uint32_cptr(
    const char* __restrict const p_str,
    const char** __restrict const pp_end,
    const os_str2num_base_t base);

/**
 * @brief This is a wrapper for stdlib strtoul which implements const-correctness and fixes portability issues (MISRA)
 * @param p_str - ptr to a string
 * @param pp_end - if pp_end is not NULL, it stores the address of the first invalid character in *pp_end
 * @param base - should be in range 2..36 or 0 for auto detection
 * @return the result of the conversion
 */
ATTR_NONNULL(1)
uint32_t
os_str_to_uint32(char* __restrict const p_str, char** __restrict const pp_end, const os_str2num_base_t base);

/**
 * @brief This is a wrapper for stdlib strtol which implements const-correctness and fixes portability issues (MISRA)
 * @param p_str - ptr to a const string
 * @param pp_end - if pp_end is not NULL, it stores the address of the first invalid character in *pp_end
 * @param base - should be in range 2..36 or 0 for auto detection
 * @return the result of the conversion
 */
ATTR_NONNULL(1)
int32_t
os_str_to_int32_cptr(
    const char* __restrict const p_str,
    const char** __restrict const pp_end,
    const os_str2num_base_t base);

/**
 * @brief This is a wrapper for stdlib strtul which implements const-correctness and fixes portability issues (MISRA)
 * @param p_str - ptr to a string
 * @param pp_end - if pp_end is not NULL, it stores the address of the first invalid character in *pp_end
 * @param base - should be in range 2..36 or 0 for auto detection
 * @return the result of the conversion
 */
ATTR_NONNULL(1)
int32_t
os_str_to_int32(char* __restrict const p_str, char** __restrict const pp_end, const os_str2num_base_t base);

#ifdef __cplusplus
}
#endif

#endif // RUUVI_ESP_WRAPPERS_OS_STR_H
