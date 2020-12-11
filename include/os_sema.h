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
#include "os_wrapper_types.h"
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t os_sema_t;
typedef StaticSemaphore_t os_sema_static_t;

ATTR_WARN_UNUSED_RESULT
os_sema_t
os_sema_create(void);

#if configSUPPORT_STATIC_ALLOCATION
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_sema_t
os_sema_create_static(os_sema_static_t *const p_sema_static);
#endif

ATTR_NONNULL(1)
void
os_sema_delete(os_sema_t *const ph_sema);

void
os_sema_wait_infinite(os_sema_t const h_sema);

bool
os_sema_wait_immediate(os_sema_t const h_sema);

bool
os_sema_wait_with_timeout(os_sema_t const h_sema, const os_delta_ticks_t ticks_to_wait);

void
os_sema_signal(os_sema_t const h_sema);

#ifdef __cplusplus
}
#endif

#endif // OS_SEMA_H
