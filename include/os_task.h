/**
 * @file os_task.h
 * @author TheSomeMan
 * @date 2020-11-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_TASK_H
#define OS_TASK_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "os_wrapper_types.h"
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ATTR_NORETURN void (*os_task_func_t)(void *p_param);
typedef ATTR_NORETURN void (*os_task_func_const_param_t)(const void *p_param);
typedef ATTR_NORETURN void (*os_task_func_without_param_t)(void);
typedef void (*os_task_finite_func_with_param_t)(void *p_param);
typedef void (*os_task_finite_func_with_const_param_t)(const void *p_param);
typedef void (*os_task_finite_func_without_param_t)(void);
typedef UBaseType_t  os_task_priority_t;
typedef TaskHandle_t os_task_handle_t;
typedef StackType_t  os_task_stack_type_t;
typedef StaticTask_t os_task_static_t;

/**
 * Create a new task thread.
 * @param p_func - pointer to the task function which never returns.
 * @param p_name - pointer to the task name
 * @param stack_depth - the size of the task stack (in bytes)
 * @param p_param - pointer that will be passed as the parameter to the task function
 * @param priority - task priority
 * @param[out] ph_task - pointer to the variable to return task handle
 * @return true if successful
 */
bool
os_task_create(
    const os_task_func_t     p_func,
    const char *const        p_name,
    const uint32_t           stack_depth,
    void *const              p_param,
    const os_task_priority_t priority,
    os_task_handle_t *const  ph_task);

/**
 * Create a new task thread.
 * @param p_func - pointer to the task function which never returns and takes pointer to 'const' as an argument.
 * @param p_name - pointer to the task name
 * @param stack_depth - the size of the task stack (in bytes)
 * @param p_param - pointer that will be passed as the parameter to the task function
 * @param priority - task priority
 * @param[out] ph_task - pointer to the variable to return task handle
 * @return true if successful
 */
bool
os_task_create_with_const_param(
    const os_task_func_const_param_t p_func,
    const char *const                p_name,
    const uint32_t                   stack_depth,
    const void *const                p_param,
    const os_task_priority_t         priority,
    os_task_handle_t *const          ph_task);

/**
 * Create a new task thread.
 * @param p_func - pointer to the task function which never returns and takes no param.
 * @param p_name - pointer to the task name
 * @param stack_depth - the size of the task stack (in bytes)
 * @param priority - task priority
 * @param[out] ph_task - pointer to the variable to return task handle
 * @return true if successful
 */
bool
os_task_create_without_param(
    const os_task_func_without_param_t p_func,
    const char *const                  p_name,
    const uint32_t                     stack_depth,
    const os_task_priority_t           priority,
    os_task_handle_t *const            ph_task);

/**
 * Create a new task thread.
 * @param p_func - pointer to the task function which may return.
 * @param p_name - pointer to the task name
 * @param stack_depth - the size of the task stack (in bytes)
 * @param p_param - pointer that will be passed as the parameter to the task function
 * @param priority - task priority
 * @return true if successful
 */
bool
os_task_create_finite(
    const os_task_finite_func_with_param_t p_func,
    const char *const                      p_name,
    const uint32_t                         stack_depth,
    void *const                            p_param,
    const os_task_priority_t               priority);

/**
 * Create a new task thread.
 * @param p_func - pointer to the task function which may return and takes pointer to 'const' as an argument.
 * @param p_name - pointer to the task name
 * @param stack_depth - the size of the task stack (in bytes)
 * @param p_param - pointer that will be passed as the parameter to the task function
 * @param priority - task priority
 * @return true if successful
 */
bool
os_task_create_finite_with_const_param(
    const os_task_finite_func_with_const_param_t p_func,
    const char *const                            p_name,
    const uint32_t                               stack_depth,
    const void *const                            p_param,
    const os_task_priority_t                     priority);

/**
 * Create a new task thread.
 * @param p_func - pointer to the task function which may return and takes no param.
 * @param p_name - pointer to the task name
 * @param stack_depth - the size of the task stack (in bytes)
 * @param priority - task priority
 * @return true if successful
 */
bool
os_task_create_finite_without_param(
    const os_task_finite_func_without_param_t p_func,
    const char *const                         p_name,
    const uint32_t                            stack_depth,
    const os_task_priority_t                  priority);

/**
 * Create a new task thread without using memory allocation from the heap.
 * @param p_func - pointer to the task function which never returns.
 * @param p_name - pointer to the task name
 * @param p_stack_mem - pointer to the statically allocated buffer for stack
 * @param stack_depth - the size of the task stack (in bytes)
 * @param p_param - pointer that will be passed as the parameter to the task function
 * @param priority - task priority
 * @param p_task_mem - pointer to the statically allocated buffer for @ref os_task_static_t
 * @param[out] ph_task - pointer to the variable to return task handle
 * @return true if successful
 */
bool
os_task_create_static(
    const os_task_func_t        p_func,
    const char *const           p_name,
    os_task_stack_type_t *const p_stack_mem,
    const uint32_t              stack_depth,
    void *const                 p_param,
    const os_task_priority_t    priority,
    os_task_static_t *const     p_task_mem,
    os_task_handle_t *const     ph_task);

