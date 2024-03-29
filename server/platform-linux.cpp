#include <zf_log.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/un.h>

#include "platform.h"
#include "devid2usb.h"

uint32_t get_id_from_usb_location(const char *sp_port_name, bool& ok)
{
    // Linux

    char link_name[256];
    char link_val[256];
    ssize_t count = -1;
    uint32_t id = 0;
    ok = false;
    if (strncmp(sp_port_name, "/dev/", 5) == 0)
    {
        memset(link_val, 0, 256);
        const char *dev = sp_port_name + 5;
        snprintf(link_name, sizeof(link_name), "/sys/class/tty/%s/device", dev);
        count = readlink(link_name, link_val, sizeof(link_val));
        if (count > 0)
        {
            const char *start = strrchr(link_val, '/');
            if (start != nullptr) start++;
            else start = link_val;
            int hub = 0;
            int ndev = 0;
            char *e;
            char *phub = nullptr;
            char *pusb = nullptr;
            if ((e = (char *)strchr(start, ':')) != nullptr) *e = 0; // trancate :1.0
            if (strchr(start, '.') == nullptr) pusb = (char *)strchr(start, '-');
            else
            {
                pusb = (char *)strrchr(start, '.');
                *pusb = 0;
                phub = (char *)strrchr(start, '.');
            }
            if (pusb) ok = (sscanf(++pusb, "%d", &ndev) == 1);
            if (phub && ok) ok = (sscanf(++phub, "%d", &hub) == 1);
            if (hub) hub--;
            id = (uint32_t)(hub * 4 + ndev);
            if (id > 9) id += 6; // bvvu strange stuff
        }
    }
    return id;
}

#define SOCKET_NAME "SOCKET_TO_BE_USED_AS_NAMED_MUTEX"
#ifndef UNIX_PATH_MAX                                                           
#define UNIX_PATH_MAX (108)                                                   
#endif
#define MIN(A,B) A<B?A:B

bool is_already_started()
{
    struct sockaddr_un SockAddr;
    int AddrLen;
    int Socket = socket(PF_UNIX, SOCK_STREAM, 0);
    if (Socket == -1)
    {
        std::cout << "Unable to open communication socket because of " << strerror(errno) << ". Exit." << std::endl;
        return true;
    }
    else
    {
        SockAddr.sun_family = AF_UNIX;
        memset(&SockAddr.sun_path, 0, UNIX_PATH_MAX);
        memcpy(&SockAddr.sun_path[1], SOCKET_NAME, MIN(strlen(SOCKET_NAME), UNIX_PATH_MAX));
        AddrLen = sizeof (SockAddr);
        if (bind(Socket, (struct sockaddr *) &SockAddr, AddrLen))
        {
            std::cout << "Another process (xxx_xinet_server) already running. Exit." << std::endl;
            return true;
        }
    }
    return false;
}
