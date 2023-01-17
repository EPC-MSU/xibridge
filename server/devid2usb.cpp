#include <zf_log.h>
#include "platform.h"
#include "devid2usb.h"

struct sp_port ** ADevId2UsbConfor::pport_list = nullptr;

void ADevId2UsbConfor::list_sp_ports()
{
    enum sp_return result = sp_list_ports(&pport_list);

    if (result != SP_OK)
    {
        ZF_LOGE("sp_list_port retuned negative result!");
        pport_list = nullptr;
    }
}

void ADevId2UsbConfor::print_sp_ports()
{
#if(ZF_LOG_ENABLED_DEBUG)    
    ZF_LOGD("Found ports:");
    for (int i = 0; pport_list[i] != NULL; i++)
    {
        struct sp_port *port = pport_list[i];
        int bus, addr;
        sp_get_port_usb_bus_address(port, &bus, &addr);
        ZF_LOGD("Found port: %s, serial %s, usb bus %x, usb address %x.", sp_get_port_name(port), sp_get_port_usb_serial(port),
            bus, addr);
    }
#endif
}

void ADevId2UsbConfor::free_sp_ports()
{
    if (pport_list != nullptr) sp_free_port_list(pport_list);
}

/*
* gets urpc-understandable DevId (with serial number) from sp_port data
*/
DevId DevId2UsbUrpc::get_devid_from_sp_port(
    const struct sp_port *psp,
    bool &ok) const
{
    // getting number from port name
    char portname[256];
    uint32_t id;
    ok = true;
#if WIN32
    id = 0;
    char *pname = sp_get_port_name(psp);
    // anyway, we need just usb with non-null serial
    if (sp_get_port_usb_serial(psp) != nullptr && pname != nullptr && strlen(pname) < 256 )
    {
        memcpy(portname, pname, strlen(pname));
        strlwr_portable(portname);
        char *start = strstr(portname, "com");
        if (start == nullptr) ok = false;
        if (sscanf(start, "com%u", &id) != 1) ok = false;
    }
    else ok = false;
#else
    int bus, addr;
    psp->sp_get_usb_bus_address(psp, &bus, &addr);
    id = (uint32_t)addr;
#endif
    return DevId(id);
}

/*
* gets ximc-understandable DevId (with serial number) from sp_port data
*/
DevId DevId2UsbXimc::get_devid_from_sp_port(
    const struct sp_port *psp,
    bool &ok) const
{
    uint32_t id = 0;
    ok = true;
    char *serial_s = sp_get_port_usb_serial(psp);
    if (serial_s == nullptr) ok = false;
    else
    {
        int pos = (int)strlen(serial_s);
        if (pos < 4) ok = false;
        else if (sscanf((serial_s + pos - 4), "%4x", &id) != 1)
            ok = false;
    }
    return DevId(id);
}

/*
* gets ximc-understandable serial number from sp_port data
*/
DevId DevId2UsbXimcExt::get_devid_from_sp_port(
    const struct sp_port *psp,
    bool &ok) const
{
    DevId ret = DevId2UsbXimc::get_devid_from_sp_port(psp, ok);
    int vid, pid;
    if (sp_get_port_usb_vid_pid(psp, &vid, &pid) != SP_OK) ok = false;
    return DevId(ret.id(), (uint16_t)pid, (uint16_t)vid);
}

/*
* checks if the device with given devid is same as one defined by the sp_port
*/
bool ADevId2UsbConfor::is_devid_matchs_sp_port(const DevId& devid, const struct sp_port *psp) const
{
    if (psp == nullptr) return false;
    bool ok;
    DevId did = get_devid_from_sp_port(psp, ok);
    if (!ok) return false;
    return devid == did;
}

/*
* gets port name which could be opened by system from devid
*/
std::string DevId2UsbUrpc::port_name_by_devid(const DevId& devid) const
{
   return serial_to_address(devid.id());
}

std::string DevId2UsbXimc::port_name_by_devid(const DevId& devid) const
{
    if (pport_list == nullptr) return "";
    for (int i = 0; pport_list[i] != NULL; i++)
    {
        if (is_devid_matchs_sp_port(devid, pport_list[i]))
            return sp_get_port_name(pport_list[i]);
    }
    return "";
}

std::vector<DevId> ADevId2UsbConfor::enumerate_dev() const
{
    std::vector<DevId> devids;

    for (int i = 0; pport_list[i] != NULL; i++)
    {
        struct sp_port *pport = pport_list[i];
        bool ok;
        DevId ret = get_devid_from_sp_port(pport, ok);
        if (ok)
        {
            devids.push_back(DevId(ret));
        }
    }
    return devids;
}

// virtual constructor from cmd line param
ADevId2UsbConfor *create_appropriate_dev_id_2_usb_configurator(const char *cmd_par)
{
    if (strcmp(cmd_par, "urpc") == 0) return new DevId2UsbUrpc();
    else if (strcmp(cmd_par, "ximc") == 0) return new DevId2UsbXimc();
    else if (strcmp(cmd_par, "ximc_ext") == 0) return new DevId2UsbXimcExt();
    return nullptr;
}
