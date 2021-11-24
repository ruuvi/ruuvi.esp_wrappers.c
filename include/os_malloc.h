/**
 * @file os_malloc.h
 * @author TheSomeMan
 * @date 2020-10-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_MALLOC_H
#define OS_MALLOC_H

#include <stdbool.h>
#include <stddef.h>
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is a wrap for malloc - it allocates a block of memory of size 'size' bytes.
 * @param size  - the size the memory block
 * @return pointer to the allocated memory block or NULL.
 */
ATTR_MALLOC
ATTR_MALLOC_SIZE(1)
void *
os_malloc(const size_t size);

/**
 * This is a wrap for calloc - it allocates memory block for an array of 'nmemb' elements,
 * each 'size' bytes and fills the array with zeroes.
 * @param nmemb - the number of elements in the array.
 * @param size  - the size of one array element.
 * @return pointer to the allocated memory block or NULL.
 */
ATTR_MALLOC
ATTR_CALLOC_SIZE(1, 2)
void *
os_calloc(const size_t nmemb, const size_t size);

/**
 * @brief This is a safer wrap for realloc,
 *        it changes the size of the memory block and checks the result of the reallocation.
 * @note This function checks if realloc returns NULL and overwrites value in p_ptr if a new memory block was allocated.
 * @param[IN,OUT] p_ptr - ptr to a variable which points to the memory block,
 * the pointer to the new memory block will be saved to p_ptr.
 * In case if the reallocation failed, then the value in p_ptr will not be changed.
 * @param size - new size of the memory block.
 * @return true if the reallocation was successful.
 */
ATTR_WARN_UNUSED_RESULT
bool
os_realloc_safe(void **const p_ptr, const size_t size);

/**
 * @brief This is a safer wrap for realloc,
 *        it changes the size of the memory block and checks the result of the reallocation,
 *        it automatically frees the original memory pointer in case if realloc fails.
 * @note This function checks if realloc returns NULL and overwrites value in p_ptr if a new memory block was allocated.
 * @param[IN,OUT] p_ptr - ptr to a variable which points to the memory block,
 * the pointer to the new memory block will be saved to p_ptr.
 * In case if the reallocation failed, then the value in p_ptr will not be changed.
 * @param size - new size of the memory block.
 * @return true if the reallocation was successful.
 */
ATTR_WARN_UNUSED_RESULT
ATTR_NONNULL(1)
bool
os_realloc_safe_and_clean(void **const p_ptr, const size_t size);

/**
 * This is a wrap for 'free' - it deallocates a block of memory
 * @note This function should not be used, use macro @ref os_free instead.
 * @param ptr  - pointer to the memory block
 */
void
os_free_internal(void *ptr);

/**
 * @brief os_free - is a wrap for 'free' which automatically sets pointer to NULL after the memory freeing.
 */
#define os_free(ptr) \
    do \
    { \
        if (NULL != (ptr)) \
        { \
            os_free_internal((void *)(ptr)); \
            ptr = NULL; \
        } \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // OS_MALLOC_H
