/**
 * @file os_mutex_recursive_recursive.c
 * @author TheSomeMan
 * @date 2021-10-08
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_mutex_recursive.h"
#include <assert.h>

ATTR_WARN_UNUSED_RESULT
os_mutex_recursive_t
os_mutex_recursive_create(void)
{
    SemaphoreHandle_t h_mutex = xSemaphoreCreateRecursiveMutex();
    return h_mutex;
}

#if configSUPPORT_STATIC_ALLOCATION
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_mutex_recursive_t
os_mutex_recursive_create_static(os_mutex_recursive_static_t *const p_mutex_static)
{
    SemaphoreHandle_t h_mutex = xSemaphoreCreateRecursiveMutexStatic(p_mutex_static);
    return h_mutex;
}
#endif

ATTR_NONNULL(1)
void
os_mutex_recursive_delete(os_mutex_recursive_t *const ph_mutex)
{
    if (NULL != *ph_mutex)
    {
        vSemaphoreDelete(*ph_mutex);
        *ph_mutex = NULL;
    }
}

bool
os_mutex_recursive_lock_with_timeout(os_mutex_recursive_t const h_mutex, const os_delta_ticks_t ticks_to_wait)
{
    assert(NULL != h_mutex);
    if (pdTRUE != xSemaphoreTakeRecursive(h_mutex, ticks_to_wait))
    {
        return false;
    }
    return true;
}

void
os_mutex_recursive_lock(os_mutex_recursive_t const h_mutex)
{
    assert(NULL != h_mutex);
    if (!os_mutex_recursive_lock_with_timeout(h_mutex, OS_DELTA_TICKS_INFINITE))
    {
        assert(0);
    }
}

bool
os_mutex_recursive_try_lock(os_mutex_recursive_t const h_mutex)
{
    assert(NULL != h_mutex);
    return os_mutex_recursive_lock_with_timeout(h_mutex, OS_DELTA_TICKS_IMMEDIATE);
}

void
os_mutex_recursive_unlock(os_mutex_recursive_t const h_mutex)
{
    assert(NULL != h_mutex);
    xSemaphoreGiveRecursive(h_mutex);
}
