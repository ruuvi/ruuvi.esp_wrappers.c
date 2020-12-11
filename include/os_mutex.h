/**
 * @file os_mutex.h
 * @author TheSomeMan
 * @date 2020-11-22
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_MUTEX_H
#define OS_MUTEX_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "os_wrapper_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t os_mutex_t;
typedef StaticSemaphore_t os_mutex_static_t;

os_mutex_t
os_mutex_create(void);

#if configSUPPORT_STATIC_ALLOCATION
os_mutex_t
os_mutex_create_static(os_mutex_static_t *const p_mutex_static);
#endif

void
os_mutex_delete(os_mutex_t *const ph_mutex);

void
os_mutex_lock(os_mutex_t const h_mutex);

void
os_mutex_unlock(os_mutex_t const h_mutex);

bool
os_mutex_try_lock(os_mutex_t const h_mutex);

bool
os_mutex_lock_with_timeout(os_mutex_t const h_mutex, const os_delta_ticks_t ticks_to_wait);

#ifdef __cplusplus
}
#endif

#endif // OS_MUTEX_H
