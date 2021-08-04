/**
 * @file os_time.c
 * @author TheSomeMan
 * @date 2020-11-30
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_time.h"

time_t
os_time_get(void)
{
    return time(NULL);
}

const char *
os_time_month_name_short(const os_time_month_e month)
{
    switch (month)
    {
        case OS_TIME_MONTH_JAN:
            return "Jan";
        case OS_TIME_MONTH_FEB:
            return "Feb";
        case OS_TIME_MONTH_MAR:
            return "Mar";
        case OS_TIME_MONTH_APR:
            return "Apr";
        case OS_TIME_MONTH_MAY:
            return "May";
        case OS_TIME_MONTH_JUN:
            return "Jun";
        case OS_TIME_MONTH_JUL:
            return "Jul";
        case OS_TIME_MONTH_AUG:
            return "Aug";
        case OS_TIME_MONTH_SEP:
            return "Sep";
        case OS_TIME_MONTH_OCT:
            return "Oct";
        case OS_TIME_MONTH_NOV:
            return "Nov";
        case OS_TIME_MONTH_DEC:
            return "Dec";
        default:
            return "???";
    }
}

const char *
os_time_month_name_long(const os_time_month_e month)
{
    switch (month)
    {
        case OS_TIME_MONTH_JAN:
            return "January";
        case OS_TIME_MONTH_FEB:
            return "February";
        case OS_TIME_MONTH_MAR:
            return "March";
        case OS_TIME_MONTH_APR:
            return "April";
        case OS_TIME_MONTH_MAY:
            return "May";
        case OS_TIME_MONTH_JUN:
            return "June";
        case OS_TIME_MONTH_JUL:
            return "July";
        case OS_TIME_MONTH_AUG:
            return "August";
        case OS_TIME_MONTH_SEP:
            return "September";
        case OS_TIME_MONTH_OCT:
            return "October";
        case OS_TIME_MONTH_NOV:
            return "November";
        case OS_TIME_MONTH_DEC:
            return "December";
        default:
            return "?????";
    }
}

const char *
os_time_wday_name_short(const os_time_wday_e day_of_the_week)
{
    switch (day_of_the_week)
    {
        case OS_TIME_WDAY_SUN:
            return "Su";
        case OS_TIME_WDAY_MON:
            return "Mo";
        case OS_TIME_WDAY_TUE:
            return "Tu";
        case OS_TIME_WDAY_WED:
            return "We";
        case OS_TIME_WDAY_THU:
            return "Th";
        case OS_TIME_WDAY_FRI:
            return "Fr";
        case OS_TIME_WDAY_SAT:
            return "Sa";
        default:
            return "??";
    }
}

const char *
os_time_wday_name_mid(const os_time_wday_e day_of_the_week)
{
    switch (day_of_the_week)
    {
        case OS_TIME_WDAY_SUN:
            return "Sun";
        case OS_TIME_WDAY_MON:
            return "Mon";
        case OS_TIME_WDAY_TUE:
            return "Tue";
        case OS_TIME_WDAY_WED:
            return "Wed";
        case OS_TIME_WDAY_THU:
            return "Thu";
        case OS_TIME_WDAY_FRI:
            return "Fri";
        case OS_TIME_WDAY_SAT:
            return "Sat";
        default:
            return "???";
    }
}

const char *
os_time_wday_name_long(const os_time_wday_e day_of_the_week)
{
    switch (day_of_the_week)
    {
        case OS_TIME_WDAY_SUN:
            return "Sunday";
        case OS_TIME_WDAY_MON:
            return "Monday";
        case OS_TIME_WDAY_TUE:
            return "Tuesday";
        case OS_TIME_WDAY_WED:
            return "Wednesday";
        case OS_TIME_WDAY_THU:
            return "Thursday";
        case OS_TIME_WDAY_FRI:
            return "Friday";
        case OS_TIME_WDAY_SAT:
            return "Saturday";
        default:
            return "???";
    }
}
