/**
 * @file os_wrapper_types.h
 * @author TheSomeMan
 * @date 2020-12-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_WRAPPER_TYPES_H
#define OS_WRAPPER_TYPES_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef TickType_t os_delta_ticks_t;

#define OS_DELTA_TICKS_IMMEDIATE             (0U)
#define OS_DELTA_TICKS_INFINITE              (portMAX_DELAY)
#define OS_DELTA_TICKS_MS_TO_TICKS(delay_ms) pdMS_TO_TICKS(delay_ms)

#ifdef __cplusplus
}
#endif

#endif // OS_WRAPPER_TYPES_H
