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

/**
 * @brief Create a timer-sig-object which will send a @ref os_signal_t periodically.
 * @param p_timer_name - ptr to a string with the timer name.
 * @param p_signal - ptr to a @ref os_signal_t object instance.
 * @param sig_num - the signal number, @ref os_signal_num_e
 * @param period_ticks - period in system ticks.
 * @return ptr to the new os_timer_sig_periodic_t instance.
 */
ATTR_WARN_UNUSED_RESULT
os_timer_sig_periodic_t *
os_timer_sig_periodic_create(
    const char *const      p_timer_name,
    os_signal_t *const     p_signal,
    const os_signal_num_e  sig_num,
    const os_delta_ticks_t period_ticks);

/**
 * @brief Create a timer-sig-object using pre-allocated memory, which will send a @ref os_signal_t periodically.
 * @param p_timer_sig_mem - to the pre-allocated memory for the timer-sig-object instance.
 * @param p_timer_name - ptr to a string with the timer name.
 * @param p_signal - ptr to a @ref os_signal_t object instance.
 * @param sig_num - the signal number, @ref os_signal_num_e
 * @param period_ticks - period in system ticks.
 * @return ptr to the os_timer_sig_periodic_t instance located in pre-allocated memory.
 */
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

/**
 * @brief Create a timer-sig-object which will send a @ref os_signal_t once.
 * @param p_timer_name - ptr to a string with the timer name.
 * @param p_signal - ptr to a @ref os_signal_t object instance.
 * @param sig_num - the signal number, @ref os_signal_num_e
 * @param period_ticks - delay in system ticks before sending the signal.
 * @return ptr to the new os_timer_sig_one_shot_t instance.
 */
ATTR_WARN_UNUSED_RESULT
os_timer_sig_one_shot_t *
os_timer_sig_one_shot_create(
    const char *const      p_timer_name,
    os_signal_t *const     p_signal,
    const os_signal_num_e  sig_num,
    const os_delta_ticks_t period_ticks);

/**
 * @brief Create a timer-sig-object using pre-allocated memory, which will send a @ref os_signal_t once.
 * @param p_timer_sig_mem - to the pre-allocated memory for the timer-sig-object instance.
 * @param p_timer_name - ptr to a string with the timer name.
 * @param p_signal - ptr to a @ref os_signal_t object instance.
 * @param sig_num - the signal number, @ref os_signal_num_e
 * @param period_ticks - delay in system ticks before sending the signal.
 * @return ptr to the os_timer_sig_one_shot_t instance located in pre-allocated memory.
 */
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

/**
 * @brief Delete the os_timer_sig_periodic_t object instance.
 * @param pp_obj - ptr to the variable which contains pointer to the os_timer_sig_periodic_t object instance,
 * it will be cleared after deleting.
 */
ATTR_NONNULL(1)
void
os_timer_sig_periodic_delete(os_timer_sig_periodic_t **const pp_obj);

/**
 * @brief Delete the os_timer_sig_one_shot_t object instance.
 * @param pp_obj - ptr to the variable which contains pointer to the os_timer_sig_one_shot_t object instance,
 * it will be cleared after deleting.
 */
ATTR_NONNULL(1)
void
os_timer_sig_one_shot_delete(os_timer_sig_one_shot_t **const pp_obj);

/**
 * @brief Activate the periodic timer to send the specified signal.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
void
os_timer_sig_periodic_start(os_timer_sig_periodic_t *const p_obj);

/**
 * @brief Activate the one-shot timer to send the specified signal.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 */
void
os_timer_sig_one_shot_start(os_timer_sig_one_shot_t *const p_obj);

/**
 * @brief Restart the periodic timer which sends the specified signal.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
void
os_timer_sig_periodic_restart(os_timer_sig_periodic_t *const p_obj, const os_delta_ticks_t delay_ticks);

/**
 * @brief Restart the one-shot timer which sends the specified signal.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 */
void
os_timer_sig_one_shot_restart(os_timer_sig_one_shot_t *const p_obj, const os_delta_ticks_t delay_ticks);

/**
 * @brief Stop the periodic timer which sends the specified signal.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
void
os_timer_sig_periodic_stop(os_timer_sig_periodic_t *const p_obj);

/**
 * @brief Stop the one-shot timer which sends the specified signal.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 */
void
os_timer_sig_one_shot_stop(os_timer_sig_one_shot_t *const p_obj);

/**
 * @brief Check if the periodic timer is active.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 * @return true if the timer is active.
 */
bool
os_timer_sig_periodic_is_active(os_timer_sig_periodic_t *const p_obj);

/**
 * @brief Check if the one-shot timer is active.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 * @return true if the timer is active.
 */
bool
os_timer_sig_one_shot_is_active(os_timer_sig_one_shot_t *const p_obj);

/**
 * @brief Simulate the triggering of the periodic timer - send the specified signal.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
void
os_timer_sig_periodic_simulate(os_timer_sig_periodic_t *const p_obj);

/**
 * @brief Simulate the triggering of the one-shot timer - send the specified signal.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 */
void
os_timer_sig_one_shot_simulate(os_timer_sig_one_shot_t *const p_obj);

#ifdef __cplusplus
}
#endif

#endif // OS_TIMER_SIG_H
