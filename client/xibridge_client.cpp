﻿#include <bindy/bindy-static.h>
#include <zf_log.h>
#include "../common/defs.h"
#include "xibridge_client.h" 
#include "../common/protocols.h"
#include "bindy_helper.h" 
#include "../common/xibridge_uri_parse.h"

typedef struct
{
    uint32_t code;
    const char *text;
 
} err_def_t;

static err_def_t _err_strings[] =
{ 
    { ERR_NO_PROTOCOL, "Protocol is either undefined or current operation is not available." },
    { ERR_NO_CONNECTION, "Connection is not created or broken." },
    { ERR_SEND_TIMEOUT, "Send data timeout." },
    { ERR_NO_BINDY, "Network component (xibridge-bindy) has not been initialized properly." },
    { ERR_SEND_DATA, "Send data error."},
    { ERR_NULLPTR_PARAM, "Null pointer output parameter." },
    { ERR_SET_CONNECTION, "Network connection setting (bindy) error."},
    { ERR_RECV_TIMEOUT, "Receive data timeout." },
    { ERR_ANOTHER_PROTOCOL, "Another protocol is to be tested [internal flag]." },
    { ERR_DEVICE_LOST, "Server tells that the device connected is lost." },
    { ERR_DEVICE_OPEN, "Server tells that the device cannot be opened." },
    { ERR_DEVICE_ERR, "Server tells that the device does not understand the command." },
    { ERR_DEVICE_ERR_VAL, "Server tells that the device does not understand some command parameter(s)." },
    { ERR_PCKT_FMT, "Invalid data format." },
    { ERR_PCKT_INV, "Invalid data packet." },
    { 0, "" }
 }; 

// to make log level controlled
ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

//to set final log level
void _log_level()
{
#ifdef _DEBUG
    zf_log_set_output_level(ZF_LOG_DEBUG);
#else
    zf_log_set_output_level(ZF_LOG_WARN);
#endif
  
}

uint32_t Xibridge_client::_server_base_protocol_version = 3;

// this simple class to make some base init actions - cross platform
class xbc_init
{
private:
    xbc_init() { _log_level(); Xibridge_client::xbc_set_base_protocol_version({ 1, 0, 0 }); };
    static xbc_init _xbc_i;
};

xbc_init xbc_init::_xbc_i;

static bool is_protocol_supported_version(const xibridge_version_t& ver)
{
    return ver.major > 0 && ver.major <= DEFAULT_PROTO_VERSION && ver.minor == 0 && ver.bagfix == 0;
}

const char *Xibridge_client::xbc_get_err_expl(uint32_t err_no)
{
    for (int i = 0; _err_strings[i].code != 0; i++)
    {
        if (_err_strings[i].code == err_no) return _err_strings[i].text;
    }
    return nullptr;
}

Xibridge_client * Xibridge_client::_get_client_as_free(conn_id_t conn_id)
{
    std::unique_lock<std::mutex> lock(Bindy_helper::_map_mutex);
    if (Bindy_helper::_map.find((conn_id_t)conn_id) == Bindy_helper::_map.cend())
    {
        return nullptr;
    }
        
    return Bindy_helper::_map.at((conn_id_t)conn_id);
}

uint32_t Xibridge_client::xbc_set_base_protocol_version(xibridge_version_t ver)
{
    if (!is_protocol_supported_version(ver)) return ERR_NO_PROTOCOL;
    _server_base_protocol_version = (uint32_t)ver.major;
    return 0;
}

xibridge_version_t Xibridge_client::xbc_get_connection_protocol_version(const xibridge_conn_t *pconn)
{
    if (pconn == nullptr) 
    {
        return xibridge_version_invalid;
    };
    Xibridge_client * cl = _get_client_as_free(pconn->conn_id);
    return {cl == nullptr ? (uint8_t)_server_base_protocol_version : (uint8_t)cl->_server_protocol_version, 0, 0};
}

