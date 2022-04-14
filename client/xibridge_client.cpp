#include <bindy/bindy-static.h>f
#include "../Common/defs.h"
#include "xibridge_client.h" 
#include "../Common/Protocols.h"
#include "bindy_helper.h" 

typedef struct
{
    int code;
    const char *text;
 
} err_def_t;

err_def_t _err_strings[] =
{ 
    { ERR_NO_PROTOCOL, "Protocol is undefined: supported versions are 1, 2, 3." },
    { ERR_NO_CONNECTION, "Connection is not created or broken." },
    { ERR_SEND_TIMEOUT, "Send data timeout." },
    { ERR_NO_BINDY, "Network component (bindy) is not initialized properly." },
    { ERR_SEND_DATA, "Send data error."},
    { ERR_RECV_TIMEOUT, "Receive data timeout." },
    { ERR_KEYFILE_NOT_REPLACED, "Xibridge initialized OK, but key file cannot be replaced." },
	{ 0, "" }
 }; 

static bool is_protocol_verified_version_t(const xibridge_version_t& ver)
{
    return ver.major > 0 && ver.major <= DEFAULT_PROTO_VERSION && ver.minor == 0 && ver.bagfix == 0;
}

const char *Xibridge_client::xi_get_err_expl(uint32_t err_no)
{
	/*
	for (int i = 0; _err_strings[i].code != 0; i++)
    {
        if (_err_strings[i].code == err_no) return _err_strings[i].text;
    }
	*/
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

uint32_t Xibridge_client::xi_init(const char *key_file_path)
{
    uint32_t ret_err = 0;
	Bindy_helper::_global_mutex.lock();
	if (!Bindy_helper::set_keyfile(key_file_path))
        ret_err = ERR_KEYFILE_NOT_REPLACED;
    bindy::Bindy *_ex = Bindy_helper::instance_bindy();
	Bindy_helper::_global_mutex.unlock();
    if (_ex == nullptr)
        ret_err = ERR_NO_BINDY;
	return ret_err;
}

uint32_t  Xibridge_client::xi_set_connection_protocol_version(xibridge_conn_t conn, xibridge_version_t ver)
{
	Xibridge_client * cl = _get_client_as_free(conn.conn_id);
    if (cl == nullptr) return ERR_NO_CONNECTION;
    if (!is_protocol_verified_version_t(ver)) return ERR_NO_PROTOCOL;
	cl -> _server_protocol_version = ver.major;
}

xibridge_version_t Xibridge_client::xi_get_connection_protocol_version(xibridge_conn_t conn)
{
	Xibridge_client * cl = _get_client_as_free(conn.conn_id);
    return {cl == nullptr ? DEFAULT_PROTO_VERSION : (uint8_t)cl->_server_protocol_version, 0, 0};
}


uint32_t Xibridge_client::xi_enumerate_adapter_devices(const char *addr, const char *adapter,
	                                              char **result,
	                                              uint32_t *pcount, uint32_t timeout
	                                           )
{
	/* *
	* Create temporary client  with Protocol 2  and some serial 2 - to be a static func
	*/
	*pcount = 0;
    *result = nullptr;
	Xibridge_client * xl = new Xibridge_client(addr, 0, timeout);
	auto conn = Bindy_helper::instance()->connect(addr, xl);
	if (conn == conn_id_invalid)
	{
		uint32_t err = xl->get_last_error();
		delete xl;
		return err;
	}
	// to do !!!
	AProtocol *protocol = create_appropriate_protocol(/*xl -> get_server_version().major*/1);  
	bvector req = protocol -> create_enum_request(xl -> get_resv_tmout());
	if (req.size() == 0)
	{
		xl->close_connection_device(); 
		delete xl;
		return ERR_NO_PROTOCOL;
	}

	xl -> _conn = conn;
	bool ret = true;
	uint32_t version = 0;
	if (xl -> _send_and_receive(req))
	{
		bvector green, grey;
        uint32_t pckt; DevId devid;
		xl -> _mutex_recv.lock();
		MBuf buf(xl -> _recv_message.data(), (int)xl -> _recv_message.size());
		xl->_mutex_recv.unlock();

		if (protocol -> get_data_from_bindy_callback(buf, green, grey, pckt, devid) == true)
		{
			*result = (unsigned char *) malloc(grey.size());
			memcpy(*result, grey.data(), grey.size());
			*pcount = protocol -> get_result_error();
			//
		}
		else
		{
			if (last_errno != nullptr)
				*last_errno = ERR_NO_PROTOCOL;
			ret = false;
		}
	}
	else
	{
		if (last_errno != nullptr)
			*last_errno = ERR_RECV_TIMEOUT;
		ret = false;
	}

	xl->close_connection_device(); // if any device really opened
	delete xl;
	return ret;
}

#include "../common/xibridge_uri_parse.h"

Xibridge_client::Xibridge_client(const char *xi_net_uri, unsigned int send_timeout, unsigned int recv_timeout) :
_server_protocol_version(DEFAULT_PROTO_VERSION),
_last_error(0),
_send_tmout((uint32_t)send_timeout),
_recv_tmout((uint32_t)recv_timeout),
_conn_id(conn_id_invalid)
{
  // bindy will be used to create  _bindy = new Bindy(key_file_path, false, false) // some init actions // call_back - to resv messages//
  // client 
   memset(_host, 0, XI_URI_HOST_LEN + 1);
   
   xibridge_parsed_uri parsed;

   if (xibridge_parse_uri(xi_net_uri, &parsed) == 0)
   {
	   memcpy(_host, parsed.uri_server_host, XI_URI_HOST_LEN);
	   _dev_id = parsed.uri_device_id;
   }
}

int Xibridge_client::xi_read_connection_buffer(uint32_t conn_id,
                                                     unsigned char *buf, int size)
{
	Xibridge_client *pcl = _get_client_as_free(conn_id);
	if (pcl == nullptr) return 0;
	pcl -> _mutex_recv.lock();
	int real_size = (int)pcl->_recv_message.size();
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
	return size;
}

int Xibridge_client::xi_write_connection(uint32_t conn_id,
	const unsigned char *buf, int size)
{
	bvector d; 
	d.assign(buf, buf + size);
	return Bindy_helper::instance()->send_bindy_data(conn_id, d) == true ? 1 : 0;
}

bool Xibridge_client::_send_and_receive(bvector &req)
{
	// send 
	bool is_ok = Bindy_helper::instance() -> send_bindy_data(_conn_id, req);
	if (!is_ok)
	{
		_last_error = ERR_SEND_DATA;
		return FALSE;
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
		return FALSE;
	}
	return TRUE;
}

bool Xibridge_client::is_connected()
{
	return  _conn_id != conn_id_invalid && 
		     Bindy_helper::instance()->is_connected(_conn_id);
}

bool Xibridge_client::open_connection_device()
{
	clr_errors();

	if (is_connected()) return true;

	auto conn = Bindy_helper::instance() -> connect(_host, this);
	if (conn == conn_id_invalid)
	{
	   _last_error = ERR_NO_CONNECTION;
	   return false;
	}

	AProtocol *proto = create_appropriate_protocol(_server_protocol_version);

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
	_conn_id = conn;
	if (_send_and_receive(req))
	{
		bvector green, grey;
        uint32_t pckt; DevId devid;
		_mutex_recv.lock();
		MBuf buf(_recv_message.data(), (int)_recv_message.size());
		_mutex_recv.unlock();
		if (proto -> get_data_from_bindy_callback(buf, green, grey, pckt, devid) == false)
		{
			return false;
		}
		return proto -> translate_response(pckt, green);
	}
	else
	{
		return false;
	}
}

bvector Xibridge_client::send_data_and_receive(bvector data, uint32_t resp_length, uint32_t & res_err)
{
	clr_errors();

	if (!is_connected())
	{
		_last_error = ERR_NO_CONNECTION;
		return bvector();
	}

	AProtocol *proto = create_appropriate_protocol(_server_protocol_version);

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
		bvector green, grey;
        uint32_t pckt; DevId  serial;
		MBuf buf(_recv_message.data(), (int)_recv_message.size());
		if (proto->get_data_from_bindy_callback(buf, green, grey, pckt, serial) == false)
		{
			return bvector();
		}
		return grey;
	}
	else
	{
		return bvector();
	}
}

