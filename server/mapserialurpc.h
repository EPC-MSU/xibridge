#ifndef MAPSERIALURPC_H
#define MAPSERIALURPC_H

#include <map>
#include <mutex>
#include "../urpc.h"
#include "common.hpp"
#include "platform.h"
#include "rw_lock.h"

/**
 * UrpcDevicePHandle - class to contain urpc device handle pointer.
 * An Urpc device handle represents some internal resource associated with the urpc device communcation
 */
 class UrpcDevicePHandle {
public:
    UrpcDevicePHandle() : _uhandle(nullptr){ }
    /*
     * Creates urpc device handle pointer, calls urpc device creation function
    */
    static urpc_device_handle_t  create_urpc_h(uint32_t serial); // creates urpc device handle

    /*
    * Executes urpc request operation.
    */
    urpc_result_t urpc_send_request(const char cid[URPC_CID_SIZE],
        const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len);

    urpc_device_handle_t uhandle() const { return _uhandle; }

    /*
    * Destroys urpc device
    */
    void destroy_urpc_h();

    void set_urpc_h(urpc_device_handle_t h) { _uhandle = h; }

    UrpcDevicePHandle(const UrpcDevicePHandle &uh)
    {
        _uhandle = uh.uhandle();
    }

    UrpcDevicePHandle & operator=(const UrpcDevicePHandle&other)
    {
        _uhandle = other.uhandle();
        return *this;
    }
  
private:
    urpc_device_handle_t _uhandle;
};

/*
 * typedef for spying connections pairs
 */
typedef std::pair<conn_id_t, uint32_t>
conn_serial;

/*
 * MapSerialUrpc - class to hold all involved urpc_devices_handle pointers in issue of multithreading.
 * Tcp-connections account is made by using of _conns list : to remove urpc device that could not be addressed;
 */
class MapSerialUrpc : public
    std::map <uint32_t, UrpcDevicePHandle> // map :serial -> UrpcDevicePHandle

{
public:
    MapSerialUrpc(){};
    ~MapSerialUrpc();

    /*
     * Checks if the connection and serial has been already opened.
     * Opens if has not.
     */
    bool open_if_not(conn_id_t conn_id, uint32_t serial);

    /*
     * Executes urpc request operation.
     */
    urpc_result_t operation_urpc_send_request(uint32_t serial,
        const char cid[URPC_CID_SIZE],
        const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len);

    /*
     * Removes connection if any, check if any of the rest of active connections matches the given serial,
     * removes urpc device if no connections exist
     * If force_urpc_remove is on, serial must be known and all associated connections will be removed from
     * the this map
     * Choose UINT32_MAX for unknown conn_id or unknown serial, the serial can be evaluated
     * while their connection id is known
     */
    void remove_conn_or_remove_urpc_device(conn_id_t conn_id, uint32_t serial, bool force_urpc_remove = false);
    void log();
    

private:
    ReadWriteLock _rwlock;
     // spy for tcp-connections
    std::list<conn_serial> _conns;
    /*
    * These next four members to simplify synchronization on the given device operations 
    */
    static ReadWriteLock _mutex_pool_mutex;
    static std::map<uint32_t, std::mutex *> _mutex_pool;
    static void lock_create_device_mutex(uint32_t serial);
    static void unlock_device_mutex(uint32_t serial);
    static void free_mutex_pool();
    /**
    * Checks if the given connection is actual and can write the serial corresponding to thic connection
    */
    bool _is_actual_connection(conn_id_t conn_id, uint32_t *pserial = nullptr);
};

#endif
