#include <time.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/time.h>
#include "modify_time_config.h"


static char result[3+1+ 3+1+20+1+20+1+20+1+20+1+20+1 + 1];
// static char *
// asctime_internal (const struct tm *tp, char *buf, size_t buflen);
struct tm _tmbuf;


time_t f4ke(time_t *t)
{
    if (t) {
        *t = MODIFIED_TIMESTAMP;
    }
    return 0;
}

int clock_getf4ke(clockid_t __clock_id, struct timespec *__res)
{
    __res->tv_sec = MODIFIED_TIMESTAMP;
    __res->tv_nsec = 0;
    return 0;
}

extern int getf4keofday(struct timeval *tv, void *tz)
{
    tv->tv_sec = MODIFIED_TIMESTAMP;
    tv->tv_usec = 0;
    return 0;
}

// struct tm *localtime(const time_t *timer)
// {
//     _tmbuf.tm_sec = 0;
//     _tmbuf.tm_min = 0;
//     _tmbuf.tm_hour = 0;
//     _tmbuf.tm_mday = 1;
//     _tmbuf.tm_mon = 0;
//     _tmbuf.tm_year = 0;
//     _tmbuf.tm_wday = 0;
//     _tmbuf.tm_yday = 0;
//     _tmbuf.tm_isdst = 0;
//     return &_tmbuf;
// }

// char * asctime(const struct tm *tp)
// {
//     struct tm replace_tp;
//     replace_tp.tm_sec = 0;
//     replace_tp.tm_min = 0;
//     replace_tp.tm_hour = 0;
//     replace_tp.tm_mday = 1;
//     replace_tp.tm_mon = 0;
//     replace_tp.tm_year = 0;
//     replace_tp.tm_wday = 0;
//     replace_tp.tm_yday = 0;
//     replace_tp.tm_isdst = 0;

//     static const char wday_name[][4] = {
//         "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
//     };
//     static const char mon_name[][4] = {
//         "Jan", "Feb", "Mar", "Apr", "May", "Jun",
//         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
//     };
//     static char result[26];
//     sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
//         wday_name[replace_tp.tm_wday],
//         mon_name[replace_tp.tm_mon],
//         replace_tp.tm_mday, replace_tp.tm_hour,
//         replace_tp.tm_min, replace_tp.tm_sec,
//         1900 + replace_tp.tm_year);
//     return result;
//     // return asctime_internal (&replace_tp, result, sizeof (result));
// }
