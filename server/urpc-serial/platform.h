#ifndef URPC_DEVSERIAL_PLATFORM_H
#define URPC_DEVSERIAL_PLATFORM_H

#include <time.h>
#ifdef _MSC_VER
    //#define _CRT_SECURE_NO_WARNINGS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
 #else
    #include <unistd.h>
#endif
#include "devserial.h"


// time to wait to open port for normal work
#define URPC_ZEROSYNC_TRIGGER_TIMEOUT 10000
// syncronization attempts count
#define URPC_ZEROSYNC_RETRY_COUNT 4
// time to wait before retry attempts
#define URPC_ZEROSYNC_RETRY_DELAY 200
// amount of zeroes to send in case of an error
#define URPC_ZEROSYNC_BURST_SIZE 64
// system timeout for port functions
#define URPC_PORT_TIMEOUT 500

/*
 * Misc
 */
void
urpc_msec_sleep(
    unsigned int msec
);

void
urpc_get_wallclock(
    time_t *sec,
    int *msec
);

#endif //URPC_DEVSERIAL_PLATFORM_H
