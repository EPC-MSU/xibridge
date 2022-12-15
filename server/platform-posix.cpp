#include "platform.h"
#include "signal.h"

std::string serial_to_address(uint32_t serial)
{
    static const std::string addr_prefix = "com:///dev/ximc/";

    char devstr[9]; // large enough to hold a uint32_t serial in hex + terminating null, so 9 bytes
    sprintf(devstr, "%08X", serial);

    const std::string addr = addr_prefix + devstr;
    return addr;
}

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