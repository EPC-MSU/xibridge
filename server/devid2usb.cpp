#include <zf_log.h>
#include "platform.h"
#include "devid2usb.h"
#include "mapdevidphnd.h"

struct sp_port ** ADevId2UsbConfor::pport_list = nullptr;
ReadWriteLock ADevId2UsbConfor::rwlock;

ADevId2UsbConfor::ADevId2UsbConfor()
{
}

void ADevId2UsbConfor::list_sp_ports()
{
    if (rwlock.is_write_lock_requested()) return; // if already locked for write - either other scan or destructor had been already called - new enumerate not needed so soon
    rwlock.write_lock();
    if (pport_list != nullptr) sp_free_port_list(pport_list);
    enum sp_return result = sp_list_ports(&pport_list);
    if (result != SP_OK)
    {
        ZF_LOGE("sp_list_port returned negative result!");
        pport_list = nullptr;
    }
    rwlock.write_unlock();
}

void ADevId2UsbConfor::print_sp_ports()
{
#if(ZF_LOG_ENABLED_DEBUG)    
    printf("Found ports:\n");
    rwlock.read_lock();
    for (int i = 0; pport_list[i] != NULL; i++)
    {
        struct sp_port *port = pport_list[i];
        int bus, addr;
        sp_get_port_usb_bus_address(port, &bus, &addr);
        char *pname = sp_get_port_name(port);
        bool ok;
        uint32_t location = get_id_from_usb_location(pname, ok);
        printf("Found port: %s, serial - %s, usb bus - %x, usb address - %x, description - %s, id based on location(bvvu) if any - %x.\n", sp_get_port_name(port), sp_get_port_usb_serial(port),
            bus, addr, sp_get_port_description(port), location);
    }
    rwlock.read_unlock();
#endif
}

void ADevId2UsbConfor::free_sp_ports()
{
    rwlock.write_lock();
    if (pport_list != nullptr) sp_free_port_list(pport_list);
    rwlock.write_unlock();
}


/*
* gets bvvu-understandable DevId (with serial number) from sp_port data
*/
DevId DevId2UsbBvvu::get_devid_from_sp_port(
    const struct sp_port *psp,
    bool &ok) const
{
    char *port_name = sp_get_port_name(psp);
    uint32_t id = get_id_from_usb_location(port_name, ok);
    return DevId(id);
}

static char *strlwr_portable(char *str)
{
    unsigned char *p = (unsigned char *)str;

    while (*p) {
        *p = tolower((unsigned char)*p);
        p++;
    }

    return str;
}

/*
* gets DevId by com (windows) and bus address (linux), masos - to do from sp_port data
*/
DevId DevId2UsbByComOrAddr::get_devid_from_sp_port(
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
    if (sp_get_port_usb_serial(psp) == nullptr)
    {
        ok = false;
    }
    else
    {
        sp_get_port_usb_bus_address(psp, &bus, &addr);
    }
    id = (uint32_t)addr;
#endif
    return DevId(id);
}

/*
* gets ximc-understandable DevId (with serial number) from sp_port data
*/
DevId DevId2UsbBySerial::get_devid_from_sp_port(
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
DevId DevId2UsbBySerialPidVid::get_devid_from_sp_port(
    const struct sp_port *psp,
    bool &ok) const
{
    DevId ret = DevId2UsbBySerial::get_devid_from_sp_port(psp, ok);
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

std::string DevId2UsbBvvu::port_name_by_devid(const DevId& devid) const
{
    std::string s = "";
    rwlock.read_lock();
    if (pport_list != nullptr)
    {
        for (int i = 0; pport_list[i] != NULL; i++)
        {
            if (is_devid_matchs_sp_port(devid, pport_list[i]))
                s = sp_get_port_name(pport_list[i]);
        }
    }
    rwlock.read_unlock();
    return s;
}

/*
* gets port name which could be opened by system from devid
*/
std::string DevId2UsbByComOrAddr::port_name_by_devid(const DevId& devid) const
{
    // some different port configure ways on ddifferent OS
#if WIN32   
    return serial_to_address(devid.id());
#else   
    std::string s = "";
    rwlock.read_lock();
    if (pport_list != nullptr)
    {
        for (int i = 0; pport_list[i] != NULL; i++)
        {
            if (is_devid_matchs_sp_port(devid, pport_list[i]))
                s = sp_get_port_name(pport_list[i]);
        }
    }
    rwlock.read_unlock();
    return s;
#endif    
}

std::string DevId2UsbBySerial::port_name_by_devid(const DevId& devid) const
{
    std::string s = "";
    rwlock.read_lock();
    if (pport_list != nullptr)
    {
        for (int i = 0; pport_list[i] != NULL; i++)
        {
            if (is_devid_matchs_sp_port(devid, pport_list[i]))
                s = sp_get_port_name(pport_list[i]);
        }
    }
    rwlock.read_unlock();
    return s;
}

std::vector<DevId> ADevId2UsbConfor::enumerate_dev(bool list_ports) const
{
    std::vector<DevId> devids;
    if (list_ports) list_sp_ports();
    rwlock.read_lock();
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
    rwlock.read_unlock();
    if (list_ports)
    {
        MapDevIdPHandle::notify_devs_rescan();
    }
    return devids;
}

// virtual constructor from cmd line param
ADevId2UsbConfor *create_appropriate_dev_id_2_usb_configurator(const char *cmd_par)
{
    if (strcmp(cmd_par, "bvvu") == 0) return new DevId2UsbBvvu();
    else if (strcmp(cmd_par, "by_com_addr") == 0) return new DevId2UsbByComOrAddr();
    else if (strcmp(cmd_par, "by_serial") == 0) return new DevId2UsbBySerial();
    else if (strcmp(cmd_par, "by_serialpidvid") == 0) return new DevId2UsbBySerialPidVid();
    return nullptr;
}