/**
 * Create a new task thread without using memory allocation from the heap.
 * @param p_func - pointer to the task function which never returns and takes pointer to 'const' as an argument.
 * @param p_name - pointer to the task name
 * @param p_stack_mem - pointer to the statically allocated buffer for stack
 * @param stack_depth - the size of the task stack (in bytes)
 * @param p_param - pointer that will be passed as the parameter to the task function
 * @param priority - task priority
 * @param p_task_mem - pointer to the statically allocated buffer for @ref os_task_static_t
 * @param[out] ph_task - pointer to the variable to return task handle
 * @return true if successful
 */
bool
os_task_create_static_with_const_param(
    const os_task_func_const_param_t p_func,
    const char *const                p_name,
    os_task_stack_type_t *const      p_stack_mem,
    const uint32_t                   stack_depth,
    const void *const                p_param,
    const os_task_priority_t         priority,
    os_task_static_t *const          p_task_mem,
    os_task_handle_t *const          ph_task);

/**
 * Create a new task thread without using memory allocation from the heap.
 * @param p_func - pointer to the task function which never returns and takes no param.
 * @param p_name - pointer to the task name
 * @param p_stack_mem - pointer to the statically allocated buffer for stack
 * @param stack_depth - the size of the task stack (in bytes)
 * @param priority - task priority
 * @param p_task_mem - pointer to the statically allocated buffer for @ref os_task_static_t
 * @param[out] ph_task - pointer to the variable to return task handle
 * @return true if successful
 */
bool
os_task_create_static_without_param(
    const os_task_func_without_param_t p_func,
    const char *const                  p_name,
    os_task_stack_type_t *const        p_stack_mem,
    const uint32_t                     stack_depth,
    const os_task_priority_t           priority,
    os_task_static_t *const            p_task_mem,
    os_task_handle_t *const            ph_task);

/**
 * Create a new task thread without using memory allocation from the heap.
 * @param p_func - pointer to the task function which may return.
 * @param p_name - pointer to the task name
 * @param p_stack_mem - pointer to the statically allocated buffer for stack
 * @param stack_depth - the size of the task stack (in bytes)
 * @param p_param - pointer that will be passed as the parameter to the task function
 * @param priority - task priority
 * @param p_task_mem - pointer to the statically allocated buffer for @ref os_task_static_t
 * @return true if successful
 */
bool
os_task_create_static_finite(
    const os_task_finite_func_with_param_t p_func,
    const char *const                      p_name,
    os_task_stack_type_t *const            p_stack_mem,
    const uint32_t                         stack_depth,
    void *const                            p_param,
    const os_task_priority_t               priority,
    os_task_static_t *const                p_task_mem);

/**
 * Create a new task thread without using memory allocation from the heap.
 * @param p_func - pointer to the task function which may return and takes pointer to 'const' as an argument.
 * @param p_name - pointer to the task name
 * @param p_stack_mem - pointer to the statically allocated buffer for stack
 * @param stack_depth - the size of the task stack (in bytes)
 * @param p_param - pointer that will be passed as the parameter to the task function
 * @param priority - task priority
 * @param p_task_mem - pointer to the statically allocated buffer for @ref os_task_static_t
 * @return true if successful
 */
bool
os_task_create_static_finite_with_const_param(
    const os_task_finite_func_with_const_param_t p_func,
    const char *const                            p_name,
    os_task_stack_type_t *const                  p_stack_mem,
    const uint32_t                               stack_depth,
    const void *const                            p_param,
    const os_task_priority_t                     priority,
    os_task_static_t *const                      p_task_mem);

/**
 * Create a new task thread without using memory allocation from the heap.
 * @param p_func - pointer to the task function which may return and takes no param.
 * @param p_name - pointer to the task name
 * @param p_stack_mem - pointer to the statically allocated buffer for stack
 * @param stack_depth - the size of the task stack (in bytes)
 * @param priority - task priority
 * @param p_task_mem - pointer to the statically allocated buffer for @ref os_task_static_t
 * @return true if successful
 */
bool
os_task_create_static_finite_without_param(
    const os_task_finite_func_without_param_t p_func,
    const char *const                         p_name,
    os_task_stack_type_t *const               p_stack_mem,
    const uint32_t                            stack_depth,
    const os_task_priority_t                  priority,
    os_task_static_t *const                   p_task_mem);

/**
 * Remove the task from the RTOS kernel's scheduler.
 * @note All resources (dynamic memory, sockets, file descriptors, ...) allocated by the task will remain allocated.
 * @param ph_task - ptr to variable which contains the task handle, this variable will be automatically cleared.
 */
void
os_task_delete(os_task_handle_t *const ph_task);

/**
 * Get task name for the current thread.
 * @return pointer to the string with the current task name.
 */
const char *
os_task_get_name(void);

/**
 * Delay a task for a given number of ticks.
 * @param delay_ticks
 */
void
os_task_delay(const os_delta_ticks_t delay_ticks);

/**
 * Get the handle of the current task.
 * @return handle of the current task - @ref os_task_handle_t
 */
os_task_handle_t
os_task_get_cur_task_handle(void);

#ifdef __cplusplus
}
#endif

#endif // OS_TASK_H
