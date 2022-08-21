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


time_t time(time_t *t)
{
    if (t) {
        *t = MODIFIED_CURRENT_TIMESTAMP;
    }
    return MODIFIED_CURRENT_TIMESTAMP;
}
