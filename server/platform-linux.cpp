#include <zf_log.h>
#include "platform.h"
#include "devid2usb.h"
#include <unistd.h>

uint32_t get_id_from_usb_location(const char *sp_port_name, bool& ok)
{
    // Linux

    char link_name[256];
    char link_val[256];
    ssize_t count = -1;
    uint32_t id = 0;
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
            int l1, l2, l3, l4, l5, hub, ndev;
            if (sscanf(start, "%d-%d.%d.%d.%d:%d.%d", &l1, &l2, &l3, &hub, &ndev, &l4, &l5) == 7 && hub > 0)
            {
                id = (uint32_t)((hub - 1) * 4 + ndev);
                if (id > 9) id += 6; // bvvu strange stuff
            }
            else
                ok = false;
        }
        else ok = false;
    }
    else ok = false;
    return id;
}
