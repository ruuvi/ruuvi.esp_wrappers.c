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

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t os_mutex_t;

os_mutex_t
os_mutex_create(void);

void
os_mutex_delete(os_mutex_t *ph_mutex);

void
os_mutex_lock(os_mutex_t h_mutex);

void
os_mutex_unlock(os_mutex_t h_mutex);

bool
os_mutex_try_lock(os_mutex_t h_mutex);

bool
os_mutex_lock_with_timeout(os_mutex_t h_mutex, TickType_t ticks_to_wait);

#ifdef __cplusplus
}
#endif

#endif // OS_MUTEX_H
