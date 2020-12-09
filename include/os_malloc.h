/**
 * @file os_malloc.h
 * @author TheSomeMan
 * @date 2020-10-01
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef RUUVI_GATEWAY_ESP_OS_MALLOC_H
#define RUUVI_GATEWAY_ESP_OS_MALLOC_H

#include <stddef.h>
#include "attribs.h"

#ifdef __cplusplus
extern "C" {
#endif

ATTR_MALLOC
void *
app_malloc(const size_t size);

void
app_free(void *ptr);

ATTR_NONNULL(1)
void
app_free_pptr(void **p_ptr);

ATTR_NONNULL(1)
void
app_free_const_pptr(const void **p_ptr);

void *
app_calloc(const size_t nmemb, const size_t size);

#ifdef __cplusplus
}
#endif

#endif // RUUVI_GATEWAY_ESP_OS_MALLOC_H
