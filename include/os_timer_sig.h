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
    void*            stub1;
    void*            stub2;
    os_signal_num_e  stub3;
    os_delta_ticks_t stub4;
    TickType_t       stub5;
    bool             stub6;
    bool             stub7;
    bool             stub8;
} os_timer_sig_periodic_static_obj_t;

typedef struct os_timer_sig_periodic_static_t
{
    os_timer_sig_periodic_static_obj_t obj_mem;
    os_timer_periodic_static_t         os_timer_mem;
} os_timer_sig_periodic_static_t;

typedef struct os_timer_sig_one_shot_static_obj_t
{
    void*            stub1;
    void*            stub2;
    os_signal_num_e  stub3;
    os_delta_ticks_t stub4;
    bool             stub5;
    bool             stub6;
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
os_timer_sig_periodic_t*
os_timer_sig_periodic_create(
    const char* const      p_timer_name,
    os_signal_t* const     p_signal,
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
os_timer_sig_periodic_t*
os_timer_sig_periodic_create_static(
    os_timer_sig_periodic_static_t* const p_timer_sig_mem,
    const char* const                     p_timer_name,
    os_signal_t* const                    p_signal,
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
os_timer_sig_one_shot_t*
os_timer_sig_one_shot_create(
    const char* const      p_timer_name,
    os_signal_t* const     p_signal,
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
os_timer_sig_one_shot_t*
os_timer_sig_one_shot_create_static(
    os_timer_sig_one_shot_static_t* const p_timer_sig_mem,
    const char* const                     p_timer_name,
    os_signal_t* const                    p_signal,
    const os_signal_num_e                 sig_num,
    const os_delta_ticks_t                period_ticks);

/**
 * @brief Delete the os_timer_sig_periodic_t object instance.
 * @param pp_obj - ptr to the variable which contains pointer to the os_timer_sig_periodic_t object instance,
 * it will be cleared after deleting.
 */
ATTR_NONNULL(1)
void
os_timer_sig_periodic_delete(os_timer_sig_periodic_t** const pp_obj);

/**
 * @brief Delete the os_timer_sig_one_shot_t object instance.
 * @param pp_obj - ptr to the variable which contains pointer to the os_timer_sig_one_shot_t object instance,
 * it will be cleared after deleting.
 */
ATTR_NONNULL(1)
void
os_timer_sig_one_shot_delete(os_timer_sig_one_shot_t** const pp_obj);

/**
 * @brief Returns period of the periodic timer.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
os_delta_ticks_t
os_timer_sig_periodic_get_period(os_timer_sig_periodic_t* const p_obj);

/**
 * @brief Returns period of the one-shot timer.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
os_delta_ticks_t
os_timer_sig_one_shot_get_period(os_timer_sig_one_shot_t* const p_obj);

/**
 * @brief Activate the periodic timer to send the specified signal.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
void
os_timer_sig_periodic_start(os_timer_sig_periodic_t* const p_obj);

/**
 * @brief Activate the one-shot timer to send the specified signal.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 */
void
os_timer_sig_one_shot_start(os_timer_sig_one_shot_t* const p_obj);

/**
 * @brief Restarts the periodic timer which sends the specified signal.
 *
 * This function is responsible for reconfiguring the timer for periodic notifications.
 * If the timer was not initially running, it starts it. For already started timers, it recalculates
 * the next trigger time based on the elapsed time since the last trigger.
 * In cases where the recalculated activation time is already past due, the next activation will be determined
 * relative to the current moment.
 *
 * @note If the argument delay_ticks is zero, then the timer will be stopped.
 *
 * @param p_obj       Pointer to the os_timer_sig_periodic_t object instance.
 * @param delay_ticks Period for the timer, specified in system ticks.
 * @param flag_reset_active_timer If true, then restart an active timer from the current moment.
 */
void
os_timer_sig_periodic_restart_with_period(
    os_timer_sig_periodic_t* const p_obj,
    const os_delta_ticks_t         delay_ticks,
    const bool                     flag_reset_active_timer);

/**
 * @brief Restarts the one-shot timer which sends the specified signal.
 *
 * This function is designed to manage the one-shot timer notifications.
 * If the timer wasn't initially active, it will initiate it.
 * For already active timers, the function recalculates the next trigger time considering the elapsed time
 * since the last activation.
 *
 * @note If the argument delay_ticks is zero, then the timer will be stopped.
 *
 * @param p_obj       Pointer to the os_timer_sig_one_shot_t object instance.
 * @param delay_ticks Delay in system ticks before dispatching the signal.
 * @param flag_reset_active_timer If true, then restart an active timer from the current moment.
 */
void
os_timer_sig_one_shot_restart_with_period(
    os_timer_sig_one_shot_t* const p_obj,
    const os_delta_ticks_t         delay_ticks,
    const bool                     flag_reset_active_timer);

/**
 * @brief Updates the timestamp associated with a periodic signal timer when the timer was triggered.
 *
 * This function is used to update the timestamp value associated with a periodic signal timer
 * when the timer was triggered. It takes a pointer to the `os_timer_sig_periodic_t` object and
 * updates the `timestamp` field with the specified system tick counter.
 *
 * @param p_obj Pointer to the `os_timer_sig_periodic_t` object.
 * @param timestamp The timestamp value to update.
 */
void
os_timer_sig_periodic_update_timestamp_when_timer_was_triggered(
    os_timer_sig_periodic_t* const p_obj,
    const TickType_t               timestamp);

/**
 * @brief Stop and restart the periodic timer which sends the specified signal.
 *
 * If the timer wasn't initially active, it will initiate it.
 * This function will restart the timer from the last time it was triggered (or from the current moment).
 * For already active timers, the function stops it and then calculates the next trigger time.
 * If the time since the last activation exceeds the configured period, the timer will be triggered immediately
 * and the signal will be sent and a timer with the specified period will be activated.
 * If the time elapsed since the last actuation is less than the configured period, the timer will be configured
 * for the remaining time, and after it is triggered, it will be reconfigured for the specified period.
 *
 * @param p_obj Pointer to the os_timer_sig_periodic_t object instance.
 * @param flag_restart_from_current_moment If true, then restart the timer from the current moment.
 */
void
os_timer_sig_periodic_relaunch(os_timer_sig_periodic_t* const p_obj, bool flag_restart_from_current_moment);

/**
 * @brief Stop and restart the one-shot timer which sends the specified signal from the current moment.
 *
 * If the timer wasn't initially active, it will initiate it.
 * For already active timers, the function stops it and restarts from the current moment.
 *
 * @param p_obj Pointer to the os_timer_sig_one_shot_t object instance.
 */
void
os_timer_sig_one_shot_relaunch(os_timer_sig_one_shot_t* const p_obj);

/**
 * @brief Stop the periodic timer which sends the specified signal.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
void
os_timer_sig_periodic_stop(os_timer_sig_periodic_t* const p_obj);

/**
 * @brief Stop the one-shot timer which sends the specified signal.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 */
void
os_timer_sig_one_shot_stop(os_timer_sig_one_shot_t* const p_obj);

/**
 * @brief Check if the periodic timer is active.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 * @return true if the timer is active.
 */
bool
os_timer_sig_periodic_is_active(os_timer_sig_periodic_t* const p_obj);

/**
 * @brief Check if the one-shot timer is active.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 * @return true if the timer is active.
 */
bool
os_timer_sig_one_shot_is_active(os_timer_sig_one_shot_t* const p_obj);

/**
 * @brief Simulate the triggering of the periodic timer - send the specified signal.
 * @param p_obj - ptr to the os_timer_sig_periodic_t object instance.
 */
void
os_timer_sig_periodic_simulate(os_timer_sig_periodic_t* const p_obj);

/**
 * @brief Simulate the triggering of the one-shot timer - send the specified signal.
 * @param p_obj - ptr to the os_timer_sig_one_shot_t object instance.
 */
void
os_timer_sig_one_shot_simulate(os_timer_sig_one_shot_t* const p_obj);

#ifdef __cplusplus
}
#endif

#endif // OS_TIMER_SIG_H
