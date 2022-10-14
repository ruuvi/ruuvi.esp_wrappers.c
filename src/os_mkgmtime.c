/**
 * @file os_mkgmtime.c
 * @author TheSomeMan
 * @date 2021-08-02
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_mkgmtime.h"
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define OS_TIME_STRUCT_TM_BASE_YEAR (1900)
#define OS_TIME_EPOCH_START_YEAR    (1970)
#define OS_TIME_EPOCH_REL_BASE_YEAR (OS_TIME_EPOCH_START_YEAR - OS_TIME_STRUCT_TM_BASE_YEAR)
#define OS_TIME_EPOCH_MAX_YEAR      (2038)
#define OS_TIME_EPOCH_REL_MAX_YEAR  (OS_TIME_EPOCH_MAX_YEAR - OS_TIME_STRUCT_TM_BASE_YEAR)

#define OS_TIME_NUM_LEAP_YEARS_IN_RANGE_1900_1970 (17)

#define OS_TIME_NUM_MONTHS_PER_YEAR    (12)
#define OS_TIME_NUM_DAYS_PER_YEAR      (365)
#define OS_TIME_NUM_HOURS_PER_DAY      (24)
#define OS_TIME_NUM_MINUTES_PER_HOURS  (60)
#define OS_TIME_NUM_SECONDS_PER_MINUTE (60)

static const int16_t g_os_mkgmtime_days_const[13] = { -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364 };

static inline bool
check_is_leap_year(const uint16_t year)
{
    if (0 == (year % 4))
    {
        if (0 == (year % 100U))
        {
            if (0 == (year % 400U))
            {
                return true;
            }
            return false;
        }
        return true;
    }
    return false;
}

static uint16_t
calc_elapsed_years_since_1900(struct tm* const p_tm_time_utc)
{
    if ((p_tm_time_utc->tm_year < (OS_TIME_EPOCH_REL_BASE_YEAR - 1))
        || (p_tm_time_utc->tm_year > (OS_TIME_EPOCH_REL_MAX_YEAR + 1)))
    {
        return (uint16_t)(-1);
    }
    int32_t num_of_elapsed_years_since_1900 = p_tm_time_utc->tm_year;

    // Adjust the month value so that it is between 0 and 11.
    if ((p_tm_time_utc->tm_mon < 0) || (p_tm_time_utc->tm_mon > (OS_TIME_NUM_MONTHS_PER_YEAR - 1)))
    {
        num_of_elapsed_years_since_1900 += p_tm_time_utc->tm_mon / OS_TIME_NUM_MONTHS_PER_YEAR;
        p_tm_time_utc->tm_mon %= OS_TIME_NUM_MONTHS_PER_YEAR;
        if (p_tm_time_utc->tm_mon < 0)
        {
            p_tm_time_utc->tm_mon += OS_TIME_NUM_MONTHS_PER_YEAR;
            num_of_elapsed_years_since_1900 -= 1;
        }
    }
    if ((num_of_elapsed_years_since_1900 < (OS_TIME_EPOCH_REL_BASE_YEAR - 1))
        || (num_of_elapsed_years_since_1900 > (OS_TIME_EPOCH_REL_MAX_YEAR + 1)))
    {
        return (uint16_t)(-1);
    }
    return (uint16_t)num_of_elapsed_years_since_1900;
}

time_t
os_mkgmtime(struct tm* const p_tm_time_utc)
{
    const uint16_t num_of_elapsed_years_since_1900 = calc_elapsed_years_since_1900(p_tm_time_utc);
    if ((uint16_t)(-1) == num_of_elapsed_years_since_1900)
    {
        return (time_t)(-1);
    }

    const int32_t days_elapsed_minus_one_since_year_to_month
        = g_os_mkgmtime_days_const[p_tm_time_utc->tm_mon]
          + ((check_is_leap_year((uint16_t)(OS_TIME_STRUCT_TM_BASE_YEAR + num_of_elapsed_years_since_1900))
              && (p_tm_time_utc->tm_mon > 1))
                 ? 1
                 : 0);

    /*
     * Calculate elapsed days since base date (midnight, 1970-01-01, UTC)
     * 365 days for each elapsed year since 1970, plus one more day for each elapsed leap year.
     */
    const int32_t num_days_elapsed_since_1970_till_cur_month
        = (int32_t)(num_of_elapsed_years_since_1900 - OS_TIME_EPOCH_REL_BASE_YEAR) * OS_TIME_NUM_DAYS_PER_YEAR
          + ((num_of_elapsed_years_since_1900 - 1) / 4) - OS_TIME_NUM_LEAP_YEARS_IN_RANGE_1900_1970
          + days_elapsed_minus_one_since_year_to_month;

    const int32_t num_days_elapsed_since_1970 = num_days_elapsed_since_1970_till_cur_month + p_tm_time_utc->tm_mday;
    if (num_days_elapsed_since_1970 < 0)
    {
        return (time_t)(-1);
    }

    const int64_t num_hours_elapsed_since_1970 = num_days_elapsed_since_1970 * OS_TIME_NUM_HOURS_PER_DAY
                                                 + p_tm_time_utc->tm_hour;
    if (num_hours_elapsed_since_1970 < 0)
    {
        return (time_t)(-1);
    }
    const int64_t num_minutes_elapsed_since_1970 = num_hours_elapsed_since_1970 * OS_TIME_NUM_MINUTES_PER_HOURS
                                                   + p_tm_time_utc->tm_min;
    if (num_minutes_elapsed_since_1970 < 0)
    {
        return (time_t)(-1);
    }
    const int64_t num_seconds_elapsed_since_1970 = num_minutes_elapsed_since_1970 * OS_TIME_NUM_SECONDS_PER_MINUTE
                                                   + p_tm_time_utc->tm_sec;
    if (num_seconds_elapsed_since_1970 < 0)
    {
        return (time_t)(-1);
    }

#if 1
    // this check should be removed if 64-bit time_t is used.
    if (num_seconds_elapsed_since_1970 > INT32_MAX)
    {
        return (time_t)(-1);
    }
#endif

    time_t unix_time = (time_t)num_seconds_elapsed_since_1970;
    if ((int64_t)unix_time != num_seconds_elapsed_since_1970)
    {
        return (time_t)(-1);
    }

    if (NULL == gmtime_r(&unix_time, p_tm_time_utc))
    {
        return (time_t)(-1);
    }

    return unix_time;
}
