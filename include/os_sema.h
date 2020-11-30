/**
 * @file os_sema.h
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_SEMA_H
#define OS_SEMA_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t os_sema_t;

os_sema_t
os_sema_create(void);

void
os_sema_delete(os_sema_t *ph_sema);

void
os_sema_wait_infinite(os_sema_t h_sema);

bool
os_sema_wait_immediate(os_sema_t h_sema);

bool
os_sema_wait_with_timeout(os_sema_t h_sema, TickType_t ticks_to_wait);

void
os_sema_signal(os_sema_t h_sema);

#ifdef __cplusplus
}
#endif

#endif // OS_SEMA_H
