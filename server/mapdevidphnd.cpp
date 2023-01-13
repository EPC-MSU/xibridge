#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <functional>
#include <zf_log.h>
#include "mapdevidphnd.h"
#include <libserialport.h>

extern char *strlwr_portable(char *str);

//from libserialport.h
static struct sp_port **pport_list = nullptr;

device_id_style MapDevIdPHandle::_dis = dis_urpc;

void list_sp_ports()
{
    enum sp_return result = sp_list_ports(&pport_list);

    if (result != SP_OK) 
    {
        ZF_LOGE("sp_list_port retuned negative result!");
        pport_list = nullptr;
    }

    ZF_LOGD("Found ports:");
    for (int i = 0; pport_list[i] != NULL; i++) 
    {
        struct sp_port *port = pport_list[i];
        ZF_LOGD("Found port: %s, serial %s.", sp_get_port_name(port), sp_get_port_usb_serial(port));
    }
}

void free_sp_ports()
{
    if (pport_list != nullptr) sp_free_port_list(pport_list);
}

/*
* gets urpc-understandable serial number from sp_port data 
*/
uint32_t xibridge_get_urpc_serial_from_sp_port(
    const struct sp_port *psp, 
    bool &ok)
{
    // getting number from port name
    char portname[256];
    uint32_t id = 0;
    char *pname = sp_get_port_name(psp);
    if (strlen(pname) < 256)
    {
        memcpy(portname, pname, strlen(pname));
        strlwr_portable(portname);
#if WIN32
        char *start = strstr(portname, "com");
        if (start == nullptr) ok = false;
        if (sscanf(start, "com%u", &id) != 1) ok = false;
#else

#endif
    }
    else ok = false;
    return id;
}

/*
* gets ximc-understandable serial number from sp_port data
*/
uint32_t xibridge_get_ximc_serial_from_sp_port(
    const struct sp_port *psp,
    bool &ok)
{
    uint32_t id;
    char *serial_s = sp_get_port_usb_serial(psp);
    int pos = (int)strlen(serial_s);
    if (pos < 4) ok = false;
    for (int k = 4; k > 0; k--)
    {
        id *= 16;
        id += serial_s[pos - k];
    }
    return id;
}

/*
* checks if the device with given devid is same as one defined by the sp_port
*/
bool xibridge_is_devid_natchs_sp_port(const DevId& devid, const struct sp_port *psp, enum device_id_style dis)
{
    if (psp == nullptr) return false;
    bool ok;
    uint32_t id;
    int vid, pid;
    switch (dis)
    {
    case dis_urpc:
        id = xibridge_get_urpc_serial_from_sp_port(psp, ok);
        if (!ok) return false;
        return id == devid.id();
    case dis_ximc:
        id = xibridge_get_ximc_serial_from_sp_port(psp, ok);
        if (!ok) return false;
        return id == devid.id();

    case dis_ext_ximc:
        id = xibridge_get_ximc_serial_from_sp_port(psp, ok);
        if (!ok) return false;
        if (sp_get_port_usb_vid_pid(psp, &vid, &pid) != SP_OK) return false;
        return id == devid.id() && (uint16_t)vid == devid.VID() && (uint16_t)pid == devid.PID();
    default:
        return false;
    }
}

/*
* gets port name which could be opened by system from devid
*/
const std::string xibridge_port_name_by_devid(const DevId& devid, enum device_id_style dis)
{
    switch (dis)
    {
    case dis_urpc:
        return serial_to_address(devid.id());
    case dis_ximc:
    case dis_ext_ximc:
        if (pport_list == nullptr) return "";
        for (int i = 0; pport_list[i] != NULL; i++)
        {
            if (xibridge_is_devid_natchs_sp_port(devid, pport_list[i], dis))
                return sp_get_port_name(pport_list[i]);
        }
        return "";
     default:
        return "";
    }
}

std::vector<DevId> xibridge_enumerate_dev(enum device_id_style dis)
{
    std::vector<DevId> devids;
    
    for (int i = 0; pport_list[i] != NULL; i++) 
    {
        struct sp_port *pport = pport_list[i];
        int vid = 0;
        int pid = 0;
        uint32_t id = 0;
        uint32_t reserved = 0;
        bool ok;
        switch (dis)
        {
        case dis_urpc:
            id = xibridge_get_urpc_serial_from_sp_port(pport, ok);
            if (!ok) continue;
            break;
        case dis_ext_ximc:
            if (sp_get_port_usb_vid_pid(pport, &vid, &pid) != SP_OK) continue;
        case dis_ximc:
            id = xibridge_get_ximc_serial_from_sp_port(pport, ok);
            if (!ok) continue;
        default:
            ZF_LOGD("Unknown device id progile!");
            continue;
        }
        devids.push_back(DevId((uint32_t)id, (uint16_t)pid, (uint16_t)vid, (uint32_t)reserved));
    }
    return devids;
}

std::map<DevId, std::mutex *> MapDevIdPHandle::_mutex_pool;
ReadWriteLock MapDevIdPHandle::_mutex_pool_mutex;

xib_device_handle_t XibDevicePHandle::create_device_h(const DevId& devid)
{
    const std::string addr = xibridge_port_name_by_devid(devid, MapDevIdPHandle::get_device_id_style());
        
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
        xib_com_device_destroy(&_uhandle);
        _uhandle = nullptr;
    }
}

void MapDevIdPHandle::free_mutex_pool()
{
    // some strange iterator behavior when it's container is empty
    if (_mutex_pool.size() == 0) return;
    std::map<DevId, std::mutex *>::const_iterator mpli = _mutex_pool.cbegin();
 
    for (; mpli != _mutex_pool.cend(); mpli++)
    {
        delete mpli -> second;
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
   free_mutex_pool();
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
    std::mutex * pm = nullptr;
    _mutex_pool_mutex.read_lock();
    if (_mutex_pool.find(devid) == _mutex_pool.cend())
    {
        _mutex_pool_mutex.read_unlock();
        pm = new std::mutex();
        _mutex_pool_mutex.write_lock();
        _mutex_pool.insert(std::make_pair(devid, pm));
        _mutex_pool_mutex.write_unlock();
    }
    else
    {

        pm = _mutex_pool[devid];
        _mutex_pool_mutex.read_unlock();
    }
    pm->lock();
}

void MapDevIdPHandle::unlock_device_mutex(const DevId &devid)
{
    std::mutex * pm = nullptr;
    _mutex_pool_mutex.read_lock();
    bool ret = false;
    if (_mutex_pool.find(devid) == _mutex_pool.cend())
    {
         ret = true;
    }
    _mutex_pool_mutex.read_unlock();
    if (ret) return;
    _mutex_pool[devid]->unlock();
}

bool MapDevIdPHandle::_is_actual_connection(conn_id_t conn_id, DevId *pserial)
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
    if (force_remove == true || _is_actual_connection(conn_id, &devid_real)) return;
    if (devid_known.id() == UINT32_MAX) devid_known = devid_real;
    if (devid_known.id() == UINT32_MAX) return;
    lock_create_device_mutex(devid_real);
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
