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
