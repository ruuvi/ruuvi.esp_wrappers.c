/**
 * @file os_timer_sig.h
 * @author TheSomeMan
 * @date 2020-12-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_TIMER_SIG_H
#define OS_TIMER_SIG_H

#include "os_signal.h"
#include "os_timer.h"
#include "os_wrapper_types.h"
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct os_timer_sig_periodic_t os_timer_sig_periodic_t;
typedef struct os_timer_sig_one_shot_t os_timer_sig_one_shot_t;

typedef struct os_timer_sig_periodic_static_obj_t
{
    void *          stub1;
    void *          stub2;
    os_signal_num_e stub3;
    bool            stub4;
    bool            stub5;
} os_timer_sig_periodic_static_obj_t;

typedef struct os_timer_sig_periodic_static_t
{
    os_timer_sig_periodic_static_obj_t obj_mem;
    os_timer_periodic_static_t         os_timer_mem;
} os_timer_sig_periodic_static_t;

typedef struct os_timer_sig_one_shot_static_obj_t
{
    void *          stub1;
    void *          stub2;
    os_signal_num_e stub3;
    bool            stub4;
    bool            stub5;
} os_timer_sig_one_shot_static_obj_t;

typedef struct os_timer_sig_one_shot_static_t
{
    os_timer_sig_one_shot_static_obj_t obj_mem;
    os_timer_one_shot_static_t         os_timer_mem;
} os_timer_sig_one_shot_static_t;

ATTR_WARN_UNUSED_RESULT
os_timer_sig_periodic_t *
os_timer_sig_periodic_create(
    const char *const      p_timer_name,
    os_signal_t *const     p_signal,
    const os_signal_num_e  sig_num,
    const os_delta_ticks_t period_ticks);

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_sig_periodic_t *
os_timer_sig_periodic_create_static(
    os_timer_sig_periodic_static_t *const p_timer_sig_mem,
    const char *const                     p_timer_name,
    os_signal_t *const                    p_signal,
    const os_signal_num_e                 sig_num,
    const os_delta_ticks_t                period_ticks);

ATTR_WARN_UNUSED_RESULT
os_timer_sig_one_shot_t *
os_timer_sig_one_shot_create(
    const char *const      p_timer_name,
    os_signal_t *const     p_signal,
    const os_signal_num_e  sig_num,
    const os_delta_ticks_t period_ticks);

ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
ATTR_RETURNS_NONNULL
os_timer_sig_one_shot_t *
os_timer_sig_one_shot_create_static(
    os_timer_sig_one_shot_static_t *const p_timer_sig_mem,
    const char *const                     p_timer_name,
    os_signal_t *const                    p_signal,
    const os_signal_num_e                 sig_num,
    const os_delta_ticks_t                period_ticks);

ATTR_NONNULL(1)
void
os_timer_sig_periodic_delete(os_timer_sig_periodic_t **const pp_obj);

ATTR_NONNULL(1)
void
os_timer_sig_one_shot_delete(os_timer_sig_one_shot_t **const pp_obj);

void
os_timer_sig_periodic_start(os_timer_sig_periodic_t *const p_obj);

void
os_timer_sig_one_shot_start(os_timer_sig_one_shot_t *const p_obj);

void
os_timer_sig_periodic_restart(os_timer_sig_periodic_t *const p_obj, const os_delta_ticks_t delay_ticks);

void
os_timer_sig_one_shot_restart(os_timer_sig_one_shot_t *const p_obj, const os_delta_ticks_t delay_ticks);

void
os_timer_sig_periodic_stop(os_timer_sig_periodic_t *const p_obj);

void
os_timer_sig_one_shot_stop(os_timer_sig_one_shot_t *const p_obj);

bool
os_timer_sig_periodic_is_active(os_timer_sig_periodic_t *const p_obj);

bool
os_timer_sig_one_shot_is_active(os_timer_sig_one_shot_t *const p_obj);

void
os_timer_sig_periodic_simulate(os_timer_sig_periodic_t *const p_obj);

void
os_timer_sig_one_shot_simulate(os_timer_sig_one_shot_t *const p_obj);

#ifdef __cplusplus
}
#endif

#endif // OS_TIMER_SIG_H
