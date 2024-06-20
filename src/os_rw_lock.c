/**
 * @file os_rw_lock.c
 * @author TheSomeMan
 * @date 2023-07-06
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_rw_lock.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define OS_RW_LOCK_NUM_RETRIES_TRY_ACQUIRE_READER (10)

void
os_rw_lock_init(os_rw_lock_t* const p_lock)
{
    p_lock->mutex_reader = os_mutex_create_static(&p_lock->mutex_reader_mem);
    p_lock->sema_writer  = os_sema_create_static(&p_lock->sema_writer_mem);
    os_sema_signal(p_lock->sema_writer);
    p_lock->readers_cnt = 0;
}

void
os_rw_lock_deinit(os_rw_lock_t* const p_lock)
{
    os_mutex_delete(&p_lock->mutex_reader);
    os_sema_delete(&p_lock->sema_writer);
}

void
os_rw_lock_acquire_reader(os_rw_lock_t* const p_lock)
{
    os_mutex_lock(p_lock->mutex_reader);
    p_lock->readers_cnt += 1;
    if (1 == p_lock->readers_cnt)
    {
        os_sema_wait_infinite(p_lock->sema_writer);
    }
    os_mutex_unlock(p_lock->mutex_reader);
}

void
os_rw_lock_release_reader(os_rw_lock_t* const p_lock)
{
    os_mutex_lock(p_lock->mutex_reader);
    p_lock->readers_cnt -= 1;
    if (0 == p_lock->readers_cnt)
    {
        os_sema_signal(p_lock->sema_writer);
    }
    os_mutex_unlock(p_lock->mutex_reader);
}

void
os_rw_lock_acquire_writer(os_rw_lock_t* const p_lock)
{
    os_sema_wait_infinite(p_lock->sema_writer);
}

void
os_rw_lock_release_writer(os_rw_lock_t* const p_lock)
{
    os_sema_signal(p_lock->sema_writer);
}

bool
os_rw_lock_try_acquire_reader(os_rw_lock_t* const p_lock)
{
    if (os_mutex_try_lock(p_lock->mutex_reader))
    {
        if (0 == p_lock->readers_cnt)
        {
            if (!os_sema_wait_immediate(p_lock->sema_writer))
            {
                os_mutex_unlock(p_lock->mutex_reader);
                return false;
            }
        }
        p_lock->readers_cnt += 1;
        os_mutex_unlock(p_lock->mutex_reader);
        return true;
    }
    for (int32_t i = 0; i < OS_RW_LOCK_NUM_RETRIES_TRY_ACQUIRE_READER; ++i)
    {
        const os_delta_ticks_t ticks_to_wait = 1;
        if (os_mutex_lock_with_timeout(p_lock->mutex_reader, ticks_to_wait))
        {
            if (0 == p_lock->readers_cnt)
            {
                if (!os_sema_wait_immediate(p_lock->sema_writer))
                {
                    os_mutex_unlock(p_lock->mutex_reader);
                    return false;
                }
            }
            p_lock->readers_cnt += 1;
            os_mutex_unlock(p_lock->mutex_reader);
            return true;
        }
        // Check if sema_writer is taken by writer, without trying to acquire mutex_reader.
        if (!os_sema_wait_immediate(p_lock->sema_writer))
        {
            // If sema_writer is taken by writer, then exit from this retry loop
            break;
        }
        os_sema_signal(p_lock->sema_writer);
    }
    return false;
}

bool
os_rw_lock_try_acquire_writer(os_rw_lock_t* const p_lock)
{
    return os_sema_wait_immediate(p_lock->sema_writer);
}
