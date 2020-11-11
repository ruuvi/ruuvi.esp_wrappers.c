/**
 * @file app_str.c
 * @author TheSomeMan
 * @date 2020-10-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "app_str.h"
#include <stdlib.h>

typedef unsigned long app_strtoul_result_t;
typedef long          app_strtol_result_t;

uint32_t
app_strtoul_cptr(const char *__restrict p_str, const char **__restrict pp_end, const str2num_base_t base)
{
    app_strtoul_result_t result = strtoul(p_str, (char **)pp_end, base);
    if (result >= UINT32_MAX)
    {
        result = UINT32_MAX;
    }
    return (uint32_t)result;
}

uint32_t
app_strtoul(char *__restrict p_str, char **__restrict pp_end, const str2num_base_t base)
{
    app_strtoul_result_t result = strtoul(p_str, pp_end, base);
    if (result >= UINT32_MAX)
    {
        result = UINT32_MAX;
    }
    return (uint32_t)result;
}

int32_t
app_strtol_cptr(const char *__restrict p_str, const char **__restrict pp_end, const str2num_base_t base)
{
    app_strtol_result_t result = strtol(p_str, (char **)pp_end, base);
    if (result >= INT32_MAX)
    {
        result = INT32_MAX;
    }
    else if (result <= INT32_MIN)
    {
        result = INT32_MIN;
    }
    else
    {
        // MISRA C:2004, 14.10 - All if...else if constructs shall be terminated with an else clause.
        // MISRA C:2012, 15.7 - All if...else if constructs shall be terminated with an else statement
    }
    return (int32_t)result;
}

int32_t
app_strtol(char *__restrict p_str, char **__restrict pp_end, const str2num_base_t base)
{
    app_strtol_result_t result = strtol(p_str, pp_end, base);
    if (result >= INT32_MAX)
    {
        result = INT32_MAX;
    }
    else if (result <= INT32_MIN)
    {
        result = INT32_MIN;
    }
    else
    {
        // MISRA C:2004, 14.10 - All if...else if constructs shall be terminated with an else clause.
        // MISRA C:2012, 15.7 - All if...else if constructs shall be terminated with an else statement
    }
    return (int32_t)result;
}
