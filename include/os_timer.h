/**
 * @file os_timer.h
 * @author TheSomeMan
 * @date 2020-12-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_TIMER_H
#define OS_TIMER_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "os_wrapper_types.h"
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct os_timer_periodic_t os_timer_periodic_t;
typedef struct os_timer_one_shot_t os_timer_one_shot_t;

typedef struct os_timer_periodic_static_obj_t
{
    void *stub1;
    void *stub2;
    void *stub3;
    bool  stub4;
} os_timer_periodic_static_obj_t;

typedef struct os_timer_periodic_static_t
{
    os_timer_periodic_static_obj_t obj_mem;
    StaticTimer_t                  timer_mem;
} os_timer_periodic_static_t;

typedef struct os_timer_one_shot_static_obj_t
{
    void *stub1;
    void *stub2;
    void *stub3;
    bool  stub4;
} os_timer_one_shot_static_obj_t;

typedef struct os_timer_one_shot_static_t
{
    os_timer_one_shot_static_obj_t obj_mem;
    StaticTimer_t                  timer_mem;
} os_timer_one_shot_static_t;

typedef void (*os_timer_callback_periodic_t)(os_timer_periodic_t *p_timer, void *p_arg);
typedef void (*os_timer_callback_one_shot_t)(os_timer_one_shot_t *p_timer, void *p_arg);

ATTR_WARN_UNUSED_RESULT
os_timer_periodic_t *
os_timer_periodic_create(
    const char *const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_periodic_t cb_func,
    void *const                        p_arg);

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_periodic_t *
os_timer_periodic_create_static(
    os_timer_periodic_static_t *const  p_mem,
    const char *const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_periodic_t cb_func,
    void *const                        p_arg);

ATTR_WARN_UNUSED_RESULT
os_timer_one_shot_t *
os_timer_one_shot_create(
    const char *const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_one_shot_t cb_func,
    void *const                        p_arg);

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_one_shot_t *
os_timer_one_shot_create_static(
    os_timer_one_shot_static_t *const  p_mem,
    const char *const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_one_shot_t cb_func,
    void *const                        p_arg);

ATTR_NONNULL(1)
void
os_timer_periodic_delete(os_timer_periodic_t **const pp_timer);

ATTR_NONNULL(1)
void
os_timer_one_shot_delete(os_timer_one_shot_t **const pp_timer);

void
os_timer_periodic_stop(os_timer_periodic_t *const p_timer);

void
os_timer_one_shot_stop(os_timer_one_shot_t *const p_timer);

void
os_timer_periodic_start(os_timer_periodic_t *const p_timer);

void
os_timer_one_shot_start(os_timer_one_shot_t *const p_timer);

void
os_timer_periodic_restart(os_timer_periodic_t *const p_timer, const os_delta_ticks_t period_ticks);

void
os_timer_one_shot_restart(os_timer_one_shot_t *const p_timer, const os_delta_ticks_t delay_ticks);

bool
os_timer_periodic_is_active(os_timer_periodic_t *const p_timer);

bool
os_timer_one_shot_is_active(os_timer_one_shot_t *const p_timer);

void
os_timer_periodic_simulate(os_timer_periodic_t *const p_timer);

void
os_timer_one_shot_simulate(os_timer_one_shot_t *const p_timer);

#ifdef __cplusplus
}
#endif

#endif // OS_TIMER_H
