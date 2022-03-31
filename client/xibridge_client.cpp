#include <bindy/bindy-static.h>
#include "../Common/defs.h"
#include "xibridge_client.h" 
#include "../Common/Protocols.h"
#include "bindy_helper.h" 

static const char * _err_strings[] =
{ /*ERR_NO_PROTOCOL*/ "Protocol is undefined: supported versions are 1,2,3.|Протокол не определен: доступны версии 1, 2 или 3.", 
  /*ERR_NO_CONNECTION*/ "Connection is not created or broken.|Соединение не установлено или разорвано."
  /*ERR_SEND_TIMEOUT*/ "Send data timeout.|Таймаут отправки данных.",
  /*ERR_NO_BINDY */    "Network component (bindy) is not initialized properly.|Сетeвой компонент (bindy) не был инициализирован."
  /* ERR_SEND_DATA */ "Send data error.| Ошибка отправки данных.",
  /* ERR_RECV_TIMEOUT */ "Receive data timeout.|Таймаут получения данных."
}; 

void Xibridge_client::xibridge_get_err_expl(char * s, int len, bool is_russian, unsigned int err_no)
{
	const char *unkn_error = is_russian ? "Фатально: неизвестная ошибка!" : "Fatal: unknown error!";
	memset(s, 0, len);
	if ((int)err_no >= sizeof(_err_strings))
	{
		strncpy(s, unkn_error, len);
	}
	else if (err_no > 0)
	{
		const char *err_str = _err_strings[(int)err_no - 1];
		const char *delim = strchr(err_str, '|');
		if (delim != nullptr)
			memcpy(s, is_russian ? delim + 1 : err_str, (is_russian ? strchr(err_str, '0') : delim) -
			(is_russian ? delim + 1 : err_str));
	}
}

void Xibridge_client::get_error_expl(char * s, int len, bool is_russian) const
{
	return xibridge_get_err_expl(s, len, is_russian, (unsigned int)_last_error);
}

Xibridge_client * Xibridge_client::_get_client_as_free(unsigned int conn_id)
{
	std::unique_lock<std::mutex> lock(Bindy_helper::_map_mutex);
	if (Bindy_helper::_map.find((conn_id_t)conn_id) == Bindy_helper::_map.cend())
	{
		return nullptr;
	}
		
	return Bindy_helper::_map.at((conn_id_t)conn_id);
}

bool Xibridge_client::xibridge_init(const char *key_file_path)
{
	bindy::Bindy *_ex = nullptr;
	Bindy_helper::_global_mutex.lock();
	Bindy_helper::set_keyfile(key_file_path);
	_ex = Bindy_helper::instance_bindy();
	Bindy_helper::_global_mutex.unlock();
	return _ex != nullptr;
}

void Xibridge_client::xibridge_shutdown()
{
	Bindy_helper::_global_mutex.lock();
	Bindy_helper::shutdown_bindy();
	Bindy_helper::_global_mutex.unlock();
}

void Xibridge_client::xibridge_set_server_protocol(unsigned int conn_id, unsigned int proto)
{
	Xibridge_client * cl = _get_client_as_free(conn_id);
	if (cl != nullptr)cl -> set_server_protocol_version(proto);
}

unsigned int Xibridge_client::xibridge_get_server_protocol(unsigned int conn_id)
{
	Xibridge_client * cl = _get_client_as_free(conn_id);
	return cl == nullptr ? 0 : (unsigned int) cl -> get_server_protocol_version();
}


