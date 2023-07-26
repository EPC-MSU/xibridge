#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <functional>
#include <zf_log.h>
#include "mapdevidphnd.h"
#include "devid2usb.h"

std::map<DevId, std::mutex> MapDevIdPHandle::_mutex_pool;
ReadWriteLock MapDevIdPHandle::_mutex_pool_mutex;
const ADevId2UsbConfor * MapDevIdPHandle::_pdev2_usb_confor = nullptr;
void(*MapDevIdPHandle::cb_devsrescanned)() = 0;


xib_device_handle_t XibDevicePHandle::create_device_h(const DevId& devid)
{
    ADevId2UsbConfor::list_sp_ports();
    const std::string addr = MapDevIdPHandle::get_devid_2_usb_confor()->port_name_by_devid(devid);
        
    ZF_LOGD("Open device with id %u.", devid.id());
    xib_device_handle_t handle = xib_com_device_create(addr.c_str());
    if (handle == nullptr) 
	{
        ZF_LOGE("Can\'t open device %s.", addr.c_str());
    }
    return handle;
}

xib_result_t XibDevicePHandle::send_request(const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len)
{
    if (_uhandle != nullptr)
    {
        return xib_com_device_send_request(_uhandle, request, request_len, response, response_len);
    }
    return xib_result_nodevice;
}

/*
* Executes urpc request operation.
*/
xib_result_t XibDevicePHandle::urpc_send_request(const char cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len)
{
    if (_uhandle != nullptr)
    {
        return urpc_device_send_request(_uhandle, cid, request, request_len, response, response_len);
    }
    return xib_result_nodevice;
}

void XibDevicePHandle::destroy_device_h()
{
    if (_uhandle != nullptr)
    {
        ZF_LOGD("Urpc device handle %lu.", (unsigned long int)_uhandle);
        xib_com_device_destroy(_uhandle);
        _uhandle = nullptr;
    }
}

void MapDevIdPHandle::log()
{
    _rwlock.read_lock();

    ZF_LOGD("MapDevIdPHandle:");
    for (auto &m : *this)
    {
        ZF_LOGD("dev_id_id_%u -> (urpc ptr %lu\n", m.first.id(), (unsigned long int)m.second.uhandle());
    }

    ZF_LOGD("MapDevIdPHandle connections pairs:");
    for (auto &m : _conns)
    {
        ZF_LOGD("conn_id %u - serial (id %u)\n", m.first, m.second.id());
    }

    _rwlock.read_unlock();
}

MapDevIdPHandle::~MapDevIdPHandle()
{
    // some strange iterator behavior when it's container is empty
    if (size() != 0)
    {
        for (auto m : *this)
        {
            ZF_LOGD("Close device at deinit stage (id %u).", m.first.id());
            m.second.destroy_device_h();
        }
    }
 }

static bool _find_conn(const conn_serial &item, conn_id_t conn_id)
{
    return item.first == conn_id;
}

static bool _find_serial(const conn_serial &item, const DevId &devid)
{
    return item.second == devid;
}

void MapDevIdPHandle::lock_create_device_mutex(const DevId &devid)
{
    _mutex_pool_mutex.read_lock();
    if (_mutex_pool.find(devid) == _mutex_pool.cend())
    {
        _mutex_pool_mutex.read_unlock();
        _mutex_pool_mutex.write_lock();
        // this will be insertion
        std::mutex& m = _mutex_pool[devid];
        _mutex_pool_mutex.write_unlock();
        m.lock();
    }
    else
    {
        std::mutex& m = _mutex_pool[devid];
        _mutex_pool_mutex.read_unlock();
        m.lock();
    }
}
  
void MapDevIdPHandle::unlock_device_mutex(const DevId &devid)
{
    //std::mutex * pm = nullptr;
    _mutex_pool_mutex.read_lock();
    bool ret = false;
    if (_mutex_pool.find(devid) == _mutex_pool.cend())
    {
         ret = true;
    }
    _mutex_pool_mutex.read_unlock();
    if (ret) return;
    _mutex_pool[devid].unlock();
}

