/**
 * @file os_time.h
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef OS_TIME_H
#define OS_TIME_H

#include <time.h>
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum os_time_month_e
{
    OS_TIME_MONTH_JAN = 0,
    OS_TIME_MONTH_FEB = 1,
    OS_TIME_MONTH_MAR = 2,
    OS_TIME_MONTH_APR = 3,
    OS_TIME_MONTH_MAY = 4,
    OS_TIME_MONTH_JUN = 5,
    OS_TIME_MONTH_JUL = 6,
    OS_TIME_MONTH_AUG = 7,
    OS_TIME_MONTH_SEP = 8,
    OS_TIME_MONTH_OCT = 9,
    OS_TIME_MONTH_NOV = 10,
    OS_TIME_MONTH_DEC = 11,
} os_time_month_e;

typedef enum os_time_wday_e
{
    OS_TIME_WDAY_SUN = 0,
    OS_TIME_WDAY_MON = 1,
    OS_TIME_WDAY_TUE = 2,
    OS_TIME_WDAY_WED = 3,
    OS_TIME_WDAY_THU = 4,
    OS_TIME_WDAY_FRI = 5,
    OS_TIME_WDAY_SAT = 6,
} os_time_wday_e;

/**
 * This is a wrap for 'time' function.
 * @return Current UNIX time in seconds since 1970.
 */
time_t
os_time_get(void);

/**
 * @brief Convert tm_mon from 'struct tm' to os_time_month_e.
 * @param tm_mon - month number in the range from 0 to 11.
 * @return os_time_month_e
 */
static inline os_time_month_e
os_time_tm_mon_to_month(const int tm_mon)
{
    return (os_time_month_e)tm_mon;
}

/**
 * @brief Get tm_mon from 'struct tm' and convert it to os_time_month_e.
 * @param p_tm - pointer to 'struct tm'
 * @return os_time_month_e
 */
static inline os_time_month_e
os_time_get_tm_mon(const struct tm* const p_tm)
{
    return os_time_tm_mon_to_month(p_tm->tm_mon);
}

/**
 * @brief Convert tm_wday from 'struct tm' to os_time_wday_e.
 * @param tm_wday - day of the week in the range from 0 to 6, starting on Sunday.
 * @return os_time_wday_e
 */
static inline os_time_wday_e
os_time_tm_wday_to_wday(const int tm_wday)
{
    return (os_time_wday_e)tm_wday;
}

/**
 * @brief Get tm_wday from 'struct tm' and convert it to os_time_wday_e.
 * @param p_tm - pointer to 'struct tm'
 * @return os_time_wday_e
 */
static inline os_time_wday_e
os_time_get_tm_wday(const struct tm* const p_tm)
{
    return os_time_tm_wday_to_wday(p_tm->tm_wday);
}

/**
 * @brief Get a three-character abbreviation for the month.
 * @param month - month.
 * @return a string with a three-character abbreviation of the month.
 */
const char*
os_time_month_name_short(const os_time_month_e month);

/**
 * @brief Get the name of the month.
 * @param month - month.
 * @return a string with a full name of the month.
 */
const char*
os_time_month_name_long(const os_time_month_e month);

/**
 * @brief Get a two-character abbreviation for the day of the week.
 * @param day_of_the_week - day of the week.
 * @return a string with a two-character abbreviation of the day of the week.
 */
const char*
os_time_wday_name_short(const os_time_wday_e day_of_the_week);

/**
 * @brief Get a three-character abbreviation for the day of the week.
 * @param day_of_the_week - day of the week.
 * @return a string with a three-character abbreviation of the day of the week.
 */
const char*
os_time_wday_name_mid(const os_time_wday_e day_of_the_week);

/**
 * @brief Get the name of the day of the week.
 * @param day_of_the_week - day of the week.
 * @return a string with a full name of the day of the week.
 */
const char*
os_time_wday_name_long(const os_time_wday_e day_of_the_week);

#ifdef __cplusplus
}
#endif

#endif // OS_TIME_H
