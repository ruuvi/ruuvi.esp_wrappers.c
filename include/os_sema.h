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

/**
 * @brief Create a new binary semaphore object.
 * @return ptr to the instance of os_sema_t object.
 */
ATTR_WARN_UNUSED_RESULT
os_sema_t
os_sema_create(void);

/**
 * @brief Create a new binary semaphore object using pre-allocated memory.
 * @param p_sema_static - pointer to the pre-allocated memory.
 * @return ptr to the instance of os_sema_t object.
 */
#if configSUPPORT_STATIC_ALLOCATION
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_sema_t
os_sema_create_static(os_sema_static_t *const p_sema_static);
#endif

/**
 * @brief Delete the os_sema_t object.
 * @param[in,out] ph_sema - pointer to the variable which contains the semaphore handle,
 * it will be cleared after deleting.
 */
ATTR_NONNULL(1)
void
os_sema_delete(os_sema_t *const ph_sema);

/**
 * @brief Wait until the semaphore is available and acquire it.
 * @param h_sema - the semaphore handle.
 */
void
os_sema_wait_infinite(os_sema_t const h_sema);

/**
 * @brief Check if the semaphore is available and acquire it (do not wait).
 * @param h_sema - the semaphore handle.
 * @return true if the semaphore was acquired.
 */
bool
os_sema_wait_immediate(os_sema_t const h_sema);

/**
 * @brief Wait for the specified timeout until the semaphore is available and then acquire it.
 * @param h_sema - the semaphore handle.
 * @param ticks_to_wait - timeout in system ticks.
 * @return true if the semaphore was acquired, false if timeout occurred.
 */
bool
os_sema_wait_with_timeout(os_sema_t const h_sema, const os_delta_ticks_t ticks_to_wait);

/**
 * @brief Release the semaphore.
 * @param h_sema - the semaphore handle.
 */
void
os_sema_signal(os_sema_t const h_sema);

#ifdef __cplusplus
}
#endif

#endif // OS_SEMA_H