bool MapDevIdPHandle::_is_actual_connection(conn_id_t conn_id, DevId *pserial)
{
    if (pserial != nullptr)
    {
        *pserial = conn_id_invalid;
    }
    _rwlock.read_lock();

    bool ret = true;
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

bool MapDevIdPHandle::open_if_not(conn_id_t conn_id, const DevId &devid)
{
    if (_is_actual_connection(conn_id)) return true;
    lock_create_device_mutex(devid);
    _rwlock.write_lock();
    MapDevIdPHandle::iterator map_it = find(devid);
    /*
     * the device either create OK or was not created
     */
    if (map_it != end())
    {
        _conns.insert(_conns.cend(), std::make_pair(conn_id, devid));
        _rwlock.write_unlock();
        unlock_device_mutex(devid);
        return true;
    }

    /*
     * not created, create now
    */
    XibDevicePHandle &uh = (*this)[devid];
    if (uh.uhandle() == nullptr) // check if someone else already created this
    {
        _rwlock.write_unlock();
        xib_device_handle_t purpc = XibDevicePHandle::create_device_h(devid);
        _rwlock.write_lock();                              // multithreding !!!
        if (purpc != nullptr && find(devid) != cend())
        {
            (*this)[devid].set_device_h(purpc);
            _conns.insert(_conns.cend(), std::make_pair(conn_id, devid));
        }
        if ((*this)[devid].uhandle() ==  nullptr)
        {
           erase(devid);
        }
        _rwlock.write_unlock();
        unlock_device_mutex(devid);
        return purpc != nullptr;
    }
    else
    {
        _rwlock.write_unlock();
        unlock_device_mutex(devid);
        return true;
    }
}

void MapDevIdPHandle::remove_conn_or_remove_device(conn_id_t conn_id, const DevId &devid_kn, bool force_remove)
{
    // first, find and remove_connection
    DevId devid_real(UINT32_MAX);
    bool destroy_serial = false;
    DevId devid_known = devid_kn;
    //if (!_is_actual_connection(conn_id, &devid_real)) return;
    _is_actual_connection(conn_id, &devid_real);
    if (devid_known.id() == UINT32_MAX) devid_known = devid_real;
    if (devid_known.id() == UINT32_MAX) return;
    lock_create_device_mutex(devid_known);
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

    if (find(devid_known) != cend())
    {
        XibDevicePHandle &uh = (*this)[devid_known];
        if ((force_remove == true) ||
        // check if there is any device with this serial in the the _conns list of pairs
           std::find_if(_conns.cbegin(), _conns.cend(), std::bind(_find_serial, std::placeholders::_1, devid_known)) ==
           _conns.cend())
        {
            destroy_serial = true;
            _rwlock.read_unlock();
            ZF_LOGD("Close device (id %u).", devid_known.id());
            uh.destroy_device_h();
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
        unlock_device_mutex(devid_known);
        return;
    }
    _rwlock.write_lock();

    if (find(devid_known) != cend())
    {
        erase(devid_known);
    }
    
    _rwlock.write_unlock();
    unlock_device_mutex(devid_known);
}

xib_result_t MapDevIdPHandle::urpc_operation_send_request(const DevId &devid,
    const char cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len)
{
    xib_result_t res = xib_result_nodevice;
    _rwlock.read_lock();
    if (find(devid) != cend())
    {
        _rwlock.read_unlock();
        lock_create_device_mutex(devid);
        res = (*this)[devid].urpc_send_request(cid, request, request_len, response, response_len);
        unlock_device_mutex(devid);
        if (res == xib_result_nodevice)
        {
            remove_conn_or_remove_device(UINT32_MAX, devid, true);
            ZF_LOGE("The usb device with  devid id %u returned xib_result_nodevice and was closed", devid.id());
        }
    }
    else
    {
        _rwlock.read_unlock();
    }
    return res;
}
xib_result_t MapDevIdPHandle::operation_send_request(const DevId &devid,
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len)
{
    xib_result_t res = xib_result_nodevice;
    _rwlock.read_lock();
    if (find(devid) != cend())
    {
        _rwlock.read_unlock();
        lock_create_device_mutex(devid);
        res = (*this)[devid].send_request(request, request_len, response, response_len);
        unlock_device_mutex(devid);
        if (res == xib_result_nodevice)
        {
            remove_conn_or_remove_device(UINT32_MAX, devid, true);
            ZF_LOGE("The usb device with  devid id %u returned xib_result_nodevice and was closed", devid.id());
        }
    }
    else
    {
        _rwlock.read_unlock();
    }
    return res;
}

std::vector<std::string> MapDevIdPHandle::enumerate_devs_opened()
{
    std::vector<std::string> r;
    _rwlock.read_lock();
    for (auto &m : *this)
    {
        r.push_back(m.first.to_string_16hdigs());
    }
    _rwlock.read_unlock();
    return r;
}

std::vector<std::string> MapDevIdPHandle::enumerate_devs()
{
    std::vector<std::string> r;
    if (_pdev2_usb_confor == nullptr) return r;
    std::vector<DevId> v = _pdev2_usb_confor->list_to_dev_id_vector();
    for (auto &m : v)
    {
        r.push_back(m.to_string_16hdigs());
    }

    return r;
}
