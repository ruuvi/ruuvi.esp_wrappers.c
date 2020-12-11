/**
 * @file os_mutex.c
 * @author TheSomeMan
 * @date 2020-11-22
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_mutex.h"
#include <assert.h>

ATTR_WARN_UNUSED_RESULT
os_mutex_t
os_mutex_create(void)
{
    SemaphoreHandle_t h_mutex = xSemaphoreCreateMutex();
    return h_mutex;
}

#if configSUPPORT_STATIC_ALLOCATION
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_mutex_t
os_mutex_create_static(os_mutex_static_t *const p_mutex_static)
{
    SemaphoreHandle_t h_mutex = xSemaphoreCreateMutexStatic(p_mutex_static);
    return h_mutex;
}
#endif

ATTR_NONNULL(1)
void
os_mutex_delete(os_mutex_t *const ph_mutex)
{
    if (NULL != *ph_mutex)
    {
        vSemaphoreDelete(*ph_mutex);
        *ph_mutex = NULL;
    }
}

bool
os_mutex_lock_with_timeout(os_mutex_t const h_mutex, const os_delta_ticks_t ticks_to_wait)
{
    assert(NULL != h_mutex);
    if (pdTRUE != xSemaphoreTake(h_mutex, ticks_to_wait))
    {
        return false;
    }
    return true;
}

void
os_mutex_lock(os_mutex_t const h_mutex)
{
    assert(NULL != h_mutex);
    if (!os_mutex_lock_with_timeout(h_mutex, OS_DELTA_TICKS_INFINITE))
    {
        assert(0);
    }
}

bool
os_mutex_try_lock(os_mutex_t const h_mutex)
{
    assert(NULL != h_mutex);
    return os_mutex_lock_with_timeout(h_mutex, OS_DELTA_TICKS_IMMEDIATE);
}

void
os_mutex_unlock(os_mutex_t const h_mutex)
{
    assert(NULL != h_mutex);
    xSemaphoreGive(h_mutex);
}