bool Xibridge_client::exec_version_request(xibridge_version_t *pversion)
{
    clr_errors();
    if (pversion == nullptr)
    {
        _last_error = ERR_NULLPTR_PARAM;
        return false;
    }

    _server_protocol_version = xbc_get_last_protocol_version().major;
    AProtocol *proto = create_appropriate_protocol(_server_protocol_version, &_last_error);
    if (proto == nullptr)
    {
        _last_error = ERR_NO_PROTOCOL;
        return false;
    }
    bvector req = proto->create_version_request(_recv_tmout);

    if (req.size() == 0)
    {
        _last_error = ERR_NO_PROTOCOL;
        return false;
    }
    if (_send_and_receive(req))
    {
        bvector res_data, data;
        uint32_t pckt;
        _mutex_recv.lock();
        MBuf buf(_recv_message.data(), (int)_recv_message.size());
        _mutex_recv.unlock();

        if (proto->get_data_from_bindy_callback(buf, res_data, data, pckt) == false)
        {
            _last_error = ERR_NO_PROTOCOL;
            return false;
        }
      
        pversion->major = proto->get_result_error();
        pversion->minor = 0;
        pversion->bagfix = 0;
        return true;
    }
    else
    {
        return false;
    }
}

bool Xibridge_client::exec_enumerate(char **result, 
                                     uint32_t *pcount)
{
    clr_errors();

    if (result != nullptr) *result = nullptr;
    if (pcount != nullptr) *pcount = 0;
    if (result == nullptr || pcount == nullptr)
    {
        _last_error = ERR_NULLPTR_PARAM;
        return false;
    }
   
    uint32_t answer_proto_version = _server_protocol_version;

    AProtocol *proto = create_appropriate_protocol(_server_protocol_version, &_last_error);
    if (proto == nullptr)
    {
        _last_error = ERR_NO_PROTOCOL;
        return false;
    }
    bvector req = proto -> create_enum_request(_recv_tmout);
    if (req.size() == 0)
    {
        _last_error = ERR_NO_PROTOCOL;
        return false;
    }
    if (_send_and_receive(req))
    {
        bvector res_data, data;
        uint32_t pckt;
        _mutex_recv.lock();
        MBuf buf(_recv_message.data(), (int)_recv_message.size());
        _mutex_recv.unlock();

        answer_proto_version = get_proto_version_of_the_recv_message();
        if (answer_proto_version != _server_protocol_version)
        {
            _server_protocol_version = answer_proto_version;
            _last_error = ERR_ANOTHER_PROTOCOL;
            return false;
        }
        if (proto->get_data_from_bindy_callback(buf, res_data, data, pckt) == false)
        {
            return false;
        }
        int num_devs = proto->get_result_error();
        if (num_devs != 0)
        {
            MBuf res(((int)(strlen(_host) + sizeof(xibridge_device_t) * 2 + 16)) * num_devs); // xi-net://<>/<>
            xi_net_dev_uris(res, _host, data, num_devs);
            *result = (char *)malloc(res.realSize());
            memcpy(*result, (const uint8_t *)res, res.realSize());
        }
  
       *pcount = num_devs;
        return true;
    }
    else
    {
        return false;
    }
}

Xibridge_client::Xibridge_client(const char *xi_net_uri, 
                                 const char *adapter):
_server_protocol_version(_server_base_protocol_version),
_conn_id(conn_id_invalid),
_send_tmout((uint32_t)TIMEOUT),
_recv_tmout((uint32_t)TIMEOUT)
{
    memset(_host, 0, XI_URI_HOST_LEN + 1);
    memset(_adapter, 0, XI_URI_HOST_LEN + 1);
    
    if (adapter == NULL)
    {

        xibridge_parsed_uri parsed;

        if (xibridge_parse_uri_dev12(xi_net_uri, &parsed) == 0)
        {
             memcpy(_host, parsed.uri_server_host, XI_URI_HOST_LEN);
             _dev_id = parsed.uri_device_id;
        }
    }
    else
    {
        // just connection needed to make some enumerate
        memcpy(_host, xi_net_uri, strlen(xi_net_uri));
        memcpy(_adapter, adapter, strlen(adapter));
    }
    clr_errors();
}

