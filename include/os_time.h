/**
 * @file os_time.h
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_TIME_H
#define OS_TIME_H

#include <time.h>
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OS_TIME_MS_TO_TICKS(ms) pdMS_TO_TICKS(ms)

time_t
os_time_get(void);

#ifdef __cplusplus
}
#endif

#endif // OS_TIME_H
