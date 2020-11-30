/**
 * @file os_time.c
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_time.h"

time_t
os_time_get(void)
{
    return time(NULL);
}
