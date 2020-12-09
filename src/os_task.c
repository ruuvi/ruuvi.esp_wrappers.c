/**
 * @file os_task.c
 * @author TheSomeMan
 * @date 2020-11-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_task.h"
#include <assert.h>
#include "os_malloc.h"
#include "log.h"

typedef struct os_task_arg_finite_with_param_t
{
    os_task_finite_func_with_param_t p_func;
    void *                           p_arg;
} os_task_arg_finite_with_param_t;

typedef struct os_task_arg_finite_with_const_param_t
{
    os_task_finite_func_with_const_param_t p_func;
    const void *                           p_arg;
} os_task_arg_finite_with_const_param_t;

static const char *TAG = "os_task";

static bool
os_task_create_internal(
    os_task_func_t           p_func,
    const char *             p_name,
    const uint32_t           stack_depth,
    void *                   p_param,
    const os_task_priority_t priority,
    os_task_handle_t *       ph_task)
{
    LOG_INFO("Start thread '%s' with priority %d, stack size %u bytes", p_name, priority, stack_depth);
    if (pdPASS != xTaskCreate(p_func, p_name, stack_depth, p_param, priority, ph_task))
    {
        LOG_ERR("Failed to start thread '%s'", p_name);
        return false;
    }
    return true;
}

bool
os_task_create(
    os_task_func_t           p_func,
    const char *             p_name,
    const uint32_t           stack_depth,
    void *                   p_param,
    const os_task_priority_t priority,
    os_task_handle_t *       ph_task)
{
    return os_task_create_internal(p_func, p_name, stack_depth, p_param, priority, ph_task);
}

bool
os_task_create_with_const_param(
    os_task_func_const_param_t p_func,
    const char *               p_name,
    const uint32_t             stack_depth,
    const void *               p_param,
    const os_task_priority_t   priority,
    os_task_handle_t *         ph_task)
{
    return os_task_create_internal((os_task_func_t)p_func, p_name, stack_depth, (void *)p_param, priority, ph_task);
}

ATTR_NORETURN
ATTR_NONNULL(1)
static void
os_task_thread_func_wrapper_without_param(void *p_arg)
{
    os_task_func_without_param_t p_func = p_arg;
    p_func();
    assert(0);
}

bool
os_task_create_without_param(
    os_task_func_without_param_t p_func,
    const char *                 p_name,
    const uint32_t               stack_depth,
    const os_task_priority_t     priority,
    os_task_handle_t *           ph_task)
{
    return os_task_create_internal(
        &os_task_thread_func_wrapper_without_param,
        p_name,
        stack_depth,
        (void *)p_func,
        priority,
        ph_task);
}

ATTR_NORETURN
ATTR_NONNULL(1)
static void
os_task_thread_func_wrapper_finite_with_param(void *p_arg)
{
    const os_task_arg_finite_with_param_t *p_param = p_arg;
    p_param->p_func(p_param->p_arg);
    app_free(p_arg);
    vTaskDelete(NULL);
    assert(0);
}

bool
os_task_create_finite(
    os_task_finite_func_with_param_t p_func,
    const char *                     p_name,
    const uint32_t                   stack_depth,
    void *                           p_param,
    const os_task_priority_t         priority)
{
    os_task_arg_finite_with_param_t *p_arg = app_calloc(1, sizeof(*p_arg));
    if (NULL == p_arg)
    {
        return false;
    }
    p_arg->p_func = p_func;
    p_arg->p_arg  = p_param;

    os_task_handle_t h_task = NULL;
    return os_task_create_internal(
        &os_task_thread_func_wrapper_finite_with_param,
        p_name,
        stack_depth,
        (void *)p_arg,
        priority,
        &h_task);
}

ATTR_NORETURN
ATTR_NONNULL(1)
static void
os_task_thread_func_wrapper_finite_with_const_param(void *p_arg)
{
    const os_task_arg_finite_with_const_param_t *p_param = p_arg;
    p_param->p_func(p_param->p_arg);
    app_free(p_arg);
    vTaskDelete(NULL);
    assert(0);
}

bool
os_task_create_finite_with_const_param(
    os_task_finite_func_with_const_param_t p_func,
    const char *                           p_name,
    const uint32_t                         stack_depth,
    const void *                           p_param,
    const os_task_priority_t               priority)
{
    os_task_arg_finite_with_const_param_t *p_arg = app_calloc(1, sizeof(*p_arg));
    if (NULL == p_arg)
    {
        return false;
    }
    p_arg->p_func = p_func;
    p_arg->p_arg  = p_param;

    os_task_handle_t h_task = NULL;
    return os_task_create_internal(
        &os_task_thread_func_wrapper_finite_with_const_param,
        p_name,
        stack_depth,
        (void *)p_arg,
        priority,
        &h_task);
}

ATTR_NORETURN
ATTR_NONNULL(1)
static void
os_task_thread_func_wrapper_finite_without_param(void *p_arg)
{
    os_task_finite_func_without_param_t p_func = p_arg;
    p_func();
    vTaskDelete(NULL);
    assert(0);
}

bool
os_task_create_finite_without_param(
    os_task_finite_func_without_param_t p_func,
    const char *                        p_name,
    const uint32_t                      stack_depth,
    const os_task_priority_t            priority)
{
    os_task_handle_t h_task = NULL;
    return os_task_create_internal(
        &os_task_thread_func_wrapper_finite_without_param,
        p_name,
        stack_depth,
        (void *)p_func,
        priority,
        &h_task);
}

static bool
os_task_create_static_internal(
    os_task_func_t           p_func,
    const char *             p_name,
    os_task_stack_type_t *   p_stack_mem,
    const uint32_t           stack_depth,
    void *                   p_param,
    const os_task_priority_t priority,
    os_task_static_t *       p_task_mem,
    os_task_handle_t *       ph_task)
{
    LOG_INFO("Start thread(static) '%s' with priority %d, stack size %u bytes", p_name, priority, stack_depth);
    *ph_task = xTaskCreateStatic(p_func, p_name, stack_depth, p_param, priority, p_stack_mem, p_task_mem);
    if (NULL == *ph_task)
    {
        LOG_ERR("Failed to start thread '%s'", p_name);
        return false;
    }
    return true;
}

bool
os_task_create_static(
    os_task_func_t           p_func,
    const char *             p_name,
    os_task_stack_type_t *   p_stack_mem,
    const uint32_t           stack_depth,
    void *                   p_param,
    const os_task_priority_t priority,
    os_task_static_t *       p_task_mem,
    os_task_handle_t *       ph_task)
{
    return os_task_create_static_internal(
        p_func,
        p_name,
        p_stack_mem,
        stack_depth,
        p_param,
        priority,
        p_task_mem,
        ph_task);
}

bool
os_task_create_static_with_const_param(
    os_task_func_const_param_t p_func,
    const char *               p_name,
    os_task_stack_type_t *     p_stack_mem,
    const uint32_t             stack_depth,
    const void *               p_param,
    const os_task_priority_t   priority,
    os_task_static_t *         p_task_mem,
    os_task_handle_t *         ph_task)
{
    return os_task_create_static_internal(
        (os_task_func_t)p_func,
        p_name,
        p_stack_mem,
        stack_depth,
        (void *)p_param,
        priority,
        p_task_mem,
        ph_task);
}

bool
os_task_create_static_without_param(
    os_task_func_without_param_t p_func,
    const char *                 p_name,
    os_task_stack_type_t *       p_stack_mem,
    const uint32_t               stack_depth,
    const os_task_priority_t     priority,
    os_task_static_t *           p_task_mem,
    os_task_handle_t *           ph_task)
{
    return os_task_create_static_internal(
        &os_task_thread_func_wrapper_without_param,
        p_name,
        p_stack_mem,
        stack_depth,
        (void *)p_func,
        priority,
        p_task_mem,
        ph_task);
}

bool
os_task_create_static_finite(
    os_task_finite_func_with_param_t p_func,
    const char *                     p_name,
    os_task_stack_type_t *           p_stack_mem,
    const uint32_t                   stack_depth,
    void *                           p_param,
    const os_task_priority_t         priority,
    os_task_static_t *               p_task_mem)
{
    os_task_arg_finite_with_param_t *p_arg = app_calloc(1, sizeof(*p_arg));
    if (NULL == p_arg)
    {
        return false;
    }
    p_arg->p_func = p_func;
    p_arg->p_arg  = p_param;

    os_task_handle_t h_task = NULL;
    return os_task_create_static_internal(
        &os_task_thread_func_wrapper_finite_with_param,
        p_name,
        p_stack_mem,
        stack_depth,
        (void *)p_arg,
        priority,
        p_task_mem,
        &h_task);
}

bool
os_task_create_static_finite_with_const_param(
    os_task_finite_func_with_const_param_t p_func,
    const char *                           p_name,
    os_task_stack_type_t *                 p_stack_mem,
    const uint32_t                         stack_depth,
    const void *                           p_param,
    const os_task_priority_t               priority,
    os_task_static_t *                     p_task_mem)
{
    os_task_arg_finite_with_const_param_t *p_arg = app_calloc(1, sizeof(*p_arg));
    if (NULL == p_arg)
    {
        return false;
    }
    p_arg->p_func = p_func;
    p_arg->p_arg  = p_param;

    os_task_handle_t h_task = NULL;
    return os_task_create_static_internal(
        &os_task_thread_func_wrapper_finite_with_const_param,
        p_name,
        p_stack_mem,
        stack_depth,
        (void *)p_arg,
        priority,
        p_task_mem,
        &h_task);
}

bool
os_task_create_static_finite_without_param(
    os_task_finite_func_without_param_t p_func,
    const char *                        p_name,
    os_task_stack_type_t *              p_stack_mem,
    const uint32_t                      stack_depth,
    const os_task_priority_t            priority,
    os_task_static_t *                  p_task_mem)
{
    os_task_handle_t h_task = NULL;
    return os_task_create_static_internal(
        &os_task_thread_func_wrapper_finite_without_param,
        p_name,
        p_stack_mem,
        stack_depth,
        (void *)p_func,
        priority,
        p_task_mem,
        &h_task);
}

void
os_task_delete(os_task_handle_t *ph_task)
{
    vTaskDelete(*ph_task);
    *ph_task = NULL;
}

const char *
os_task_get_name(void)
{
    const char *task_name = pcTaskGetTaskName(NULL);
    if (NULL == task_name)
    {
        task_name = "???";
    }
    return task_name;
}

os_task_handle_t
os_task_get_cur_task_handle(void)
{
    return xTaskGetCurrentTaskHandle();
}
