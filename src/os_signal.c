/**
 * @file os_signal.c
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_signal.h"
#include "os_wrapper_types.h"
#include "os_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "os_malloc.h"

struct os_signal_t
{
    os_task_handle_t task_handle;
    uint32_t         sig_mask;
    bool             is_static;
};

_Static_assert(sizeof(os_signal_t) == sizeof(os_signal_static_t), "os_signal_t != os_signal_static_t");

ATTR_NONNULL(1)
static os_signal_t*
os_signal_init(os_signal_t* const p_signal, const bool is_static)
{
    p_signal->task_handle = NULL;
    p_signal->sig_mask    = 0x0U;
    p_signal->is_static   = is_static;
    return p_signal;
}

os_signal_t*
os_signal_create(void)
{
    os_signal_t* const p_signal = os_calloc(1, sizeof(*p_signal));
    if (NULL == p_signal)
    {
        return NULL;
    }
    const bool is_static = false;
    return os_signal_init(p_signal, is_static);
}

ATTR_RETURNS_NONNULL
ATTR_NONNULL(1)
os_signal_t*
os_signal_create_static(os_signal_static_t* const p_signal_mem)
{
    os_signal_t* const p_signal  = (os_signal_t*)p_signal_mem;
    const bool         is_static = true;
    return os_signal_init(p_signal, is_static);
}

ATTR_NONNULL(1)
void
os_signal_delete(os_signal_t** const pp_signal)
{
    os_signal_t* p_signal = *pp_signal;
    p_signal->task_handle = NULL;
    p_signal->sig_mask    = 0x0U;
    *pp_signal            = NULL;
    if (!p_signal->is_static)
    {
        os_free(p_signal);
    }
}

bool
os_signal_register_cur_thread(os_signal_t* const p_signal)
{
    if (NULL == p_signal)
    {
        return false;
    }
    if (NULL != p_signal->task_handle)
    {
        return false;
    }
    p_signal->task_handle = os_task_get_cur_task_handle();
    return true;
}

void
os_signal_unregister_cur_thread(os_signal_t* const p_signal)
{
    if (NULL == p_signal)
    {
        return;
    }
    p_signal->task_handle = NULL;
}

bool
os_signal_is_any_thread_registered(os_signal_t* const p_signal)
{
    if (NULL == p_signal)
    {
        return false;
    }
    if (NULL != p_signal->task_handle)
    {
        return true;
    }
    return false;
}

bool
os_signal_is_current_thread_registered(os_signal_t* const p_signal)
{
    if (NULL == p_signal)
    {
        return false;
    }
    os_task_handle_t cur_task_handle = os_task_get_cur_task_handle();
    if (cur_task_handle == p_signal->task_handle)
    {
        return true;
    }
    return false;
}

ATTR_CONST
static bool
os_signal_is_valid_sig_num(const os_signal_num_e sig_num)
{
    if (sig_num < OS_SIGNAL_NUM_0)
    {
        return false;
    }
    if (sig_num > OS_SIGNAL_NUM_30)
    {
        return false;
    }
    return true;
}

bool
os_signal_add(os_signal_t* const p_signal, const os_signal_num_e sig_num)
{
    if (NULL == p_signal)
    {
        return false;
    }
    if (!os_signal_is_valid_sig_num(sig_num))
    {
        return false;
    }
    const os_signal_sig_idx_t sig_idx = sig_num - OS_SIGNAL_NUM_0;
    if ((0 != (p_signal->sig_mask & (1U << sig_idx))))
    {
        return false;
    }
    p_signal->sig_mask |= 1U << sig_idx;
    return true;
}

bool
os_signal_send(os_signal_t* const p_signal, const os_signal_num_e sig_num)
{
    if (NULL == p_signal)
    {
        return false;
    }
    if (!os_signal_is_valid_sig_num(sig_num))
    {
        return false;
    }
    const os_signal_sig_idx_t sig_idx  = sig_num - OS_SIGNAL_NUM_0;
    const uint32_t            sig_mask = (uint32_t)(1U << sig_idx);
    if (NULL == p_signal->task_handle)
    {
        return false;
    }

    BaseType_t flag_higher_priority_task_woken = pdFALSE;

    if (xPortInIsrContext())
    {
        if (pdPASS != xTaskNotifyFromISR(p_signal->task_handle, sig_mask, eSetBits, &flag_higher_priority_task_woken))
        {
            return false;
        }
        if (flag_higher_priority_task_woken)
        {
            portYIELD_FROM_ISR();
        }
    }
    else
    {
        if (pdPASS != xTaskNotify(p_signal->task_handle, sig_mask, eSetBits))
        {
            return false;
        }
    }
    return true;
}

ATTR_NONNULL(4)
bool
os_signal_wait_with_sig_mask(
    os_signal_t* const         p_signal,
    const os_signal_sig_mask_t expected_sig_mask,
    const os_delta_ticks_t     timeout_ticks,
    os_signal_events_t* const  p_sig_events)
{
    if (NULL == p_signal)
    {
        return false;
    }
    if (NULL == p_signal->task_handle)
    {
        return false;
    }
    const os_signal_sig_mask_t sig_mask_to_wait = p_signal->sig_mask & expected_sig_mask;

    os_signal_sig_mask_t sig_mask = 0x0U;
    if (pdTRUE != xTaskNotifyWait(0x0U, sig_mask_to_wait, &sig_mask, timeout_ticks))
    {
        return false;
    }
    if (0 == sig_mask)
    {
        return false;
    }

    p_sig_events->sig_mask = sig_mask & sig_mask_to_wait;
    p_sig_events->last_ofs = 0;
    return true;
}

ATTR_NONNULL(3)
bool
os_signal_wait_with_timeout(
    os_signal_t* const        p_signal,
    const os_delta_ticks_t    timeout_ticks,
    os_signal_events_t* const p_sig_events)
{
    return os_signal_wait_with_sig_mask(p_signal, p_signal->sig_mask, timeout_ticks, p_sig_events);
}

ATTR_NONNULL(2)
void
os_signal_wait(os_signal_t* const p_signal, os_signal_events_t* const p_sig_events)
{
    os_signal_wait_with_timeout(p_signal, OS_DELTA_TICKS_INFINITE, p_sig_events);
}

ATTR_NONNULL(1)
os_signal_num_e
os_signal_num_get_next(os_signal_events_t* const p_sig_events)
{
    if (0 == p_sig_events->sig_mask)
    {
        return OS_SIGNAL_NUM_NONE;
    }
    const uint32_t max_bit_idx = OS_SIGNAL_NUM_30 - OS_SIGNAL_NUM_0;
    for (uint32_t i = p_sig_events->last_ofs; i <= max_bit_idx; ++i)
    {
        const uint32_t mask = 1U << i;
        if (p_sig_events->sig_mask & mask)
        {
            p_sig_events->sig_mask ^= mask;
            p_sig_events->last_ofs = i;
            return (os_signal_num_e)(OS_SIGNAL_NUM_0 + i);
        }
    }
    return OS_SIGNAL_NUM_NONE;
}
