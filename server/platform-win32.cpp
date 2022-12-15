#include "platform.h"
#include "zf_log.h"
#include <WinSock2.h>


std::string serial_to_address(uint32_t serial)
{
    static const std::string addr_prefix = "com:\\\\.\\COM";

    char devstr[4]; // large enough to hold a 999
    sprintf_s(devstr, "%i", serial);

    const std::string addr = addr_prefix + devstr;
    return addr;
}

int initialization()
{
    WSADATA wsaData;

    int iResult;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        ZF_LOGE("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    return 0;
}