#include "platform.h"
#include <execinfo.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <zf_log.h>


int initialization()
{
    // Don't remove! Otherwise, will fail on send() operation, if socket fails (inside bindy, inside cryptopp)!
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGPIPE, &act, NULL);
    return 0;
}

void handler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 32);

    // print out all the frames to stderr
    ZF_LOGE("IN SIGNAL HANDLER: signal no %d:\n", sig);
    ZF_LOGE("Stack trace...");
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    ZF_LOGE("End of stack trace.");
    exit(1);
}

void msec_sleep(
    unsigned int msec
    )
{
    // POSIX 1.b
    struct timespec ts;
    ts.tv_sec = (time_t)(msec / 1E3);
    ts.tv_nsec = (long)(msec * 1E6 - ts.tv_sec * 1E9);
    if (nanosleep(&ts, NULL) != 0)
    {
        ZF_LOGE("nanosleep failed: ");
    }
}