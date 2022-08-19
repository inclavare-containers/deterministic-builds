#include <time.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/time.h>
#include "../config/time_config.h"


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
