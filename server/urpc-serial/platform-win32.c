#include "platform.h"

#include <initguid.h>
// do not force to use DDK with MSVC or other
#ifdef _MSC_VER
    #include <winioctl.h>
#else
    #include <ddk/ntddser.h>
#endif
#include <setupapi.h>
#include <process.h>

#include <zf_log.h>

/*
 * Misc
 */

void urpc_msec_sleep(
    unsigned int msec
)
{
    Sleep(msec);
}

void urpc_get_wallclock_us(
    uint64_t *us
)
{
    const time_t DELTA_EPOCH_IN_MICROSECS = (time_t)11644473600000000;
    FILETIME ft;
    time_t tmpres = 0;
    if (us != NULL)
    {
        memset(&ft, 0, sizeof(ft));

        GetSystemTimeAsFileTime(&ft);

        tmpres = ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres /= 10;  /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        *us = (time_t)tmpres;
    }
}

void urpc_get_wallclock(
    time_t *sec,
    int *msec
)
{
    uint64_t us;
    urpc_get_wallclock_us(&us);
    *sec = (time_t)(us / 1000000);
    *msec = (us % 1000000) / 1000; // use milliseconds
}