bool Xibridge_client::xibridge_enumerate_adapter_devices(const char *addr, const char *adapter,
	                                              unsigned char *result,
	                                              unsigned int *pcount, unsigned int timeout,
	                                              unsigned int* last_errno)
{
	/* *
	* Create temporary client  with Protocol 2  and some serial 2 - to be a static func
	*/
	*pcount = 0;
	Xibridge_client * xl = new Xibridge_client(addr, 3, 1, 0, timeout);
	auto conn = Bindy_helper::instance()->connect(addr, xl);
	if (conn == conn_id_invalid)
	{
		if (last_errno != nullptr) *last_errno = ERR_NO_CONNECTION;
		return false;
	}

	AProtocol *protocol = create_appropriate_protocol(1);  // Protocol 1 only
	bvector req = protocol -> create_enum_request(xl -> get_resv_tmout());
	if (req.size() == 0)
	{
		if (last_errno != nullptr) *last_errno = ERR_NO_PROTOCOL;
		return false;
	}

	xl->_conn_id = conn;
	bool ret = true;
	uint32 version = 0;
	if (xl -> _send_and_receive(req))
	{
		bvector green, grey;
        uint32 pckt; DevId devid;
		xl -> _mutex_recv.lock();
		MBuf buf(xl -> _recv_message.data(), (int)xl -> _recv_message.size());
		xl->_mutex_recv.unlock();

		if (protocol -> get_data_from_bindy_callback(buf, green, grey, pckt, devid) == true)
		{
			result = (unsigned char *) malloc(grey.size());
			memcpy(result, grey.data(), grey.size());
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

Xibridge_client::Xibridge_client(const char *addr, unsigned int serial, 
	                                               unsigned int proto_version, 
												   unsigned int send_tmout, 
												   unsigned int recv_tmout) :
_server_protocol_version((uint32)proto_version),
_dev_num((uint32)serial),
_last_error(0),
_send_tmout((uint32)send_tmout),
_recv_tmout((uint32)recv_tmout),
_conn_id(conn_id_invalid)
{
  // bindy will be used to create  _bindy = new Bindy(key_file_path, false, false) // some init actions // call_back - to resv messages//
  // client 
   memset(_host, 0, MAXHOST + 1);
   int len = (int)strlen(addr);
   memcpy(_host, addr, len > MAXHOST ? MAXHOST : len);
}

int Xibridge_client::xibridge_read_connection_buffer(unsigned int conn_id,
                                                     unsigned char *buf, int size)
{
	Xibridge_client *pcl = _get_client_as_free(conn_id);
	if (pcl == nullptr) return 0;
	pcl -> _mutex_recv.lock();
	int real_size = pcl->_recv_message.size();
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

int Xibridge_client::xibridge_write_connection(unsigned int conn_id,
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
	bvector req = proto->create_open_request(_dev_num, _recv_tmout);
	if (req.size() == 0)
	{
		_last_error = ERR_NO_PROTOCOL;
		return false;
	}
	_conn_id = conn;
	if (_send_and_receive(req))
	{
		bvector green, grey;
        uint32 pckt; DevId devid;
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

bvector Xibridge_client::send_data_and_receive(bvector data, uint32 resp_length, uint32 & res_err)
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

	bvector req = proto->create_cmd_request(_dev_num, _recv_tmout, &data, resp_length);
	if (req.size() == 0)
	{
		_last_error = ERR_NO_PROTOCOL;
		return bvector();
	}
	if (_send_and_receive(req))
	{
		bvector green, grey;
        uint32 pckt; DevId  serial;
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

bool Xibridge_client::xibridge_request_response(unsigned int conn_id, 
	                                            const unsigned char *req, 
												int req_len, 
												unsigned char *resp, 
												int resp_len, unsigned int *res_err)
{
	if ((conn_id_t)conn_id == conn_id_invalid) return false;
	
	Xibridge_client *pcl = _get_client_as_free((conn_id_t)conn_id);
	if (pcl == nullptr) return false;
    MBuf send_data((uint8 *)req, req_len);

	bvector response = pcl -> send_data_and_receive(send_data.to_vector(), resp_len, *res_err);
	// do not check real resp length because various reponses could be received or nit - error, the situation of timeout and so on

	// but we restrict the size of vector 
	if (resp != nullptr && resp_len != 0)
	{
		memcpy(resp, response.data(), response.size() < (unsigned int)resp_len ? response.size() : resp_len );
	}
	return pcl->get_last_error() == 0;
}

unsigned int Xibridge_client::xibridge_get_last_err_no(unsigned int conn_id)
{
	Xibridge_client *pcl = _get_client_as_free((conn_id_t)conn_id);

	return pcl == nullptr ? 0 : (unsigned int)pcl -> get_last_error();
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

	bvector req = proto->create_close_request(_dev_num, _recv_tmout);
	if (req.size() == 0)
	{
		_last_error = ERR_NO_PROTOCOL;
		return false;
	}
	bool ret;
	if (_send_and_receive(req))
	{
		bvector green, grey;
        uint32 pckt; DevId devid;
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

void Xibridge_client::xibridge_close_connection_device(unsigned int conn_id)
{
	if ((conn_id_t)conn_id == conn_id_invalid) return;
	bool non_connected;
	Xibridge_client *pcl;
	Bindy_helper::_map_mutex.lock();
	non_connected = Bindy_helper::_map.find((conn_id_t)conn_id) == Bindy_helper::_map.cend();
	if (!non_connected) pcl = Bindy_helper::_map.at((conn_id_t)conn_id);;
	Bindy_helper::_map_mutex.unlock();
	pcl -> close_connection_device();
	delete pcl;
} 

void Xibridge_client::disconnect()
{
	Bindy_helper::instance() -> disconnect(_conn_id);
	_conn_id = conn_id_invalid;
}

uint32  Xibridge_client::xibridge_detect_protocol_version(const char *addr, uint32 send_timeout, uint32 recv_timeout)
{
	/* *
	* Create temporary client  with Protocol 2  and some serial 2 - to be a static func
	*/

	Xibridge_client * xl = new Xibridge_client(addr, 3, 2, send_timeout, recv_timeout);
	auto conn = Bindy_helper::instance()->connect(addr, xl);
	if (conn == conn_id_invalid)
	{
		return 0;
	}

	AProtocol *proto = create_appropriate_protocol(2);  // Protocol 2
	bvector req = proto->create_open_request(xl->get_dev_num(), xl->get_resv_tmout());
	if (req.size() == 0)
	{
		return 0;
	}

	xl -> _conn_id = conn;

	uint32 version = 0;
	if (xl -> _send_and_receive(req))
	{
		version = xl -> get_proto_version_of_the_recv_message();
		if (version == 2) // resolved 
		{
			// check if there is a xibridge server
			//xl -> set_server_protocol_version(3);
			// send special version command
			bvector req = create_appropriate_protocol(3)->create_version_request(xl->get_resv_tmout());
			if (req.size() == 0)
			{
				return 0;
			}
			if (xl->_send_and_receive(req))
			{
				if (xl->get_proto_version_of_the_recv_message() == 3)
					version = 3; // resolved to unknown proto version
			}
			//
			xl -> close_connection_device(); // if any device really opened
			delete xl;
		}
		
	}
	return version;
}