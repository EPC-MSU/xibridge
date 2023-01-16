#ifndef DEVID2USB_H
#define DEVID2USB_H

#include <vector>
#include <libserialport.h>
#include "../common/utils.h"

enum device_conf_style {
    dcs_urpc,          // urpc convention used   
    dcs_ximc,          // ximc convention used
    dcs_ximc_ext       // ximc extended (no practice in use yet)
};

/**
* ADevId2UsbConfor - abstact class to configure matching DevId (given at any server request) to comport/usb entitiy .
* In future, com/usb with definite device type connected to a port 
* 
*/
class ADevId2UsbConfor {
public:
    virtual ~ADevId2UsbConfor() { free_sp_ports; }
    // pure virtual funcs
    virtual DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const = 0;
    virtual device_conf_style get_configure_type() const = 0;
    virtual std::string port_name_by_devid(const DevId& devid) const = 0;
    // non virtual funcs   
    bool is_devid_matchs_sp_port(const DevId& devid, const struct sp_port *psp) const;
    std::vector<DevId> enumerate_dev() const;
    // static member funcs
    static void list_sp_ports();
    static void free_sp_ports();
    static void print_sp_ports();

protected:
    ADevId2UsbConfor() { list_sp_ports(); }
    static struct sp_port **pport_list;
};

class DevId2UsbUrpc : public ADevId2UsbConfor
{
public:
    DevId2UsbUrpc() :ADevId2UsbConfor() { }

    DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;

    virtual device_conf_style get_configure_type() const 
        {return dcs_urpc;}

    virtual std::string port_name_by_devid(const DevId& devid) const;

};

class DevId2UsbXimc : public ADevId2UsbConfor
{
public:
    DevId2UsbXimc() :ADevId2UsbConfor() { }

    virtual DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;

    virtual device_conf_style get_configure_type() const
        {return dcs_ximc;}

    virtual std::string port_name_by_devid(const DevId& devid) const;
};

class DevId2UsbXimcExt : public DevId2UsbXimc
{
public:
    DevId2UsbXimcExt():DevId2UsbXimc() { }

    virtual DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;

    virtual device_conf_style get_configure_type() const 
    {
        return dcs_ximc_ext;
    }

    // the same func is ok
    //virtual std::string port_name_by_devid(const DevId& devid);
};


// virtual constructor to create 
extern ADevId2UsbConfor *create_appropriate_dev_id_2_usb_configurator(const char *cmd_par);

extern char *strlwr_portable(char *str);

#endif