void Xibridge_client::_set_last_error(uint32_t err, 
                                      const char *add_text)
{
    _last_error = err;
    const char *stand_text = xbc_get_err_expl(err);
    if (stand_text == nullptr || add_text == nullptr) return;
    if (strstr(stand_text, "%s") != nullptr)
    {
        _complex_error = add_text;
    }
}

uint32_t Xibridge_client::xbc_read_connection_buffer(const xibridge_conn_t *pconn, 
                                                     uint8_t *buf, 
                                                     uint32_t size, 
                                                     uint32_t* preal_read)
{
    if (pconn == nullptr)
    {
        return ERR_NULLPTR_PARAM;
    }

    Xibridge_client *pcl = _get_client_as_free(pconn -> conn_id);
    if (pcl == nullptr)
    {   
        return ERR_NO_CONNECTION;
    }
    pcl -> clr_errors();
    pcl -> _mutex_recv.lock();
    uint32_t real_size = (uint32_t)pcl->_recv_message.size();
    bool clear = false;
    if (real_size <= size)
    {
         size = real_size;
         clear = true;
    }
    memcpy(buf, pcl->_recv_message.data(), size);
    if (clear) pcl->_recv_message.clear();
    else pcl->_recv_message.assign(pcl->_recv_message.cbegin() + size, pcl->_recv_message.cend());
    pcl->_mutex_recv.unlock();
    if (preal_read != nullptr) *preal_read = size;
    return pcl->get_last_error();
}

uint32_t Xibridge_client::xbc_write_connection(const xibridge_conn_t *pconn,
                                               const uint8_t *buf, 
                                               uint32_t size)
{
    if (pconn == nullptr)
    {
        return ERR_NULLPTR_PARAM;
    }

    Xibridge_client *pcl = _get_client_as_free(pconn->conn_id);
    if (pcl == nullptr)
    {
        return ERR_NO_CONNECTION;
    }
    pcl->clr_errors();
    bvector d; 
    d.assign(buf, buf + size);
    Bindy_helper::instance()->send_bindy_data(pconn -> conn_id, d);
    return pcl ->get_last_error();
}

bool Xibridge_client::_send_and_receive(bvector &req)
{
    // send 
    bool is_ok = Bindy_helper::instance() -> send_bindy_data(_conn_id, req);
    if (!is_ok)
    {
        _last_error = ERR_SEND_DATA;
        return false;
    }

    // receive 
    _is_really_recv = false;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    while (!_is_really_recv)
    {
        if (std::cv_status::timeout == _is_recv.wait_for(lock, std::chrono::milliseconds(_recv_tmout)))
            break;
    }

    if (_is_really_recv)
        return true; 
    else
    {
        _last_error = ERR_RECV_TIMEOUT;
        return false;
    }
    return true;
}

bool Xibridge_client::is_connected()
{
    return _conn_id != conn_id_invalid && 
           Bindy_helper::instance()->is_connected(_conn_id);
}

bool Xibridge_client::decrement_server_protocol_version()
{
    if (_server_protocol_version > 1)
    {
        _server_protocol_version--;
    }
    else
    {
        _server_protocol_version = xibridge_get_last_protocol_version().major;
    }
    return false;
}

bool Xibridge_client::open_connection()
{
    clr_errors();
    _conn_id = Bindy_helper::instance() -> connect(this);
    if (_conn_id == conn_id_invalid)
    {
        _last_error = ERR_NO_CONNECTION;
        return false;
    }
    return true;
}

