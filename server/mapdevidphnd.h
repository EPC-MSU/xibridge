#ifndef MAPDEVIDPHND_H
#define MAPDEVIDPHND_H

#include <map>
#include <mutex>
#include "xib-serial/xib_com.h"
#include "common.hpp"
#include "platform.h"
#include "rw_lock.h"
#include "../common/utils.h"
#include "devid2usb.h"

/**
 * XibDevicePHandle - class to contain some xibridge-compatible (request with known response len and response) usb/com device handle pointer.
 * A device handle represents some internal resource associated with the xibridge-compatible device communcation via COM
 */
class XibDevicePHandle {
public:
    XibDevicePHandle() : _uhandle(nullptr){ }
    /*
     * Creates usb device handle pointer, calls device creation function
    */
    static xib_device_handle_t  create_device_h(const DevId& serial); // creates device handle from device identifier

    /*
    * Executes request operation in urpc emulating mode.
    */
    xib_result_t urpc_send_request(const char cid[URPC_CID_SIZE],
        const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len);

    /*
    * Executes common xibridge request-response operation.
    */
    xib_result_t send_request( const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len);

    xib_device_handle_t uhandle() const { return _uhandle; }

    /*
    * Destroys device
    */
    void destroy_device_h();

    void set_device_h(xib_device_handle_t h) { _uhandle = h; }

    XibDevicePHandle(const XibDevicePHandle &uh)
    {
        _uhandle = uh.uhandle();
    }

    XibDevicePHandle & operator=(const XibDevicePHandle&other)
    {
        _uhandle = other.uhandle();
        return *this;
    }
  
private:
    xib_device_handle_t _uhandle;
};

/*
 * typedef for spying connections pairs
 */
typedef std::pair<conn_id_t, DevId>
conn_serial;

/*
 * MapDevIdPHandle - class to hold all involved usb_devices_handle pointers in issue of multithreading.
 * Tcp-connections account is made by using of _conns list : to remove device that could not be addressed;
 */
class MapDevIdPHandle : public
    std::map <DevId, XibDevicePHandle> // map : DevId ->  device PHandle

{
public:
    MapDevIdPHandle() {};
    ~MapDevIdPHandle();

    /*
     * Checks if the connection and serial has been already opened.
     * Opens if has not.
     */
    bool open_if_not(conn_id_t conn_id, const DevId &devid);

    /*
    * Executes urpc request operation.
    */
    xib_result_t urpc_operation_send_request(const DevId &devid,
        const char cid[URPC_CID_SIZE],
        const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len);

    /*
    * Executes common request-response operation.
    */
    xib_result_t operation_send_request(const DevId &devid,
        const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len
    );

    /*
     * Removes connection if any, check if any of the rest of active connections matches the given serial,
     * removes usb device if no connections exist
     * If force_remove is on, serial must be known and all associated connections will be removed from
     * the this map
     * Choose UINT32_MAX for unknown conn_id or unknown devid, the devid can be evaluated
     * while their connection id is known
     */
    void remove_conn_or_remove_device(
        conn_id_t conn_id, 
        const DevId &devid_known, 
        bool force_remove = false
    );
    void log();
    static void set_devid_2_usb_confor(const ADevId2UsbConfor *pdev2usb) 
        {_pdev2_usb_confor = pdev2usb;}
    static const ADevId2UsbConfor *get_devid_2_usb_confor() 
        {return _pdev2_usb_confor;}

private:
    ReadWriteLock _rwlock;
     // spy for tcp-connections
    std::list<conn_serial> _conns;
    /*
    * These next four members to simplify synchronization on the given device operations 
    */
    static ReadWriteLock _mutex_pool_mutex;
    static std::map<DevId, std::mutex *> _mutex_pool;
    static void lock_create_device_mutex(const DevId &devid);
    static void unlock_device_mutex(const DevId &devid);
    static void free_mutex_pool();
    
    /**
    * Checks if the given connection is actual and can write the serial corresponding to thic connection
    */
    bool _is_actual_connection(conn_id_t conn_id, DevId *devid = nullptr);
    static const ADevId2UsbConfor *_pdev2_usb_confor;
};

#endif
