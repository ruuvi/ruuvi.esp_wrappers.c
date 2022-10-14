/**
 * @file os_mutex_recursive_recursive.h
 * @author TheSomeMan
 * @date 2021-10-08
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_MUTEX_RECURSIVE_H
#define OS_MUTEX_RECURSIVE_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "os_wrapper_types.h"
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t os_mutex_recursive_t;
typedef StaticSemaphore_t os_mutex_recursive_static_t;

/**
 * @brief Create a new mutex object.
 * @return ptr to the instance of os_mutex_recursive_t object.
 */
ATTR_WARN_UNUSED_RESULT
os_mutex_recursive_t
os_mutex_recursive_create(void);

/**
 * @brief Create a new mutex object using pre-allocated memory.
 * @param p_mutex_static - pointer to the pre-allocated memory.
 * @return ptr to the instance of os_mutex_recursive_t object.
 */
#if configSUPPORT_STATIC_ALLOCATION
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_mutex_recursive_t
os_mutex_recursive_create_static(os_mutex_recursive_static_t* const p_mutex_static);
#endif

/**
 * @brief Delete the os_mutex_recursive_t object.
 * @note Do no delete mutex if it is locked by some thread.
 * @param[in,out] ph_mutex - pointer to the variable which contains the mutex handle, it will be cleared after deleting.
 */
ATTR_NONNULL(1)
void
os_mutex_recursive_delete(os_mutex_recursive_t* const ph_mutex);

/**
 * @brief Lock the mutex (wait until it will be locked).
 * @param h_mutex - the mutex handle.
 */
void
os_mutex_recursive_lock(os_mutex_recursive_t const h_mutex);

/**
 * @brief Unlock the mutex.
 * @param h_mutex - the mutex handle.
 */
void
os_mutex_recursive_unlock(os_mutex_recursive_t const h_mutex);

/**
 * @brief Try to lock the mutex (do not wait until it will be locked).
 * @param h_mutex - the mutex handle.
 * @return true if the mutex was locked.
 */
bool
os_mutex_recursive_try_lock(os_mutex_recursive_t const h_mutex);

/**
 * @brief Wait for the specified timeout until the mutex is available and then lock it.
 * @param h_mutex - the mutex handle.
 * @param ticks_to_wait - timeout in system ticks.
 * @return true if the mutex was locked, false if timeout occurred.
 */
bool
os_mutex_recursive_lock_with_timeout(os_mutex_recursive_t const h_mutex, const os_delta_ticks_t ticks_to_wait);

#ifdef __cplusplus
}
#endif

#endif // OS_MUTEX_RECURSIVE_H
