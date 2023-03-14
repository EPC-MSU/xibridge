#ifndef DEVID2USB_H
#define DEVID2USB_H

#include <vector>
#include <libserialport.h>
#include "../common/utils.h"
#include "rw_lock.h"

/**
* ADevId2UsbConfor - abstact thread-safe class to configure matching DevId (given at any server request) to comport/usb entitiy .
* In future, com/usb with definite device type connected to a port 
* 
*/
class ADevId2UsbConfor {
public:
    virtual ~ADevId2UsbConfor() 
        { free_sp_ports(); }
    
    // pure virtual func
    virtual std::string port_name_by_devid(const DevId& devid) const = 0;
    
    std::vector<DevId> enumerate_dev() const;

    static void print_sp_ports();
    static void list_sp_ports();

protected:
    ADevId2UsbConfor()
    {
        list_sp_ports();
    }
    // non virtual funcs   
    bool is_devid_matchs_sp_port(
        const DevId& devid,
        const struct sp_port *psp
    ) const;

// pure virtual funcs
    virtual DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok
    ) const = 0;

// static data member
    static struct sp_port **pport_list;
    static ReadWriteLock rwlock;
private:
     static void free_sp_ports();
};

/**
* DevId2UsbByComOrAddr - thread-safe class to configure as urpc-style matching DevId (given at any server request) to comport/usb entitiy .
* In future, com/usb with definite device type connected to a port
*
*/
class DevId2UsbBvvu : public ADevId2UsbConfor
{
public:
    DevId2UsbBvvu() :
        ADevId2UsbConfor() { }
    virtual std::string port_name_by_devid(const DevId& devid) const;

protected:
    DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;
};

/**
* DevId2UsbByComOrAddr - thread-safe class to configure as urpc-style matching DevId (given at any server request) to comport/usb entitiy .
* In future, com/usb with definite device type connected to a port
*
*/
class DevId2UsbByComOrAddr : public ADevId2UsbConfor
{
public:
    DevId2UsbByComOrAddr():
        ADevId2UsbConfor() { }
    virtual std::string port_name_by_devid(const DevId& devid) const;

protected:
    DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;
};

/**
* DevId2UsbBySerial - thread-safe class to configure as ximc-style matching DevId (given at any server request) to comport/usb entitiy .
* In future, com/usb with definite device type connected to a port
*
*/
class DevId2UsbBySerial : public ADevId2UsbConfor
{
public:
    DevId2UsbBySerial() :ADevId2UsbConfor() { }
    virtual std::string port_name_by_devid(const DevId& devid) const;

protected:
    DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok
    ) const;
};

/**
* DevId2UsbBySerial - thread-safe class to configure as ximc_ext-style matching DevId (given at any server request) to comport/usb entitiy .
* In future, com/usb with definite device type connected to a port
*
*/
class DevId2UsbBySerialPidVid : public DevId2UsbBySerial
{
public:
    DevId2UsbBySerialPidVid():DevId2UsbBySerial() { }

protected:
    DevId get_devid_from_sp_port(
        const struct sp_port *psp,
        bool &ok) const;
};


// virtual constructor to create 
extern ADevId2UsbConfor *create_appropriate_dev_id_2_usb_configurator(const char *cmd_par);

extern char *strlwr_portable(char *str);

#endif