bool Xibridge_client::xi_request_response(xibridge_conn_t conn, 
	                                            const unsigned char *req, 
												int req_len, 
												unsigned char *resp, 
												int resp_len, uint32_t *res_err)
{
	if ((conn_id_t)conn_id == conn_id_invalid) return false;
	
	Xibridge_client *pcl = _get_client_as_free(conn.conn_id);
	if (pcl == nullptr) return false;
    MBuf send_data((uint8_t *)req, req_len);

	bvector response = pcl -> send_data_and_receive(send_data.to_vector(), resp_len, *res_err);
	// do not check real resp length because various reponses could be received or nit - error, the situation of timeout and so on

	// but we restrict the size of vector 
	if (resp != nullptr && resp_len != 0)
	{
		memcpy(resp, response.data(), response.size() < (uint32_t)resp_len ? response.size() : resp_len );
	}
	return pcl->get_last_error() == 0;
}

uint32_t Xibridge_client::xi_get_last_err_no(xibridge_conn_t conn)
{
	Xibridge_client *pcl = _get_client_as_free(conn.conn_id);
	return pcl == nullptr ? 0 : (uint32_t)pcl -> get_last_error();
}


bool Xibridge_client::close_connection_device()
{
	clr_errors();

	if (!is_connected())
	{
		_last_error = ERR_NO_CONNECTION;
		return false;
	}
	AProtocol *proto = create_appropriate_protocol(_server_protocol_version);
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
		bvector green, grey;
        uint32_t pckt; DevId devid;
		MBuf buf(_recv_message.data(), (int)_recv_message.size());
		if (proto->get_data_from_bindy_callback(buf, green, grey, pckt, devid) == false)
		{
			ret = false;
		}
		ret = proto->translate_response(pckt, green);
	}
	else
	{
		ret = false;
	}
	disconnect();
	return ret;
	
}

uint32_t Xibridge_client::xi_close_connection_device(xibridge_conn_t conn)
{
	bool non_connected;
	Xibridge_client *pcl = _get_client_as_free(conn.conn_id);
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

uint32_t  Xibridge_client::_detect_protocol_version()
{
	if (_is_proto_detected) return _server_protocol_version;
	if (!is_connected())
	{
		return DEFAULT_PROTO_VERSION;
	}

	AProtocol *proto = create_appropriate_protocol(_server_protocol_version); 
	bvector req = proto->create_open_request(_dev_id, get_resv_tmout());
	if (req.size() == 0)
	{
		return DEFAULT_PROTO_VERSION;
	}
	
	uint32_t version = 0;
	if (_send_and_receive(req))
	{
		version = get_proto_version_of_the_recv_message();
		if (version == 2) // resolved 
		{
			// check if there is a xibridge server
			//xl -> set_server_protocol_version(3);
			// send special version command
			bvector req = create_appropriate_protocol(3) -> create_version_request(get_resv_tmout());
			if (req.size() == 0)
			{
				return 2;
			}
			if (_send_and_receive(req))
			{
				if (get_proto_version_of_the_recv_message() == 3)
					version = 3; // resolved to unknown proto version
			}
			//
	 }
		
	}
	return version;
}