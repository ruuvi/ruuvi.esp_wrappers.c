/**
 * @file os_signal.h
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_SIGNAL_H
#define OS_SIGNAL_H

#include <stdint.h>
#include <stdbool.h>
#include "os_wrapper_types.h"
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct os_signal_t os_signal_t;

typedef struct os_signal_static_t
{
    void *   stub1;
    uint32_t stub2;
    bool     stub3;
} os_signal_static_t;

typedef enum os_signal_num_e
{
    OS_SIGNAL_NUM_NONE = 0,
    OS_SIGNAL_NUM_0,
    OS_SIGNAL_NUM_1,
    OS_SIGNAL_NUM_2,
    OS_SIGNAL_NUM_3,
    OS_SIGNAL_NUM_4,
    OS_SIGNAL_NUM_5,
    OS_SIGNAL_NUM_6,
    OS_SIGNAL_NUM_7,
    OS_SIGNAL_NUM_8,
    OS_SIGNAL_NUM_9,
    OS_SIGNAL_NUM_10,
    OS_SIGNAL_NUM_11,
    OS_SIGNAL_NUM_12,
    OS_SIGNAL_NUM_13,
    OS_SIGNAL_NUM_14,
    OS_SIGNAL_NUM_15,
    OS_SIGNAL_NUM_16,
    OS_SIGNAL_NUM_17,
    OS_SIGNAL_NUM_18,
    OS_SIGNAL_NUM_19,
    OS_SIGNAL_NUM_20,
    OS_SIGNAL_NUM_21,
    OS_SIGNAL_NUM_22,
    OS_SIGNAL_NUM_23,
    OS_SIGNAL_NUM_24,
    OS_SIGNAL_NUM_25,
    OS_SIGNAL_NUM_26,
    OS_SIGNAL_NUM_27,
    OS_SIGNAL_NUM_28,
    OS_SIGNAL_NUM_29,
    OS_SIGNAL_NUM_30,
} os_signal_num_e;

typedef uint32_t os_signal_sig_idx_t;
typedef uint32_t os_signal_sig_mask_t;

typedef struct os_signal_bit_mask_t
{
    os_signal_sig_mask_t sig_mask;
    os_signal_sig_idx_t  last_ofs;
} os_signal_events_t;

/**
 * @brief Create new os_signal_t object.
 * @return ptr to the instance of os_signal_t object.
 */
os_signal_t *
os_signal_create(void);

/**
 * @brief Create new os_signal_t object using pre-allocated memory.
 * @param p_signal_mem - pointer to the pre-allocated memory.
 * @return ptr to the instance of os_signal_t object.
 */
ATTR_RETURNS_NONNULL
ATTR_NONNULL(1)
os_signal_t *
os_signal_create_static(os_signal_static_t *const p_signal_mem);

/**
 * @brief Create os_signal_t object.
 * @param pp_signal - ptr to ptr to the os_signal_t object.
 * @note the passed ptr to the object will be set to NULL.
 * @return None.
 */
ATTR_NONNULL(1)
void
os_signal_delete(os_signal_t **pp_signal);

/**
 * @brief Register the current task in os_signal_t object.
 * @param true if the current task was registered successfully, false - if some other task was already registered.
 */
bool
os_signal_register_cur_thread(os_signal_t *const p_signal);

void
os_signal_unregister_cur_thread(os_signal_t *const p_signal);

/**
 * @brief Check if any task registered in os_signal_t object.
 * @param true if any task was registered.
 */
bool
os_signal_is_any_thread_registered(os_signal_t *const p_signal);

/**
 * @brief Check if the current task registered in os_signal_t object.
 * @param true if the current task was registered.
 */
bool
os_signal_is_current_thread_registered(os_signal_t *const p_signal);

/**
 * @brief Register a signal number to handle by os_signal_t.
 * @param p_signal   - ptr to os_signal_t instance.
 * @param sig_num    - signal number, @ref os_signal_num_e.
 * @return true if success.
 */
bool
os_signal_add(os_signal_t *const p_signal, const os_signal_num_e sig_num);

/**
 * @brief Send the signal to the registered thread.
 * @param p_signal       - ptr to os_signal_t
 * @param sig_num        - os_signal_num_e
 * @return true if successful
 */
bool
os_signal_send(os_signal_t *const p_signal, const os_signal_num_e sig_num);

/**
 * @brief Wait for the calling task to receive one or more signals.
 * @param p_signal          - ptr to @ref os_signal_t
 * @param expected_sig_mask - bit-mask for expected signals
 * @param timeout_ticks     - timeout in system ticks or OS_TASK_DELAY_IMMEDIATE / OS_TASK_DELAY_INFINITE
 * @param[OUT] p_sig_events - ptr to @ref os_signal_events_t.
 * @return true if success.
 */
ATTR_NONNULL(4)
bool
os_signal_wait_with_sig_mask(
    os_signal_t *const         p_signal,
    const os_signal_sig_mask_t expected_sig_mask,
    const os_delta_ticks_t     timeout_ticks,
    os_signal_events_t *const  p_sig_events);

/**
 * @brief Wait for the calling task to receive one or more signals withing the specified timeout.
 * @param p_signal          - ptr to @ref os_signal_t
 * @param timeout_ticks     - timeout in system ticks or OS_TASK_DELAY_IMMEDIATE / OS_TASK_DELAY_INFINITE
 * @param[OUT] p_sig_events - ptr to @ref os_signal_events_t
 * @return true if success
 */
ATTR_NONNULL(3)
bool
os_signal_wait_with_timeout(
    os_signal_t *const        p_signal,
    const os_delta_ticks_t    timeout_ticks,
    os_signal_events_t *const p_sig_events);

/**
 * @brief Wait infinitely for the calling task to receive one or more signals.
 * @param p_signal          - ptr to @ref os_signal_t
 * @param[OUT] p_sig_events - ptr to @ref os_signal_events_t
 * @return true if success
 */
ATTR_NONNULL(2)
void
os_signal_wait(os_signal_t *const p_signal, os_signal_events_t *const p_sig_events);

ATTR_NONNULL(1)
os_signal_num_e
os_signal_num_get_next(os_signal_events_t *const p_sig_events);

#ifdef __cplusplus
}
#endif

#endif // OS_SIGNAL_H
