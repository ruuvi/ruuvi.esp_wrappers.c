/**
 * @file os_gmtime.h
 * @author TheSomeMan
 * @date 2021-08-02
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef RUUVI_OS_MKGMTIME_H
#define RUUVI_OS_MKGMTIME_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert a broken-down time structure (struct tm), expressed as UTC time,
 *        to time_t - calendar time representation (or Unix-time - seconds since the Epoch 1970).
 * @note The function ignores the values supplied by the caller in the tm_wday, tm_yday and tm_isdst fields.
 * @note This function modifies the fields of the tm structure as follows: tm_wday and tm_yday are set to values
 *       determined from the contents of the other fields;
 *       if structure members are outside their valid interval, they will be normalized
 *       (so that, for example, 40 October is changed into 9 November);
 *       tm_isdst is set to 0.
 * @note If the specified broken-down time cannot be represented as calendar time (seconds since the Epoch),
 *       the function returns (time_t)-1 and does not alter the members of the broken-down time structure.
 * @param[in,out] p_tm_time_utc - pointer to 'struct tm' with a broken-down time, expressed as UTC.
 * @return time_t - calendar time (or unix-time - seconds since the Epoch 1970) or (time_t)-1 if a error was detected.
 */
time_t
os_mkgmtime(struct tm *const p_tm_time_utc);

#ifdef __cplusplus
}
#endif

#endif // RUUVI_OS_MKGMTIME_H
