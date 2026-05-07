/**
 * @file os_malloc.c
 * @author TheSomeMan
 * @date 2020-10-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_malloc.h"
#include <stdlib.h>

#if OS_MALLOC_TRACE
#include <string.h>
#include "sys/queue.h"
#include "os_mutex.h"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO
#include "log.h"
static const char* TAG = "MEM_TRACE";

typedef struct os_malloc_trace_info_t
{
    void*  p_mem;
    size_t size;
    TAILQ_ENTRY(os_malloc_trace_info_t) list;
    const char* p_file;
    int32_t     line;
#if !OS_MALLOC_TRACE_DISABLE_TIMESTAMP
    uint32_t timestamp;
#endif
} os_malloc_trace_info_t;

typedef TAILQ_HEAD(os_malloc_trace_list_t, os_malloc_trace_info_t) os_malloc_trace_list_t;

static os_malloc_trace_list_t g_os_malloc_trace_list;
static os_mutex_t             g_p_os_malloc_trace_mutex;
static os_mutex_static_t      g_os_malloc_trace_mutex_mem;
static int32_t                g_os_malloc_trace_cnt;

static os_malloc_trace_list_t*
os_malloc_trace_mutex_lock(void)
{
    if (NULL == g_p_os_malloc_trace_mutex)
    {
        g_p_os_malloc_trace_mutex = os_mutex_create_static(&g_os_malloc_trace_mutex_mem);
        os_mutex_lock(g_p_os_malloc_trace_mutex);
        TAILQ_INIT(&g_os_malloc_trace_list);
        g_os_malloc_trace_cnt = 0;
    }
    else
    {
        os_mutex_lock(g_p_os_malloc_trace_mutex);
    }
    return &g_os_malloc_trace_list;
}

static void
os_malloc_trace_mutex_unlock(os_malloc_trace_list_t** p_p_list)
{
    os_mutex_unlock(g_p_os_malloc_trace_mutex);
    *p_p_list = NULL;
}
#endif

#if OS_MALLOC_TRACE
ATTR_MALLOC
ATTR_MALLOC_SIZE(1)
void*
os_malloc_internal(const size_t size, const char* const p_file, const int32_t line)
{
    return os_calloc_internal(1, size, p_file, line);
}
#else
ATTR_MALLOC
ATTR_MALLOC_SIZE(1)
void*
os_malloc(const size_t size)
{
    return malloc(size);
}
#endif

#if OS_MALLOC_TRACE
void
os_free_internal(void* ptr, const char* const p_file, const int32_t line)
{
    if (NULL == ptr)
    {
        return;
    }

    os_malloc_trace_info_t* const p_info = (os_malloc_trace_info_t*)((uint8_t*)ptr - sizeof(os_malloc_trace_info_t));
    const size_t                  size   = p_info->size;
    os_malloc_trace_list_t*       p_list = os_malloc_trace_mutex_lock();
    if (NULL != p_info->list.tqe_prev)
    {
        TAILQ_REMOVE(p_list, p_info, list);
        g_os_malloc_trace_cnt -= 1;
    }
    os_malloc_trace_mutex_unlock(&p_list);
    memset(p_info, 0, sizeof(*p_info) + size);

    free(p_info);
}
#else
void
os_free_internal(void* ptr)
{
    if (NULL != ptr)
    {
        free(ptr);
    }
}
#endif

#if OS_MALLOC_TRACE
ATTR_MALLOC
ATTR_CALLOC_SIZE(1, 2)
void*
os_calloc_internal(const size_t nmemb, const size_t size, const char* const p_file, const int32_t line)
{
    void* const p_mem = calloc(1, sizeof(os_malloc_trace_info_t) + nmemb * size);
    if (NULL == p_mem)
    {
        return NULL;
    }

    os_malloc_trace_info_t* const p_info = p_mem;
    p_info->p_mem                        = p_mem;
    p_info->size                         = nmemb * size;
    p_info->p_file                       = p_file;
    p_info->line                         = line;
#if !OS_MALLOC_TRACE_DISABLE_TIMESTAMP
    p_info->timestamp = xTaskGetTickCount();
#endif

    os_malloc_trace_list_t* p_list = os_malloc_trace_mutex_lock();
    TAILQ_INSERT_TAIL(p_list, p_info, list);
    g_os_malloc_trace_cnt += 1;
    os_malloc_trace_mutex_unlock(&p_list);
    return (void*)((uint8_t*)p_mem + sizeof(os_malloc_trace_info_t));
}
#else
ATTR_MALLOC
ATTR_CALLOC_SIZE(1, 2)
void*
os_calloc(const size_t nmemb, const size_t size)
{
    return calloc(nmemb, size);
}
#endif

#if OS_MALLOC_TRACE
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
bool
os_realloc_safe_internal(void** const p_ptr, const size_t size, const char* const p_file, const int32_t line)
{
    void* ptr = *p_ptr;
    if (NULL == ptr)
    {
        return false;
    }

    const os_malloc_trace_info_t* const p_info_old
        = (os_malloc_trace_info_t*)((uint8_t*)ptr - sizeof(os_malloc_trace_info_t));

    void* p_new_ptr = os_malloc_internal(size, p_file, line);
    if (NULL == p_new_ptr)
    {
        *p_ptr = NULL;
        return false;
    }
    memcpy(p_new_ptr, ptr, ((size < p_info_old->size) ? size : p_info_old->size));
    os_free_internal(ptr, p_file, line);
    *p_ptr = p_new_ptr;
    return true;
}
#else
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
bool
os_realloc_safe(void** const p_ptr, const size_t size)
{
    void* ptr       = *p_ptr;
    void* p_new_ptr = realloc(ptr, size);
    if (NULL == p_new_ptr)
    {
        return false;
    }
    *p_ptr = p_new_ptr;
    return true;
}
#endif

#if OS_MALLOC_TRACE
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
bool
os_realloc_safe_and_clean_internal(void** const p_ptr, const size_t size, const char* const p_file, const int32_t line)
{
    void*      p_old_ptr = *p_ptr;
    const bool res       = os_realloc_safe_internal(p_ptr, size, p_file, line);
    if (!res)
    {
        os_free_internal(p_old_ptr, p_file, line);
    }
    return res;
}
#else
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
bool
os_realloc_safe_and_clean(void** const p_ptr, const size_t size)
{
    void* ptr       = *p_ptr;
    void* p_new_ptr = realloc(ptr, size);
    if (NULL == p_new_ptr)
    {
        os_free(*p_ptr);
        return false;
    }
    *p_ptr = p_new_ptr;
    return true;
}
#endif

#if OS_MALLOC_TRACE
void
os_malloc_trace_dump(void)
{
    os_malloc_trace_list_t* p_list = os_malloc_trace_mutex_lock();
    uint32_t                cnt    = 0;
    LOG_INFO("Num blocks allocated: %d", g_os_malloc_trace_cnt);
    os_malloc_trace_info_t* p_info;
    TAILQ_FOREACH(p_info, p_list, list)
    {
#if !OS_MALLOC_TRACE_DISABLE_TIMESTAMP
        LOG_INFO(
            "[%4u] %p: %u bytes (at %u), %s:%d",
            cnt,
            p_info->p_mem,
            p_info->size,
            p_info->timestamp,
            p_info->p_file,
            p_info->line);
#else
        LOG_INFO(
            "[%4u] %p: %zu bytes, %s:%d",
            (printf_uint_t)cnt,
            p_info->p_mem,
            p_info->size,
            p_info->p_file,
            p_info->line);
#endif
        cnt += 1;
    }
    os_malloc_trace_mutex_unlock(&p_list);
}
#endif

#if OS_MALLOC_TRACE
void
os_malloc_trace_clear(void)
{
    os_malloc_trace_list_t* p_list = os_malloc_trace_mutex_lock();
    os_malloc_trace_info_t* p_info;
    TAILQ_FOREACH(p_info, p_list, list)
    {
        p_info->list.tqe_prev = NULL;
    }
    TAILQ_INIT(p_list);
    g_os_malloc_trace_cnt = 0;
    os_malloc_trace_mutex_unlock(&p_list);
}
#endif
