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

void
os_sema_delete(os_sema_t *ph_sema)
{
    if (NULL != *ph_sema)
    {
        vSemaphoreDelete(*ph_sema);
        *ph_sema = NULL;
    }
}

bool
os_sema_wait_with_timeout(os_sema_t h_sema, TickType_t ticks_to_wait)
{
    if (pdTRUE != xSemaphoreTake(h_sema, ticks_to_wait))
    {
        return false;
    }
    return true;
}

void
os_sema_wait_infinite(os_sema_t h_sema)
{
    if (!os_sema_wait_with_timeout(h_sema, portMAX_DELAY))
    {
        assert(0);
    }
}

bool
os_sema_wait_immediate(os_sema_t h_sema)
{
    return os_sema_wait_with_timeout(h_sema, 0);
}

void
os_sema_signal(os_sema_t h_sema)
{
    xSemaphoreGive(h_sema);
}
