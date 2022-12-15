#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <functional>
#include <zf_log.h>
#include "mapserialurpc.h"

std::map<uint32_t, std::mutex *> MapSerialUrpc::_mutex_pool;
ReadWriteLock MapSerialUrpc::_mutex_pool_mutex;

urpc_device_handle_t UrpcDevicePHandle::create_urpc_h(uint32_t serial)
{
    const std::string addr = serial_to_address(serial);
    ZF_LOGD("Open device %u.", serial);
    urpc_device_handle_t handle = urpc_device_create(addr.c_str());
    if (handle == nullptr) 
	{
        ZF_LOGE("Can\'t open device %s.", addr.c_str());
    }
    return handle;
}

/*
 * Executes urpc request operation.
 */
urpc_result_t UrpcDevicePHandle::urpc_send_request(const char cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len)
{
    if (_uhandle != nullptr)
    {
        return urpc_device_send_request(_uhandle, cid, request, request_len, response, response_len);
    }
    return urpc_result_nodevice;
}

void UrpcDevicePHandle::destroy_urpc_h()
{
    if (_uhandle != nullptr)
    {
        ZF_LOGD("Urpc device handle %u.", _uhandle);
        urpc_device_destroy(&_uhandle);
        _uhandle = nullptr;
    }
}

void MapSerialUrpc::free_mutex_pool()
{
    // some strange iterator behavior when it's container is empty
    if (_mutex_pool.size() == 0) return;
    std::map<uint32_t, std::mutex *>::const_iterator mpli = _mutex_pool.cbegin();
 
    for (; mpli != _mutex_pool.cend(); mpli++)
    {
        delete mpli -> second;
    } 
}

void MapSerialUrpc::log()
{
    _rwlock.read_lock();

    ZF_LOGD("MapSerialUrpc:");
    for (auto &m : *this)
    {
        ZF_LOGD("serial_%u -> (urpc ptr %u\n", m.first, m.second.uhandle());
    }

    ZF_LOGD("MapSerialUrpc connections pairs:");
    for (auto &m : _conns)
    {
        ZF_LOGD("conn_id %u - serial %u\n", m.first, m.second);
    }

    _rwlock.read_unlock();
}

MapSerialUrpc::~MapSerialUrpc()
{
    // some strange iterator behavior when it's container is empty
    if (size() != 0)
    {
        for (auto m : *this)
        {
            ZF_LOGD("Close device at deinit stage %u.", m.first);
            m.second.destroy_urpc_h();
        }
    }
   free_mutex_pool();
}

static bool _find_conn(const conn_serial &item, conn_id_t conn_id)
{
    return item.first == conn_id;
}

static bool _find_serial(const conn_serial &item, uint32_t serial)
{
    return item.second == serial;
}

void MapSerialUrpc::lock_create_device_mutex(uint32_t serial)
{
    std::mutex * pm = nullptr;
    _mutex_pool_mutex.read_lock();
    if (_mutex_pool.find(serial) == _mutex_pool.cend())
    {
        _mutex_pool_mutex.read_unlock();
        pm = new std::mutex();
        _mutex_pool_mutex.write_lock();
        _mutex_pool.insert(std::make_pair(serial, pm));
        _mutex_pool_mutex.write_unlock();
    }
    else
    {

        pm = _mutex_pool[serial];
        _mutex_pool_mutex.read_unlock();
    }
    pm->lock();
}

void MapSerialUrpc::unlock_device_mutex(uint32_t serial)
{
    std::mutex * pm = nullptr;
    _mutex_pool_mutex.read_lock();
    bool ret = false;
    if (_mutex_pool.find(serial) == _mutex_pool.cend())
    {
         ret = true;
    }
    _mutex_pool_mutex.read_unlock();
    if (ret) return;
    _mutex_pool[serial]->unlock();
}

bool MapSerialUrpc::_is_actual_connection(conn_id_t conn_id, uint32_t *pserial)
{
    if (pserial != nullptr)
    {
        *pserial = conn_id_invalid;
    }
    _rwlock.read_lock();

    bool ret = false;
    // first, glance, if any is already in list
    std::list<conn_serial>::const_iterator it;
    if ((it = std::find_if(_conns.cbegin(), _conns.cend(), std::bind(_find_conn, std::placeholders::_1, conn_id))) !=
        _conns.cend())
    {
        if (pserial != nullptr)
        {
           *pserial = it->second;
        }
    }
    else
    {
        ret = false;
    }
    _rwlock.read_unlock();
    return ret;
}

