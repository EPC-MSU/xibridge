#include "platform.h"

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/file.h>
#include <sys/time.h>
#include <string.h>

#ifdef __APPLE__
    /* We need IOKit */
    #include <CoreFoundation/CoreFoundation.h>
    #include <IOKit/IOKitLib.h>
    #include <IOKit/usb/IOUSBLib.h>
    #include <sys/sysctl.h>
    /* and time too */
    #include <mach/clock.h>
    #include <mach/mach.h>
#endif

#include <zf_log.h>

/*
 * Misc
 */

void urpc_msec_sleep(
    unsigned int msec
)
{
    // POSIX 1.b
    struct timespec ts;
    ts.tv_sec = (time_t)(msec / 1E3);
    ts.tv_nsec = (long)(msec * 1E6 - ts.tv_sec * 1E9);
    if (nanosleep( &ts, NULL ) != 0)
    {
        ZF_LOGE("nanosleep failed: ");
    }
}

void urpc_get_wallclock_us(
    uint64_t *us
)
{
    struct timeval now;
    gettimeofday(&now, 0);
    if (us)
    {
        *us = now.tv_sec * 1000000 + now.tv_usec;
    }
}

void urpc_get_wallclock(
    time_t *sec, int *msec
)
{
    struct timeval now;
    gettimeofday(&now, 0);
    if (sec && msec)
    {
        *sec = now.tv_sec;
        *msec = now.tv_usec / 1000;
    }
}
