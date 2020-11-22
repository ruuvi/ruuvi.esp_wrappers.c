/**
 * @file esp_type_wrapper.h
 * @author TheSomeMan
 * @date 2020-10-20
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef ESP_TYPE_WRAPPER_H
#define ESP_TYPE_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int      printf_int_t;
typedef unsigned printf_uint_t;

typedef long          printf_long_t;
typedef unsigned long printf_ulong_t;

typedef int socket_t;
typedef int socket_recv_result_t;
typedef int socket_send_result_t;

typedef int file_descriptor_t;
typedef int file_read_result_t;

#ifdef __cplusplus
}
#endif

#endif // ESP_TYPE_WRAPPER_H
