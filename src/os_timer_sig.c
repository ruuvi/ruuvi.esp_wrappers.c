/**
 * @file os_timer_sig.c
 * @author TheSomeMan
 * @date 2020-12-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_timer_sig.h"
#include "os_timer.h"
#include "os_signal.h"
#include "os_wrapper_types.h"
#include "app_malloc.h"
#include "attribs.h"

struct os_timer_sig_periodic_t
{
    os_timer_periodic_t *p_timer;
    os_signal_t *        p_signal;
    os_signal_num_e      sig_num;
    bool                 is_static;
    bool                 is_active;
};

_Static_assert(
    sizeof(os_timer_sig_periodic_t) == sizeof(os_timer_sig_periodic_static_obj_t),
    "os_timer_sig_periodic_t != os_timer_sig_periodic_static_t");

struct os_timer_sig_one_shot_t
{
    os_timer_one_shot_t *p_timer;
    os_signal_t *        p_signal;
    os_signal_num_e      sig_num;
    bool                 is_static;
    bool                 is_active;
};

_Static_assert(
    sizeof(os_timer_sig_one_shot_t) == sizeof(os_timer_sig_one_shot_static_obj_t),
    "os_timer_sig_one_shot_t != os_timer_sig_one_shot_static_t");

static void
os_timer_sig_cb_periodic(ATTR_UNUSED os_timer_periodic_t *p_timer, void *p_arg)
{
    if (NULL == p_arg)
    {
        return;
    }
    os_timer_sig_periodic_t *p_obj = p_arg;
    if (p_obj->is_active)
    {
        os_signal_send(p_obj->p_signal, p_obj->sig_num);
    }
}

static void
os_timer_sig_cb_one_shot(ATTR_UNUSED os_timer_one_shot_t *p_timer, void *p_arg)
{
    if (NULL == p_arg)
    {
        return;
    }
    os_timer_sig_one_shot_t *p_obj = p_arg;
    if (p_obj->is_active)
    {
        p_obj->is_active = false;
        os_signal_send(p_obj->p_signal, p_obj->sig_num);
    }
}

os_timer_sig_periodic_t *
os_timer_sig_periodic_create(
    const char *const      p_timer_name,
    os_signal_t *          p_signal,
    const os_signal_num_e  sig_num,
    const os_delta_ticks_t period_ticks)
{
    os_timer_sig_periodic_t *p_obj = app_calloc(1, sizeof(*p_obj));
    if (NULL == p_obj)
    {
        return NULL;
    }
    p_obj->p_signal  = p_signal;
    p_obj->sig_num   = sig_num;
    p_obj->is_static = false;
    p_obj->is_active = false;
    p_obj->p_timer   = os_timer_periodic_create(p_timer_name, period_ticks, &os_timer_sig_cb_periodic, p_obj);
    if (NULL == p_obj->p_timer)
    {
        os_timer_sig_periodic_delete(&p_obj);
        return NULL;
    }
    return p_obj;
}

os_timer_sig_periodic_t *
os_timer_sig_periodic_create_static(
    os_timer_sig_periodic_static_t *p_timer_sig_mem,
    const char *const               p_timer_name,
    os_signal_t *                   p_signal,
    const os_signal_num_e           sig_num,
    const os_delta_ticks_t          period_ticks)
{
    os_timer_sig_periodic_t *p_obj = (os_timer_sig_periodic_t *)&p_timer_sig_mem->obj_mem;

    p_obj->p_signal  = p_signal;
    p_obj->sig_num   = sig_num;
    p_obj->is_static = true;
    p_obj->is_active = false;

    p_obj->p_timer = os_timer_periodic_create_static(
        &p_timer_sig_mem->os_timer_mem,
        p_timer_name,
        period_ticks,
        &os_timer_sig_cb_periodic,
        p_obj);
    return p_obj;
}

os_timer_sig_one_shot_t *
os_timer_sig_one_shot_create(
    const char *const      p_timer_name,
    os_signal_t *          p_signal,
    const os_signal_num_e  sig_num,
    const os_delta_ticks_t period_ticks)
{
    os_timer_sig_one_shot_t *p_obj = app_calloc(1, sizeof(*p_obj));
    if (NULL == p_obj)
    {
        return NULL;
    }
    p_obj->p_signal  = p_signal;
    p_obj->sig_num   = sig_num;
    p_obj->is_static = false;
    p_obj->is_active = false;
    p_obj->p_timer   = os_timer_one_shot_create(p_timer_name, period_ticks, &os_timer_sig_cb_one_shot, p_obj);
    if (NULL == p_obj->p_timer)
    {
        os_timer_sig_one_shot_delete(&p_obj);
        return NULL;
    }
    return p_obj;
}

os_timer_sig_one_shot_t *
os_timer_sig_one_shot_create_static(
    os_timer_sig_one_shot_static_t *p_timer_sig_mem,
    const char *const               p_timer_name,
    os_signal_t *                   p_signal,
    const os_signal_num_e           sig_num,
    const os_delta_ticks_t          period_ticks)
{
    os_timer_sig_one_shot_t *p_obj = (os_timer_sig_one_shot_t *)&p_timer_sig_mem->obj_mem;

    p_obj->p_signal  = p_signal;
    p_obj->sig_num   = sig_num;
    p_obj->is_static = true;
    p_obj->is_active = false;

    p_obj->p_timer = os_timer_one_shot_create_static(
        &p_timer_sig_mem->os_timer_mem,
        p_timer_name,
        period_ticks,
        &os_timer_sig_cb_one_shot,
        p_obj);
    return p_obj;
}

void
os_timer_sig_periodic_delete(os_timer_sig_periodic_t **pp_obj)
{
    os_timer_sig_periodic_t *p_obj = *pp_obj;
    if (NULL == p_obj)
    {
        return;
    }
    p_obj->is_active = false;
    if (NULL != p_obj->p_timer)
    {
        os_timer_periodic_delete(&p_obj->p_timer);
    }
    p_obj->p_signal = NULL;
    p_obj->sig_num  = OS_SIGNAL_NUM_NONE;
    if (!p_obj->is_static)
    {
        app_free_pptr((void **)pp_obj);
    }
}

void
os_timer_sig_one_shot_delete(os_timer_sig_one_shot_t **pp_obj)
{
    os_timer_sig_one_shot_t *p_obj = *pp_obj;
    if (NULL == p_obj)
    {
        return;
    }
    p_obj->is_active = false;
    if (NULL != p_obj->p_timer)
    {
        os_timer_one_shot_delete(&p_obj->p_timer);
    }
    p_obj->p_signal = NULL;
    p_obj->sig_num  = OS_SIGNAL_NUM_NONE;
    if (!p_obj->is_static)
    {
        app_free_pptr((void **)pp_obj);
    }
}

void
os_timer_sig_periodic_start(os_timer_sig_periodic_t *p_obj)
{
    p_obj->is_active = true;
    os_timer_periodic_start(p_obj->p_timer);
}

void
os_timer_sig_one_shot_start(os_timer_sig_one_shot_t *p_obj)
{
    p_obj->is_active = true;
    os_timer_one_shot_start(p_obj->p_timer);
}

void
os_timer_sig_periodic_restart(os_timer_sig_periodic_t *p_obj, const os_delta_ticks_t delay_ticks)
{
    p_obj->is_active = true;
    os_timer_periodic_restart(p_obj->p_timer, delay_ticks);
}

void
os_timer_sig_one_shot_restart(os_timer_sig_one_shot_t *p_obj, const os_delta_ticks_t delay_ticks)
{
    p_obj->is_active = true;
    os_timer_one_shot_restart(p_obj->p_timer, delay_ticks);
}

void
os_timer_sig_periodic_stop(os_timer_sig_periodic_t *p_obj)
{
    p_obj->is_active = false;
    os_timer_periodic_stop(p_obj->p_timer);
}

void
os_timer_sig_one_shot_stop(os_timer_sig_one_shot_t *p_obj)
{
    p_obj->is_active = false;
    os_timer_one_shot_stop(p_obj->p_timer);
}

bool
os_timer_sig_periodic_is_active(os_timer_sig_periodic_t *p_obj)
{
    return p_obj->is_active;
}

bool
os_timer_sig_one_shot_is_active(os_timer_sig_one_shot_t *p_obj)
{
    return p_obj->is_active;
}

void
os_timer_sig_periodic_simulate(os_timer_sig_periodic_t *p_obj)
{
    os_timer_periodic_simulate(p_obj->p_timer);
}

void
os_timer_sig_one_shot_simulate(os_timer_sig_one_shot_t *p_obj)
{
    os_timer_one_shot_simulate(p_obj->p_timer);
}
