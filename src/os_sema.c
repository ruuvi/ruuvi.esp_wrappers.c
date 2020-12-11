/**
 * @file os_sema.c
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_sema.h"
#include <assert.h>

os_sema_t
os_sema_create(void)
{
    SemaphoreHandle_t h_sema = xSemaphoreCreateBinary();
    return h_sema;
}

#if configSUPPORT_STATIC_ALLOCATION
os_sema_t
os_sema_create_static(os_sema_static_t *p_sema_static)
{
    SemaphoreHandle_t h_sema = xSemaphoreCreateBinaryStatic(p_sema_static);
    return h_sema;
}
#endif

void
os_sema_delete(os_sema_t *const ph_sema)
{
    if (NULL != *ph_sema)
    {
        vSemaphoreDelete(*ph_sema);
        *ph_sema = NULL;
    }
}

bool
os_sema_wait_with_timeout(os_sema_t h_sema, const os_delta_ticks_t ticks_to_wait)
{
    assert(NULL != h_sema);
    if (pdTRUE != xSemaphoreTake(h_sema, ticks_to_wait))
    {
        return false;
    }
    return true;
}

void
os_sema_wait_infinite(os_sema_t h_sema)
{
    assert(NULL != h_sema);
    if (!os_sema_wait_with_timeout(h_sema, OS_DELTA_TICKS_INFINITE))
    {
        assert(0);
    }
}

bool
os_sema_wait_immediate(os_sema_t h_sema)
{
    assert(NULL != h_sema);
    return os_sema_wait_with_timeout(h_sema, OS_DELTA_TICKS_IMMEDIATE);
}

void
os_sema_signal(os_sema_t h_sema)
{
    assert(NULL != h_sema);
    xSemaphoreGive(h_sema);
}
