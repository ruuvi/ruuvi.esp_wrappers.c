/**
 * @file str_buf.h
 * @author TheSomeMan
 * @date 2020-08-23
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef STR_BUF_H
#define STR_BUF_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STR_BUF_INIT(buf_, len_) \
    { \
        .buf = (buf_), .size = (len_), .idx = 0, \
    }

#define STR_BUF_INIT_NULL() \
    STR_BUF_INIT(NULL, 0)

#define STR_BUF_INIT_WITH_ARR(arr_) \
    STR_BUF_INIT((arr_), sizeof(arr_))

typedef size_t str_buf_size_t;

typedef struct str_buf_t
{
    char *         buf;
    str_buf_size_t size;
    str_buf_size_t idx;
} str_buf_t;

/**
 * Init str_buf_t object with valid pointer to buffer and it's size.
 * @return str_but_t object
 */
ATTR_NONNULL(1)
str_buf_t
str_buf_init(char *p_buf, const str_buf_size_t buf_size);

/**
 * Init str_buf_t object with NULL pointer to buffer.
 * @return str_but_t object
 */
str_buf_t
str_buf_init_null(void);

/**
 * Allocate buffer for the accumulated string.
 * @param p_str_buf - pointer to str_buf_t object
 * @return true if the buffer allocated successfully.
 */
ATTR_NONNULL(1)
bool
str_buf_init_with_alloc(str_buf_t *p_str_buf);

/**
 * Get the accumulated length of string.
 * @param p_str_buf - pointer to str_buf_t object
 * @return length of string.
 */
ATTR_NONNULL(1)
str_buf_size_t
str_buf_get_len(const str_buf_t *p_str_buf);

/**
 * Check if buffer overflow occurred during last call to str_buf_vprintf or str_buf_printf.
 * @param p_str_buf - pointer to str_buf_t object
 * @return true if buffer overflow occurred.
 */
ATTR_NONNULL(1)
bool
str_buf_is_overflow(const str_buf_t *p_str_buf);

/**
 * Print string to buffer or calculate size of string if the p_str_buf->buf is NULL.
 * @param p_str_buf - pointer to str_buf_t object
 * @param fmt - format string
 * @param args - arguments for format string
 * @return true if the string was successfully printed to the buffer and there was no buffer overflow.
 */
ATTR_NONNULL(1, 2)
bool
str_buf_vprintf(str_buf_t *p_str_buf, const char *fmt, va_list args);

/**
 * Print string to buffer or calculate size of string if the p_str_buf->buf is NULL.
 * @param p_str_buf - pointer to str_buf_t object
 * @param fmt - format string
 * @param ... - arguments for format string
 * @return true if the string was successfully printed to the buffer and there was no buffer overflow.
 */
ATTR_PRINTF(2, 3)
ATTR_NONNULL(1, 2)
bool
str_buf_printf(str_buf_t *p_str_buf, const char *fmt, ...);

/**
 * Allocate buffer for a new string and print it there.
 * @note The allocated buffer can be deallocated using str_buf_free_buf
 * @param fmt - format string
 * @param args - arguments for format string
 * @return str_buf_t which points to the new allocated buffer
 */
ATTR_NONNULL(1)
str_buf_t
str_buf_vprintf_with_alloc(const char *fmt, va_list args);

/**
 * Allocate buffer for a new string and print it there.
 * @note The allocated buffer can be deallocated using str_buf_free_buf
 * @param fmt - format string
 * @param ... - arguments for format string
 * @return str_buf_t which points to the new allocated buffer
 */
ATTR_PRINTF(1, 2)
ATTR_NONNULL(1)
str_buf_t
str_buf_printf_with_alloc(const char *fmt, ...);

/**
 * Free the buffer to which the str_buf_t object points to.
 * @param p_str_buf - pointer to str_buf_t object
 */
ATTR_NONNULL(1)
void
str_buf_free_buf(str_buf_t *p_str_buf);

#ifdef __cplusplus
}
#endif

#endif // STR_BUF_H
