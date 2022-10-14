/**
 * @file os_timer.c
 * @author TheSomeMan
 * @date 2020-12-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_timer.h"
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "os_wrapper_types.h"
#include "os_task.h"
#include "os_malloc.h"

struct os_timer_periodic_t
{
    TimerHandle_t                h_timer;
    os_timer_callback_periodic_t cb_func;
    void*                        p_arg;
    bool                         is_static;
};

struct os_timer_periodic_without_arg_t
{
    TimerHandle_t                            h_timer;
    os_timer_callback_periodic_without_arg_t cb_func;
    void*                                    p_stub;
    bool                                     is_static;
};

struct os_timer_periodic_const_arg_t
{
    TimerHandle_t                          h_timer;
    os_timer_callback_periodic_const_arg_t cb_func;
    const void*                            p_arg;
    bool                                   is_static;
};

struct os_timer_periodic_cptr_t
{
    TimerHandle_t                     h_timer;
    os_timer_callback_periodic_cptr_t cb_func;
    void*                             p_arg;
    bool                              is_static;
};

struct os_timer_periodic_cptr_without_arg_t
{
    TimerHandle_t                                 h_timer;
    os_timer_callback_periodic_cptr_without_arg_t cb_func;
    void*                                         p_stub;
    bool                                          is_static;
};

struct os_timer_periodic_cptr_const_arg_t
{
    TimerHandle_t                               h_timer;
    os_timer_callback_periodic_cptr_const_arg_t cb_func;
    const void*                                 p_arg;
    bool                                        is_static;
};

_Static_assert(
    sizeof(os_timer_periodic_t) == sizeof(os_timer_periodic_static_obj_t),
    "os_timer_periodic_t != os_timer_periodic_static_obj_t");

struct os_timer_one_shot_t
{
    TimerHandle_t                h_timer;
    os_timer_callback_one_shot_t cb_func;
    void*                        p_arg;
    bool                         is_static;
};

struct os_timer_one_shot_without_arg_t
{
    TimerHandle_t                            h_timer;
    os_timer_callback_one_shot_without_arg_t cb_func;
    void*                                    p_stub;
    bool                                     is_static;
};

struct os_timer_one_shot_const_arg_t
{
    TimerHandle_t                          h_timer;
    os_timer_callback_one_shot_const_arg_t cb_func;
    const void*                            p_arg;
    bool                                   is_static;
};

struct os_timer_one_shot_cptr_t
{
    TimerHandle_t                     h_timer;
    os_timer_callback_one_shot_cptr_t cb_func;
    void*                             p_arg;
    bool                              is_static;
};

struct os_timer_one_shot_cptr_without_arg_t
{
    TimerHandle_t                                 h_timer;
    os_timer_callback_one_shot_cptr_without_arg_t cb_func;
    void*                                         p_stub;
    bool                                          is_static;
};

struct os_timer_one_shot_cptr_const_arg_t
{
    TimerHandle_t                               h_timer;
    os_timer_callback_one_shot_cptr_const_arg_t cb_func;
    const void*                                 p_arg;
    bool                                        is_static;
};

_Static_assert(
    sizeof(os_timer_one_shot_t) == sizeof(os_timer_one_shot_static_obj_t),
    "os_timer_one_shot_t != os_timer_one_shot_static_obj_t");

_Static_assert(
    sizeof(os_timer_one_shot_without_arg_t) == sizeof(os_timer_one_shot_static_obj_t),
    "os_timer_one_shot_without_arg_t != os_timer_one_shot_static_obj_t");

_Static_assert(
    sizeof(os_timer_one_shot_const_arg_t) == sizeof(os_timer_one_shot_static_obj_t),
    "os_timer_one_shot_const_arg_t != os_timer_one_shot_static_obj_t");

_Static_assert(
    sizeof(os_timer_one_shot_cptr_t) == sizeof(os_timer_one_shot_static_obj_t),
    "os_timer_one_shot_cptr_t != os_timer_one_shot_static_obj_t");

_Static_assert(
    sizeof(os_timer_one_shot_cptr_without_arg_t) == sizeof(os_timer_one_shot_static_obj_t),
    "os_timer_one_shot_cptr_without_arg_t != os_timer_one_shot_static_obj_t");

_Static_assert(
    sizeof(os_timer_one_shot_cptr_const_arg_t) == sizeof(os_timer_one_shot_static_obj_t),
    "os_timer_one_shot_cptr_const_arg_t != os_timer_one_shot_static_obj_t");

static void
os_timer_callback_periodic(TimerHandle_t h_timer)
{
    os_timer_periodic_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

static void
os_timer_callback_periodic_without_arg(TimerHandle_t h_timer)
{
    os_timer_periodic_without_arg_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer);
}

static void
os_timer_callback_periodic_const_arg(TimerHandle_t h_timer)
{
    os_timer_periodic_const_arg_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

static void
os_timer_callback_periodic_cptr(TimerHandle_t h_timer)
{
    os_timer_periodic_cptr_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

static void
os_timer_callback_periodic_cptr_without_arg(TimerHandle_t h_timer)
{
    os_timer_periodic_cptr_without_arg_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer);
}

static void
os_timer_callback_periodic_cptr_const_arg(TimerHandle_t h_timer)
{
    os_timer_periodic_cptr_const_arg_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

static void
os_timer_callback_one_shot(TimerHandle_t h_timer)
{
    os_timer_one_shot_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

static void
os_timer_callback_one_shot_without_arg(TimerHandle_t h_timer)
{
    os_timer_one_shot_without_arg_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer);
}

static void
os_timer_callback_one_shot_const_arg(TimerHandle_t h_timer)
{
    os_timer_one_shot_const_arg_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

static void
os_timer_callback_one_shot_cptr(TimerHandle_t h_timer)
{
    os_timer_one_shot_cptr_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

static void
os_timer_callback_one_shot_cptr_without_arg(TimerHandle_t h_timer)
{
    os_timer_one_shot_cptr_without_arg_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer);
}

static void
os_timer_callback_one_shot_cptr_const_arg(TimerHandle_t h_timer)
{
    os_timer_one_shot_cptr_const_arg_t* p_timer = pvTimerGetTimerID(h_timer);
    if (NULL == p_timer)
    {
        // in case if os_timer_delete was called, but it has not been handled yet
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

ATTR_WARN_UNUSED_RESULT
os_timer_periodic_t*
os_timer_periodic_create(
    const char* const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_periodic_t p_cb_func,
    void* const                        p_arg)
{
    os_timer_periodic_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->p_arg     = p_arg;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(p_timer_name, period_ticks, pdTRUE, p_timer, &os_timer_callback_periodic);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_periodic_without_arg_t*
os_timer_periodic_without_arg_create(
    const char* const                              p_timer_name,
    const os_delta_ticks_t                         period_ticks,
    const os_timer_callback_periodic_without_arg_t p_cb_func)
{
    os_timer_periodic_without_arg_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic_without_arg);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_periodic_const_arg_t*
os_timer_periodic_const_arg_create(
    const char* const                            p_timer_name,
    const os_delta_ticks_t                       period_ticks,
    const os_timer_callback_periodic_const_arg_t p_cb_func,
    const void* const                            p_arg)
{
    os_timer_periodic_const_arg_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->p_arg     = p_arg;
    p_timer->is_static = false;
    p_timer->h_timer = xTimerCreate(p_timer_name, period_ticks, pdTRUE, p_timer, &os_timer_callback_periodic_const_arg);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_periodic_cptr_t*
os_timer_periodic_cptr_create(
    const char* const                       p_timer_name,
    const os_delta_ticks_t                  period_ticks,
    const os_timer_callback_periodic_cptr_t p_cb_func,
    void* const                             p_arg)
{
    os_timer_periodic_cptr_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->p_arg     = p_arg;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(p_timer_name, period_ticks, pdTRUE, p_timer, &os_timer_callback_periodic_cptr);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_periodic_cptr_without_arg_t*
os_timer_periodic_cptr_without_arg_create(
    const char* const                                   p_timer_name,
    const os_delta_ticks_t                              period_ticks,
    const os_timer_callback_periodic_cptr_without_arg_t p_cb_func)
{
    os_timer_periodic_cptr_without_arg_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic_cptr_without_arg);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_periodic_cptr_const_arg_t*
os_timer_periodic_cptr_const_arg_create(
    const char* const                                 p_timer_name,
    const os_delta_ticks_t                            period_ticks,
    const os_timer_callback_periodic_cptr_const_arg_t p_cb_func,
    const void* const                                 p_arg)
{
    os_timer_periodic_cptr_const_arg_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->p_arg     = p_arg;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic_cptr_const_arg);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_periodic_t*
os_timer_periodic_create_static(
    os_timer_periodic_static_t* const  p_mem,
    const char* const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_periodic_t p_cb_func,
    void* const                        p_arg)
{
    os_timer_periodic_t* const p_timer = (os_timer_periodic_t*)&p_mem->obj_mem;
    p_timer->cb_func                   = p_cb_func;
    p_timer->p_arg                     = p_arg;
    p_timer->is_static                 = true;
    p_timer->h_timer                   = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_periodic_without_arg_t*
os_timer_periodic_without_arg_create_static(
    os_timer_periodic_static_t* const              p_mem,
    const char* const                              p_timer_name,
    const os_delta_ticks_t                         period_ticks,
    const os_timer_callback_periodic_without_arg_t p_cb_func)
{
    os_timer_periodic_without_arg_t* const p_timer = (os_timer_periodic_without_arg_t*)&p_mem->obj_mem;
    p_timer->cb_func                               = p_cb_func;
    p_timer->is_static                             = true;
    p_timer->h_timer                               = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic_without_arg,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_periodic_const_arg_t*
os_timer_periodic_const_arg_create_static(
    os_timer_periodic_static_t* const            p_mem,
    const char* const                            p_timer_name,
    const os_delta_ticks_t                       period_ticks,
    const os_timer_callback_periodic_const_arg_t p_cb_func,
    const void* const                            p_arg)
{
    os_timer_periodic_const_arg_t* const p_timer = (os_timer_periodic_const_arg_t*)&p_mem->obj_mem;
    p_timer->cb_func                             = p_cb_func;
    p_timer->p_arg                               = p_arg;
    p_timer->is_static                           = true;
    p_timer->h_timer                             = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic_const_arg,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_periodic_cptr_t*
os_timer_periodic_cptr_create_static(
    os_timer_periodic_static_t* const       p_mem,
    const char* const                       p_timer_name,
    const os_delta_ticks_t                  period_ticks,
    const os_timer_callback_periodic_cptr_t p_cb_func,
    void* const                             p_arg)
{
    os_timer_periodic_cptr_t* const p_timer = (os_timer_periodic_cptr_t*)&p_mem->obj_mem;
    p_timer->cb_func                        = p_cb_func;
    p_timer->p_arg                          = p_arg;
    p_timer->is_static                      = true;
    p_timer->h_timer                        = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic_cptr,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_periodic_cptr_without_arg_t*
os_timer_periodic_cptr_without_arg_create_static(
    os_timer_periodic_static_t* const                   p_mem,
    const char* const                                   p_timer_name,
    const os_delta_ticks_t                              period_ticks,
    const os_timer_callback_periodic_cptr_without_arg_t p_cb_func)
{
    os_timer_periodic_cptr_without_arg_t* const p_timer = (os_timer_periodic_cptr_without_arg_t*)&p_mem->obj_mem;
    p_timer->cb_func                                    = p_cb_func;
    p_timer->is_static                                  = true;
    p_timer->h_timer                                    = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic_cptr_without_arg,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_periodic_cptr_const_arg_t*
os_timer_periodic_cptr_const_arg_create_static(
    os_timer_periodic_static_t* const                 p_mem,
    const char* const                                 p_timer_name,
    const os_delta_ticks_t                            period_ticks,
    const os_timer_callback_periodic_cptr_const_arg_t p_cb_func,
    const void* const                                 p_arg)
{
    os_timer_periodic_cptr_const_arg_t* const p_timer = (os_timer_periodic_cptr_const_arg_t*)&p_mem->obj_mem;
    p_timer->cb_func                                  = p_cb_func;
    p_timer->p_arg                                    = p_arg;
    p_timer->is_static                                = true;
    p_timer->h_timer                                  = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdTRUE,
        p_timer,
        &os_timer_callback_periodic_cptr_const_arg,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_one_shot_t*
os_timer_one_shot_create(
    const char* const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_one_shot_t p_cb_func,
    void* const                        p_arg)
{
    os_timer_one_shot_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->p_arg     = p_arg;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(p_timer_name, period_ticks, pdFALSE, p_timer, &os_timer_callback_one_shot);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_one_shot_without_arg_t*
os_timer_one_shot_without_arg_create(
    const char* const                              p_timer_name,
    const os_delta_ticks_t                         period_ticks,
    const os_timer_callback_one_shot_without_arg_t p_cb_func)
{
    os_timer_one_shot_without_arg_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_without_arg);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_one_shot_const_arg_t*
os_timer_one_shot_const_arg_create(
    const char* const                            p_timer_name,
    const os_delta_ticks_t                       period_ticks,
    const os_timer_callback_one_shot_const_arg_t p_cb_func,
    const void* const                            p_arg)
{
    os_timer_one_shot_const_arg_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->p_arg     = p_arg;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_const_arg);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_one_shot_cptr_t*
os_timer_one_shot_cptr_create(
    const char* const                       p_timer_name,
    const os_delta_ticks_t                  period_ticks,
    const os_timer_callback_one_shot_cptr_t p_cb_func,
    void* const                             p_arg)
{
    os_timer_one_shot_cptr_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->p_arg     = p_arg;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(p_timer_name, period_ticks, pdFALSE, p_timer, &os_timer_callback_one_shot_cptr);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_one_shot_cptr_without_arg_t*
os_timer_one_shot_cptr_without_arg_create(
    const char* const                                   p_timer_name,
    const os_delta_ticks_t                              period_ticks,
    const os_timer_callback_one_shot_cptr_without_arg_t p_cb_func)
{
    os_timer_one_shot_cptr_without_arg_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_cptr_without_arg);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
os_timer_one_shot_cptr_const_arg_t*
os_timer_one_shot_cptr_const_arg_create(
    const char* const                                 p_timer_name,
    const os_delta_ticks_t                            period_ticks,
    const os_timer_callback_one_shot_cptr_const_arg_t p_cb_func,
    const void* const                                 p_arg)
{
    os_timer_one_shot_cptr_const_arg_t* p_timer = os_calloc(1, sizeof(*p_timer));
    if (NULL == p_timer)
    {
        return NULL;
    }
    p_timer->cb_func   = p_cb_func;
    p_timer->p_arg     = p_arg;
    p_timer->is_static = false;
    p_timer->h_timer   = xTimerCreate(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_cptr_const_arg);
    if (NULL == p_timer->h_timer)
    {
        os_free(p_timer);
        return NULL;
    }
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_one_shot_t*
os_timer_one_shot_create_static(
    os_timer_one_shot_static_t* const  p_mem,
    const char* const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_one_shot_t p_cb_func,
    void* const                        p_arg)
{
    os_timer_one_shot_t* const p_timer = (os_timer_one_shot_t*)&p_mem->obj_mem;
    p_timer->cb_func                   = p_cb_func;
    p_timer->p_arg                     = p_arg;
    p_timer->is_static                 = true;
    p_timer->h_timer                   = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_one_shot_without_arg_t*
os_timer_one_shot_without_arg_create_static(
    os_timer_one_shot_static_t* const              p_mem,
    const char* const                              p_timer_name,
    const os_delta_ticks_t                         period_ticks,
    const os_timer_callback_one_shot_without_arg_t p_cb_func)
{
    os_timer_one_shot_without_arg_t* const p_timer = (os_timer_one_shot_without_arg_t*)&p_mem->obj_mem;
    p_timer->cb_func                               = p_cb_func;
    p_timer->is_static                             = true;
    p_timer->h_timer                               = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_without_arg,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_one_shot_const_arg_t*
os_timer_one_shot_const_arg_create_static(
    os_timer_one_shot_static_t* const            p_mem,
    const char* const                            p_timer_name,
    const os_delta_ticks_t                       period_ticks,
    const os_timer_callback_one_shot_const_arg_t p_cb_func,
    const void* const                            p_arg)
{
    os_timer_one_shot_const_arg_t* const p_timer = (os_timer_one_shot_const_arg_t*)&p_mem->obj_mem;
    p_timer->cb_func                             = p_cb_func;
    p_timer->p_arg                               = p_arg;
    p_timer->is_static                           = true;
    p_timer->h_timer                             = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_const_arg,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_one_shot_cptr_t*
os_timer_one_shot_cptr_create_static(
    os_timer_one_shot_static_t* const       p_mem,
    const char* const                       p_timer_name,
    const os_delta_ticks_t                  period_ticks,
    const os_timer_callback_one_shot_cptr_t p_cb_func,
    void* const                             p_arg)
{
    os_timer_one_shot_cptr_t* const p_timer = (os_timer_one_shot_cptr_t*)&p_mem->obj_mem;
    p_timer->cb_func                        = p_cb_func;
    p_timer->p_arg                          = p_arg;
    p_timer->is_static                      = true;
    p_timer->h_timer                        = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_cptr,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_one_shot_cptr_without_arg_t*
os_timer_one_shot_cptr_without_arg_create_static(
    os_timer_one_shot_static_t* const                   p_mem,
    const char* const                                   p_timer_name,
    const os_delta_ticks_t                              period_ticks,
    const os_timer_callback_one_shot_cptr_without_arg_t p_cb_func)
{
    os_timer_one_shot_cptr_without_arg_t* const p_timer = (os_timer_one_shot_cptr_without_arg_t*)&p_mem->obj_mem;
    p_timer->cb_func                                    = p_cb_func;
    p_timer->is_static                                  = true;
    p_timer->h_timer                                    = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_cptr_without_arg,
        &p_mem->timer_mem);
    return p_timer;
}

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_one_shot_cptr_const_arg_t*
os_timer_one_shot_cptr_const_arg_create_static(
    os_timer_one_shot_static_t* const                 p_mem,
    const char* const                                 p_timer_name,
    const os_delta_ticks_t                            period_ticks,
    const os_timer_callback_one_shot_cptr_const_arg_t p_cb_func,
    const void* const                                 p_arg)
{
    os_timer_one_shot_cptr_const_arg_t* const p_timer = (os_timer_one_shot_cptr_const_arg_t*)&p_mem->obj_mem;
    p_timer->cb_func                                  = p_cb_func;
    p_timer->p_arg                                    = p_arg;
    p_timer->is_static                                = true;
    p_timer->h_timer                                  = xTimerCreateStatic(
        p_timer_name,
        period_ticks,
        pdFALSE,
        p_timer,
        &os_timer_callback_one_shot_cptr_const_arg,
        &p_mem->timer_mem);
    return p_timer;
}

bool
os_timer_periodic_is_active(os_timer_periodic_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_periodic_without_arg_is_active(os_timer_periodic_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_periodic_const_arg_is_active(os_timer_periodic_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_periodic_cptr_is_active(os_timer_periodic_cptr_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_periodic_cptr_without_arg_is_active(os_timer_periodic_cptr_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_periodic_cptr_const_arg_is_active(os_timer_periodic_cptr_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_one_shot_is_active(os_timer_one_shot_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_one_shot_without_arg_is_active(os_timer_one_shot_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_one_shot_const_arg_is_active(os_timer_one_shot_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_one_shot_cptr_is_active(os_timer_one_shot_cptr_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_one_shot_cptr_without_arg_is_active(os_timer_one_shot_cptr_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

bool
os_timer_one_shot_cptr_const_arg_is_active(os_timer_one_shot_cptr_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return false;
    }
    if (pdFALSE == xTimerIsTimerActive(p_timer->h_timer))
    {
        return false;
    }
    return true;
}

ATTR_NONNULL(1)
void
os_timer_periodic_delete(os_timer_periodic_t** const p_p_timer)
{
    os_timer_periodic_t* p_timer = *p_p_timer;
    *p_p_timer                   = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_periodic_without_arg_delete(os_timer_periodic_without_arg_t** const p_p_timer)
{
    os_timer_periodic_without_arg_t* p_timer = *p_p_timer;
    *p_p_timer                               = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_periodic_const_arg_delete(os_timer_periodic_const_arg_t** const p_p_timer)
{
    os_timer_periodic_const_arg_t* p_timer = *p_p_timer;
    *p_p_timer                             = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_periodic_cptr_delete(os_timer_periodic_cptr_t** const p_p_timer)
{
    os_timer_periodic_cptr_t* p_timer = *p_p_timer;
    *p_p_timer                        = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_periodic_cptr_without_arg_delete(os_timer_periodic_cptr_without_arg_t** const p_p_timer)
{
    os_timer_periodic_cptr_without_arg_t* p_timer = *p_p_timer;
    *p_p_timer                                    = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_periodic_cptr_const_arg_delete(os_timer_periodic_cptr_const_arg_t** const p_p_timer)
{
    os_timer_periodic_cptr_const_arg_t* p_timer = *p_p_timer;
    *p_p_timer                                  = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_one_shot_delete(os_timer_one_shot_t** const p_p_timer)
{
    os_timer_one_shot_t* p_timer = *p_p_timer;
    *p_p_timer                   = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_one_shot_without_arg_delete(os_timer_one_shot_without_arg_t** const p_p_timer)
{
    os_timer_one_shot_without_arg_t* p_timer = *p_p_timer;
    *p_p_timer                               = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_one_shot_const_arg_delete(os_timer_one_shot_const_arg_t** const p_p_timer)
{
    os_timer_one_shot_const_arg_t* p_timer = *p_p_timer;
    *p_p_timer                             = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_one_shot_cptr_delete(os_timer_one_shot_cptr_t** const p_p_timer)
{
    os_timer_one_shot_cptr_t* p_timer = *p_p_timer;
    *p_p_timer                        = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_one_shot_cptr_without_arg_delete(os_timer_one_shot_cptr_without_arg_t** const p_p_timer)
{
    os_timer_one_shot_cptr_without_arg_t* p_timer = *p_p_timer;
    *p_p_timer                                    = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

ATTR_NONNULL(1)
void
os_timer_one_shot_cptr_const_arg_delete(os_timer_one_shot_cptr_const_arg_t** const p_p_timer)
{
    os_timer_one_shot_cptr_const_arg_t* p_timer = *p_p_timer;
    *p_p_timer                                  = NULL;
    if (NULL == p_timer)
    {
        return;
    }
    vTimerSetTimerID(p_timer->h_timer, NULL);
    while (pdPASS != xTimerDelete(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
    // dynamic timers are automatically freed by the Delete command handler
    if (!p_timer->is_static)
    {
        os_free(p_timer);
    }
}

void
os_timer_periodic_stop(os_timer_periodic_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_without_arg_stop(os_timer_periodic_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_const_arg_stop(os_timer_periodic_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_cptr_stop(os_timer_periodic_cptr_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_cptr_without_arg_stop(os_timer_periodic_cptr_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_cptr_const_arg_stop(os_timer_periodic_cptr_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_stop(os_timer_one_shot_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_without_arg_stop(os_timer_one_shot_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_const_arg_stop(os_timer_one_shot_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_cptr_stop(os_timer_one_shot_cptr_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_cptr_without_arg_stop(os_timer_one_shot_cptr_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_cptr_const_arg_stop(os_timer_one_shot_cptr_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerStop(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_start(os_timer_periodic_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_periodic_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_without_arg_start(os_timer_periodic_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_periodic_without_arg_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_const_arg_start(os_timer_periodic_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_periodic_const_arg_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_cptr_start(os_timer_periodic_cptr_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_periodic_cptr_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_cptr_without_arg_start(os_timer_periodic_cptr_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_periodic_cptr_without_arg_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_cptr_const_arg_start(os_timer_periodic_cptr_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_periodic_cptr_const_arg_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_start(os_timer_one_shot_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_one_shot_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_without_arg_start(os_timer_one_shot_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_one_shot_without_arg_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_const_arg_start(os_timer_one_shot_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_one_shot_const_arg_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_cptr_start(os_timer_one_shot_cptr_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_one_shot_cptr_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_cptr_without_arg_start(os_timer_one_shot_cptr_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_one_shot_cptr_without_arg_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_one_shot_cptr_const_arg_start(os_timer_one_shot_cptr_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    if (os_timer_one_shot_cptr_const_arg_is_active(p_timer))
    {
        return;
    }
    while (pdPASS != xTimerStart(p_timer->h_timer, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_periodic_set_period(os_timer_periodic_t* const p_timer, const os_delta_ticks_t delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_periodic_without_arg_set_period(
    os_timer_periodic_without_arg_t* const p_timer,
    const os_delta_ticks_t                 delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_periodic_const_arg_set_period(os_timer_periodic_const_arg_t* const p_timer, const os_delta_ticks_t delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_periodic_cptr_set_period(os_timer_periodic_cptr_t* const p_timer, const os_delta_ticks_t delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_periodic_cptr_without_arg_set_period(
    os_timer_periodic_cptr_without_arg_t* const p_timer,
    const os_delta_ticks_t                      delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_periodic_cptr_const_arg_set_period(
    os_timer_periodic_cptr_const_arg_t* const p_timer,
    const os_delta_ticks_t                    delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_one_shot_set_period(os_timer_one_shot_t* const p_timer, const os_delta_ticks_t delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_one_shot_without_arg_set_period(
    os_timer_one_shot_without_arg_t* const p_timer,
    const os_delta_ticks_t                 delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_one_shot_const_arg_set_period(os_timer_one_shot_const_arg_t* const p_timer, const os_delta_ticks_t delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_one_shot_cptr_set_period(os_timer_one_shot_cptr_t* const p_timer, const os_delta_ticks_t delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_one_shot_cptr_without_arg_set_period(
    os_timer_one_shot_cptr_without_arg_t* const p_timer,
    const os_delta_ticks_t                      delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

static void
os_timer_one_shot_cptr_const_arg_set_period(
    os_timer_one_shot_cptr_const_arg_t* const p_timer,
    const os_delta_ticks_t                    delta_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    while (pdPASS != xTimerChangePeriod(p_timer->h_timer, delta_ticks, 0))
    {
        os_task_delay(1);
    }
}

void
os_timer_periodic_restart(os_timer_periodic_t* const p_timer, const os_delta_ticks_t period_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_periodic_set_period(p_timer, period_ticks);
    os_timer_periodic_start(p_timer);
}

void
os_timer_periodic_without_arg_restart(
    os_timer_periodic_without_arg_t* const p_timer,
    const os_delta_ticks_t                 period_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_periodic_without_arg_set_period(p_timer, period_ticks);
    os_timer_periodic_without_arg_start(p_timer);
}

void
os_timer_periodic_const_arg_restart(os_timer_periodic_const_arg_t* const p_timer, const os_delta_ticks_t period_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_periodic_const_arg_set_period(p_timer, period_ticks);
    os_timer_periodic_const_arg_start(p_timer);
}

void
os_timer_periodic_cptr_restart(os_timer_periodic_cptr_t* const p_timer, const os_delta_ticks_t period_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_periodic_cptr_set_period(p_timer, period_ticks);
    os_timer_periodic_cptr_start(p_timer);
}

void
os_timer_periodic_cptr_without_arg_restart(
    os_timer_periodic_cptr_without_arg_t* const p_timer,
    const os_delta_ticks_t                      period_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_periodic_cptr_without_arg_set_period(p_timer, period_ticks);
    os_timer_periodic_cptr_without_arg_start(p_timer);
}

void
os_timer_periodic_cptr_const_arg_restart(
    os_timer_periodic_cptr_const_arg_t* const p_timer,
    const os_delta_ticks_t                    period_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_periodic_cptr_const_arg_set_period(p_timer, period_ticks);
    os_timer_periodic_cptr_const_arg_start(p_timer);
}

void
os_timer_one_shot_restart(os_timer_one_shot_t* const p_timer, const os_delta_ticks_t delay_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_one_shot_set_period(p_timer, delay_ticks);
    os_timer_one_shot_start(p_timer);
}

void
os_timer_one_shot_without_arg_restart(
    os_timer_one_shot_without_arg_t* const p_timer,
    const os_delta_ticks_t                 delay_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_one_shot_without_arg_set_period(p_timer, delay_ticks);
    os_timer_one_shot_without_arg_start(p_timer);
}

void
os_timer_one_shot_const_arg_restart(os_timer_one_shot_const_arg_t* const p_timer, const os_delta_ticks_t delay_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_one_shot_const_arg_set_period(p_timer, delay_ticks);
    os_timer_one_shot_const_arg_start(p_timer);
}

void
os_timer_one_shot_cptr_restart(os_timer_one_shot_cptr_t* const p_timer, const os_delta_ticks_t delay_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_one_shot_cptr_set_period(p_timer, delay_ticks);
    os_timer_one_shot_cptr_start(p_timer);
}

void
os_timer_one_shot_cptr_without_arg_restart(
    os_timer_one_shot_cptr_without_arg_t* const p_timer,
    const os_delta_ticks_t                      delay_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_one_shot_cptr_without_arg_set_period(p_timer, delay_ticks);
    os_timer_one_shot_cptr_without_arg_start(p_timer);
}

void
os_timer_one_shot_cptr_const_arg_restart(
    os_timer_one_shot_cptr_const_arg_t* const p_timer,
    const os_delta_ticks_t                    delay_ticks)
{
    if (NULL == p_timer)
    {
        return;
    }
    os_timer_one_shot_cptr_const_arg_set_period(p_timer, delay_ticks);
    os_timer_one_shot_cptr_const_arg_start(p_timer);
}

void
os_timer_periodic_simulate(os_timer_periodic_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

void
os_timer_periodic_without_arg_simulate(os_timer_periodic_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer);
}

void
os_timer_periodic_const_arg_simulate(os_timer_periodic_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

void
os_timer_periodic_cptr_simulate(os_timer_periodic_cptr_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

void
os_timer_periodic_cptr_without_arg_simulate(os_timer_periodic_cptr_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer);
}

void
os_timer_periodic_cptr_const_arg_simulate(os_timer_periodic_cptr_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

void
os_timer_one_shot_simulate(os_timer_one_shot_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

void
os_timer_one_shot_without_arg_simulate(os_timer_one_shot_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer);
}

void
os_timer_one_shot_const_arg_simulate(os_timer_one_shot_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

void
os_timer_one_shot_cptr_simulate(os_timer_one_shot_cptr_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}

void
os_timer_one_shot_cptr_without_arg_simulate(os_timer_one_shot_cptr_without_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer);
}

void
os_timer_one_shot_cptr_const_arg_simulate(os_timer_one_shot_cptr_const_arg_t* const p_timer)
{
    if (NULL == p_timer)
    {
        return;
    }
    p_timer->cb_func(p_timer, p_timer->p_arg);
}
