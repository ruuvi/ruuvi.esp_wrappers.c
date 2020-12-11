/**
 * @file os_str.c
 * @author TheSomeMan
 * @date 2020-10-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_str.h"
#include <stdlib.h>

typedef unsigned long os_strtoul_result_t;
typedef long          os_strtol_result_t;

ATTR_NONNULL(1)
uint32_t
os_str_to_uint32_cptr(
    const char *__restrict const p_str,
    const char **__restrict const pp_end,
    const os_str2num_base_t base)
{
    os_strtoul_result_t result = strtoul(p_str, (char **)pp_end, base);
    if (result >= UINT32_MAX)
    {
        return UINT32_MAX;
    }
    return (uint32_t)result;
}

ATTR_NONNULL(1)
uint32_t
os_str_to_uint32(char *__restrict const p_str, char **__restrict const pp_end, const os_str2num_base_t base)
{
    os_strtoul_result_t result = strtoul(p_str, pp_end, base);
    if (result >= UINT32_MAX)
    {
        return UINT32_MAX;
    }
    return (uint32_t)result;
}

ATTR_NONNULL(1)
int32_t
os_str_to_int32_cptr(
    const char *__restrict const p_str,
    const char **__restrict const pp_end,
    const os_str2num_base_t base)
{
    os_strtol_result_t result = strtol(p_str, (char **)pp_end, base);
    if (result >= INT32_MAX)
    {
        return INT32_MAX;
    }
    if (result <= INT32_MIN)
    {
        return INT32_MIN;
    }
    return (int32_t)result;
}

ATTR_NONNULL(1)
int32_t
os_str_to_int32(char *__restrict const p_str, char **__restrict const pp_end, const os_str2num_base_t base)
{
    os_strtol_result_t result = strtol(p_str, pp_end, base);
    if (result >= INT32_MAX)
    {
        return INT32_MAX;
    }
    if (result <= INT32_MIN)
    {
        return INT32_MIN;
    }
    return (int32_t)result;
}