bool Xibridge_client::open_device()
{
    clr_errors();
    uint32_t answer_version = _server_protocol_version;
    AProtocol *proto = create_appropriate_protocol(_server_protocol_version, &_last_error);
    if (proto == nullptr) 
    {
        _last_error = ERR_NO_PROTOCOL;
        return false;
    }
    bvector req = proto->create_open_request(_dev_id, _recv_tmout);
    if (req.size() == 0)
    {
        _last_error = ERR_NO_PROTOCOL;
        return false;
    }
    if (_send_and_receive(req))
    {
        bvector res_data, data;
        uint32_t pckt; 
        _mutex_recv.lock();
        MBuf buf(_recv_message.data(), (int)_recv_message.size());
        _mutex_recv.unlock();
        
        answer_version = get_proto_version_of_the_recv_message();
        if (answer_version != _server_protocol_version)
        {
            _server_protocol_version = answer_version;
            _last_error = ERR_ANOTHER_PROTOCOL;
            return false;
        }
        if (proto -> get_data_from_bindy_callback(buf, res_data, data, pckt) == false)
        {
            return false;
        }
        if (!proto->translate_response(pckt, res_data))
        {
            _last_error = ERR_DEVICE_OPEN;
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

bvector Xibridge_client::send_data_and_receive(bvector data, 
                                               uint32_t resp_length)
{
    clr_errors();

    if (!is_connected())
    {
        _last_error = ERR_NO_CONNECTION;
        return bvector();
    }

    AProtocol *proto = create_appropriate_protocol(_server_protocol_version, &_last_error);

    if (proto == nullptr)
    {
        _last_error = ERR_NO_PROTOCOL;
        return bvector();
    }

    bvector req = proto->create_cmd_request(_dev_id, _recv_tmout, &data, resp_length);
    if (req.size() == 0)
    {
        _last_error = ERR_NO_PROTOCOL;
        return bvector();
    }
    if (_send_and_receive(req))
    {
        bvector res_data, data;
        uint32_t pckt; 
        MBuf buf(_recv_message.data(), (int)_recv_message.size());
        if (proto->get_data_from_bindy_callback(buf, res_data, data, pckt) == false)
        {
            return bvector();
        }
        return data;
    }
    else
    {
        return bvector();
    }
}

uint32_t Xibridge_client::xbc_request_response(const xibridge_conn_t *pconn, 
                                               const unsigned char *req, 
                                               uint32_t req_len, 
                                               unsigned char *resp, 
                                               uint32_t resp_len)
{
    if (pconn == nullptr)
    {
        return  ERR_NULLPTR_PARAM;
    }

    Xibridge_client * pcl = _get_client_as_free(pconn->conn_id);
    if (pcl == nullptr) return ERR_NO_CONNECTION;
    MBuf send_data((uint8_t *)req, req_len);

    bvector response = pcl -> send_data_and_receive(send_data.to_vector(), resp_len);
    // do not check real resp length because various reponses could be received or nit - error, the situation of timeout and so on

    // but we restrict the size of vector 
    if (resp != nullptr && resp_len != 0)
    {
        memcpy(resp, response.data(), response.size() < resp_len ? response.size() : resp_len );
    }
    return pcl->get_last_error();
}

bool Xibridge_client::close_connection_device()
{
    clr_errors();

    if (!is_connected())
    {
        _last_error = ERR_NO_CONNECTION;
        return false;
    }
    AProtocol *proto = create_appropriate_protocol(_server_protocol_version, &_last_error);
    if (proto == nullptr)
    {
        _last_error = ERR_NO_PROTOCOL;
        return false;
    }
    bvector req = proto -> create_close_request(_dev_id, _recv_tmout);
    if (req.size() == 0)
    {
        _last_error = ERR_NO_PROTOCOL;
        return false;
    }
    bool ret;
    if (_send_and_receive(req))
    {
        bvector res_data, data;
        uint32_t pckt; 
        MBuf buf(_recv_message.data(), (int)_recv_message.size());
        if (proto->get_data_from_bindy_callback(buf, res_data, data, pckt) == false)
        {
            ret = false;
        }
        ret = proto->translate_response(pckt, res_data);
    }
    else
    {
        ret = false;
    }
    disconnect();
    return ret;
    
}

uint32_t Xibridge_client::xbc_close_connection_device(const xibridge_conn_t *pconn)
{
    if (pconn == nullptr)
    {
        return ERR_NULLPTR_PARAM;
    }

    Xibridge_client *pcl = _get_client_as_free(pconn -> conn_id);
    if (pcl != nullptr)
    {
        pcl->close_connection_device();
        delete pcl;
    }
    return 0;
} 

void Xibridge_client::disconnect()
{
    Bindy_helper::instance() -> disconnect(_conn_id);
    _conn_id = conn_id_invalid;
}