bool MapSerialUrpc::open_if_not(conn_id_t conn_id, uint32_t serial)
{
    if (_is_actual_connection(conn_id)) return true;
    lock_create_device_mutex(serial);
    _rwlock.write_lock();
    MapSerialUrpc::iterator map_it = find(serial);
    /*
     * the device either create OK or was not created
     */
    if (map_it != end())
    {
        _conns.insert(_conns.cend(), std::make_pair(conn_id, serial));
        _rwlock.write_unlock();
        unlock_device_mutex(serial);
        return true;
    }

    /*
     * not created, create now
    */
    UrpcDevicePHandle &uh = (*this)[serial];
    if (uh.uhandle() == nullptr) // check if someone else already created this
    {
        _rwlock.write_unlock();
        urpc_device_handle_t purpc = UrpcDevicePHandle::create_urpc_h(serial);
        _rwlock.write_lock();                              // multithreding !!!
        if (purpc != nullptr && find(serial) != cend())
        {
            (*this)[serial].set_urpc_h(purpc);
            _conns.insert(_conns.cend(), std::make_pair(conn_id, serial));
        }
        if ((*this)[serial].uhandle() ==  nullptr)
        {
           erase(serial);
        }
        _rwlock.write_unlock();
        unlock_device_mutex(serial);
        return purpc != nullptr;
    }
    else
    {
        _rwlock.write_unlock();
        unlock_device_mutex(serial);
        return true;
    }
}

void MapSerialUrpc::remove_conn_or_remove_urpc_device(conn_id_t conn_id, uint32_t serial_known, bool force_urpc_remove)
{
    // first, find and remove_connection
    uint32_t serial_real = UINT32_MAX;
    bool destroy_serial = false;
    if (force_urpc_remove == true || _is_actual_connection(conn_id, &serial_real)) return;
    if (serial_known == UINT32_MAX) serial_known = serial_real;
    if (serial_known == UINT32_MAX) return;
    lock_create_device_mutex(serial_real);
    _rwlock.write_lock();

    std::list<conn_serial>::const_iterator it;
    // find the conn_id connection in _conns list of pairs 
    if ((it = std::find_if(_conns.cbegin(), _conns.cend(), std::bind(_find_conn, std::placeholders::_1, conn_id))) !=
            _conns.cend())
    {
        _conns.erase(it);
    }
    _rwlock.write_unlock();

    _rwlock.read_lock();

    if (find(serial_known) != cend())
    {
        UrpcDevicePHandle &uh = (*this)[serial_known];
        if ((force_urpc_remove == true) ||
        // check if there is any device with this serial in the the _conns list of pairs
           std::find_if(_conns.cbegin(), _conns.cend(), std::bind(_find_serial, std::placeholders::_1, serial_known)) ==
           _conns.cend())
        {
            destroy_serial = true;
            _rwlock.read_unlock();
            ZF_LOGD("Close device %u.", serial_known);
            uh.destroy_urpc_h();
        }
		else
		{

			_rwlock.read_unlock();
		}
    }
    else
    {
	    _rwlock.read_unlock();
    }
    if (!destroy_serial)
    {
        unlock_device_mutex(serial_known);
        return;
    }
    _rwlock.write_lock();

    if (find(serial_known) != cend())
    {
        erase(serial_known);
    }
    
    _rwlock.write_unlock();
    unlock_device_mutex(serial_known);
}

urpc_result_t MapSerialUrpc::operation_urpc_send_request(uint32_t serial,
    const char cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len)
{
    urpc_result_t res = urpc_result_nodevice;
    _rwlock.read_lock();
    if (find(serial) != cend())
    {
        _rwlock.read_unlock();
        lock_create_device_mutex(serial);
        res = (*this)[serial].urpc_send_request(cid, request, request_len, response, response_len);
        unlock_device_mutex(serial);
        if (res == urpc_result_nodevice)
        {
            remove_conn_or_remove_urpc_device(UINT32_MAX, serial, true);
            ZF_LOGE("The urpc device with  serial %u returned urpc_result_nodevice and was closed", serial);
        }
    }
    else
    {
        _rwlock.read_unlock();
    }
    return res;
}
