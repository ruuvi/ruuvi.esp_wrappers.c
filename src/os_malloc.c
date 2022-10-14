/**
 * @file os_malloc.c
 * @author TheSomeMan
 * @date 2020-10-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_malloc.h"
#include <stdlib.h>

ATTR_MALLOC
ATTR_MALLOC_SIZE(1)
void*
os_malloc(const size_t size)
{
    return malloc(size);
}

void
os_free_internal(void* ptr)
{
    if (NULL != ptr)
    {
        free(ptr);
    }
}

ATTR_MALLOC
ATTR_CALLOC_SIZE(1, 2)
void*
os_calloc(const size_t nmemb, const size_t size)
{
    return calloc(nmemb, size);
}

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
