#include "platform.h"
#include "signal.h"

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