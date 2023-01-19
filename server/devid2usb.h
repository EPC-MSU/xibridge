#ifndef DEVID2USB_H
#define DEVID2USB_H

#include <vector>
#include <libserialport.h>
#include "../common/utils.h"

/**
* ADevId2UsbConfor - abstact class to configure matching DevId (given at any server request) to comport/usb entitiy .
* In future, com/usb with definite device type connected to a port 
* 
*/
class ADevId2UsbConfor {
public:
    virtual ~ADevId2UsbConfor() 
    { free_sp_ports(); }
    
    // pure virtual funcs
    virtual std::string port_name_by_devid(const DevId& devid) const = 0;
    // non virtual funcs   
    bool is_devid_matchs_sp_port(
        const DevId& devid, 
        const struct sp_port *psp
        ) const;
    std::vector<DevId> enumerate_dev() const;

    static void print_sp_ports();

protected:
    ADevId2UsbConfor()
    {
        list_sp_ports();
    }

// pure virtual funcs
    virtual DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const = 0;

// static data member
    static struct sp_port **pport_list;

private:
    static void list_sp_ports();
    static void free_sp_ports();
};

class DevId2UsbUrpc : public ADevId2UsbConfor
{
public:
    DevId2UsbUrpc():
        ADevId2UsbConfor() { }
    virtual std::string port_name_by_devid(const DevId& devid) const;

protected:
    DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;
};

class DevId2UsbXimc : public ADevId2UsbConfor
{
public:
    DevId2UsbXimc() :ADevId2UsbConfor() { }
    virtual std::string port_name_by_devid(const DevId& devid) const;

protected:
    DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;
};

class DevId2UsbXimcExt : public DevId2UsbXimc
{
public:
    DevId2UsbXimcExt():DevId2UsbXimc() { }

protected:
    DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;
};


// virtual constructor to create 
extern ADevId2UsbConfor *create_appropriate_dev_id_2_usb_configurator(const char *cmd_par);

extern char *strlwr_portable(char *str);

#endif