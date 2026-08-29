/**
 * @file os_rw_lock.h
 * @brief Provides reader-writer locking mechanism for synchronizing access to resources.
 * This module allows multiple threads to read a resource simultaneously or
 * allows one thread to have exclusive write access.
 * @author TheSomeMan
 * @date 2023-07-06
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_RW_LOCK_H
#define OS_RW_LOCK_H

#include <stdbool.h>
#include "os_mutex.h"
#include "os_sema.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct os_rw_lock_t
{
    os_mutex_t        mutex_reader;     /*!< Mutex for synchronizing access to readers_cnt */
    os_mutex_static_t mutex_reader_mem; /*!< Static memory allocation for the reader mutex */
    os_sema_t         sema_writer;      /*!< Semaphore for synchronizing write access */
    os_sema_static_t  sema_writer_mem;  /*!< Semaphore memory allocation for the writer mutex */
    int32_t           readers_cnt;      /*!< Count of active readers */
} os_rw_lock_t;

/**
 * @brief Initialize an os_rw_lock_t object with pre-allocated memory.
 * @param p_lock Pointer to an os_rw_lock_t object.
 */
ATTR_NONNULL(1)
void
os_rw_lock_init(os_rw_lock_t* const p_lock);

/**
 * @brief Deinitialize an os_rw_lock_t object.
 * @param p_lock Pointer to an os_rw_lock_t object.
 */
void
os_rw_lock_deinit(os_rw_lock_t* const p_lock);

/**
 * @brief Attempt to acquire a reader lock.
 * @details If a writer holds the lock, the function will block until the writer releases the lock.
 * @param p_lock Pointer to an os_rw_lock_t object.
 */
void
os_rw_lock_acquire_reader(os_rw_lock_t* const p_lock);

/**
 * @brief Release a previously acquired reader lock.
 * @param p_lock Pointer to an os_rw_lock_t object.
 */
void
os_rw_lock_release_reader(os_rw_lock_t* const p_lock);

/**
 * @brief Attempt to acquire a writer lock.
 * @details If any reader or writer holds the lock, the function will block until the lock is available.
 * @param p_lock Pointer to an os_rw_lock_t object.
 */
void
os_rw_lock_acquire_writer(os_rw_lock_t* const p_lock);

/**
 * @brief Release a previously acquired writer lock.
 * @param p_lock Pointer to an os_rw_lock_t object.
 */
void
os_rw_lock_release_writer(os_rw_lock_t* const p_lock);

/**
 * @brief Attempts to acquire a reader lock without indefinite blocking.
 *
 * @details In a reader-writer lock scenario, a reader lock may be held by other readers for a potentially
 * long duration. For instance, consider three threads A (low priority), B (medium priority), and C (high priority):
 * 1. Thread A invokes os_rw_lock_try_acquire_reader and locks mutex_reader.
 * 2. Before sema_writer is locked, thread B is activated and consumes significant CPU time.
 * 3. In the meantime, if thread C is activated and attempts to invoke os_rw_lock_try_acquire_reader,
 * it fails due to the locked mutex_reader.
 *
 * To account for such scenarios, this function employs a retry mechanism with a fixed number of attempts
 * coupled with a minimal delay to briefly wait for the lock. On its first attempt, the function tries to
 * acquire mutex_reader without any wait. If unsuccessful, it subsequently tries to acquire the lock with
 * the smallest possible delay. This gives thread A a chance to increase its priority (due to priority
 * inheritance mechanism associated with the mutex), allowing it to complete its locking of sema_writer
 * and release mutex_reader, hence, allowing thread C to acquire the lock.
 *
 * @note However, it's crucial to consider that there remains a minuscule probability of a false failure. This
 * may occur during the simultaneous reader acquisition attempts from different threads in sophisticated
 * scenarios involving the activation/deactivation of higher-priority threads following intricate patterns.
 *
 * @param p_lock Pointer to an os_rw_lock_t object.
 * @return true if the reader lock was successfully acquired; false otherwise.
 */
bool
os_rw_lock_try_acquire_reader(os_rw_lock_t* const p_lock);

/**
 * @brief Attempt to acquire a writer lock without blocking.
 * @param p_lock Pointer to an os_rw_lock_t object.
 * @return true if the writer lock was successfully acquired; false otherwise.
 */
bool
os_rw_lock_try_acquire_writer(os_rw_lock_t* const p_lock);

#ifdef __cplusplus
}
#endif

#endif // OS_RW_LOCK_H
