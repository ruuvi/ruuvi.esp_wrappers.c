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

/**
 * @brief Create a timer-object which will call specified callback-function periodically.
 * @param p_timer_name - ptr to a string with the timer name.
 * @param period_ticks - period in system ticks.
 * @param cb_func - ptr to the callback function.
 * @param p_arg - ptr to the argument for the callback function.
 * @return ptr to the new os_timer_periodic_t instance.
 */
ATTR_WARN_UNUSED_RESULT
os_timer_periodic_t *
os_timer_periodic_create(
    const char *const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_periodic_t cb_func,
    void *const                        p_arg);

/**
 * @brief Create a timer-object using pre-allocated memory, which will call specified callback-function periodically.
 * @param p_mem - ptr to the pre-allocated memory for the timer-object instance.
 * @param p_timer_name - ptr to a string with the timer name.
 * @param period_ticks - period in system ticks.
 * @param cb_func - ptr to the callback function.
 * @param p_arg - ptr to the argument for the callback function.
 * @return ptr to the os_timer_periodic_t instance located in pre-allocated memory.
 */
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

/**
 * @brief Create a timer-object which will call specified callback-function once.
 * @param p_timer_name - ptr to a string with the timer name.
 * @param period_ticks - delay in system ticks before calling the callback-function.
 * @param cb_func - ptr to the callback function.
 * @param p_arg - ptr to the argument for the callback function.
 * @return ptr to the new os_timer_one_shot_t instance.
 */
ATTR_WARN_UNUSED_RESULT
os_timer_one_shot_t *
os_timer_one_shot_create(
    const char *const                  p_timer_name,
    const os_delta_ticks_t             period_ticks,
    const os_timer_callback_one_shot_t cb_func,
    void *const                        p_arg);

/**
 * @brief Create a timer-object using pre-allocated memory, which will call specified callback-function once.
 * @param p_mem - ptr to the pre-allocated memory for the timer-object instance.
 * @param p_timer_name - ptr to a string with the timer name.
 * @param period_ticks - delay in system ticks before calling the callback-function.
 * @param cb_func - ptr to the callback function.
 * @param p_arg - ptr to the argument for the callback function.
 * @return ptr to the os_timer_one_shot_t instance located in pre-allocated memory.
 */
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

/**
 * @brief Remove the periodic timer.
 * @param pp_timer - ptr to the variable which contains pointer to the timer object instance,
 * it will be cleared after deleting.
 */
ATTR_NONNULL(1)
void
os_timer_periodic_delete(os_timer_periodic_t **const pp_timer);

/**
 * @brief Remove the one-shot timer.
 * @param pp_timer - ptr to the variable which contains pointer to the timer object instance,
 * it will be cleared after deleting.
 */
ATTR_NONNULL(1)
void
os_timer_one_shot_delete(os_timer_one_shot_t **const pp_timer);

/**
 * @brief Start the periodic timer.
 * @param p_timer - ptr to the timer object instance.
 */
void
os_timer_periodic_start(os_timer_periodic_t *const p_timer);

/**
 * @brief Start the one-shot timer.
 * @param p_timer - ptr to the timer object instance.
 */
void
os_timer_one_shot_start(os_timer_one_shot_t *const p_timer);

/**
 * @brief Stop the periodic timer.
 * @param p_timer - ptr to the timer object instance.
 */
void
os_timer_periodic_stop(os_timer_periodic_t *const p_timer);

/**
 * @brief Stop the one-shot timer.
 * @param p_timer - ptr to the timer object instance.
 */
void
os_timer_one_shot_stop(os_timer_one_shot_t *const p_timer);

/**
 * @brief Restart the periodic timer.
 * @param p_timer - ptr to the timer object instance.
 */
void
os_timer_periodic_restart(os_timer_periodic_t *const p_timer, const os_delta_ticks_t period_ticks);

/**
 * @brief Restart the one-shot timer.
 * @param p_timer - ptr to the timer object instance.
 */
void
os_timer_one_shot_restart(os_timer_one_shot_t *const p_timer, const os_delta_ticks_t delay_ticks);

/**
 * @brief Check if the periodic timer is active.
 * @param p_timer - ptr to the timer object instance.
 * @return true if the timer is active.
 */
bool
os_timer_periodic_is_active(os_timer_periodic_t *const p_timer);

/**
 * @brief Check if the one-shot timer is active.
 * @param p_timer - ptr to the timer object instance.
 * @return true if the timer is active.
 */
bool
os_timer_one_shot_is_active(os_timer_one_shot_t *const p_timer);

/**
 * @brief Simulate the triggering of the periodic timer - call the callback function.
 * @param p_timer - ptr to the timer object instance.
 */
void
os_timer_periodic_simulate(os_timer_periodic_t *const p_timer);

/**
 * @brief Simulate the triggering of the one-shot timer - call the callback function.
 * @param p_timer - ptr to the timer object instance.
 */
void
os_timer_one_shot_simulate(os_timer_one_shot_t *const p_timer);

#ifdef __cplusplus
}
#endif

#endif // OS_TIMER_H
