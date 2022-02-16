#include <bindy/bindy-static.h>
#include "../Common/defs.h"
#include "xibridge_client.h"
#include "../Common/Protocols.h"
#include "bindy_helper.h" 


bool Xibridge_client::xibridge_init(const char *key_file_path)
{
	Bindy_helper::set_keyfile(key_file_path);
	return Bindy_helper::instance_bindy() != nullptr;
}


void Xibridge_client::xibridge_shutdown()
{
	Bindy_helper::shutdown_bindy();
}


uint32 Xibridge_client::xibridge_recognize_proto_version()
{

	return 0;
}

std::vector<uint32> Xibridge_client::enumerate_dev_numbers(uint8 *extra_enum_data, uint32& extta_size)
{
	return std::vector<uint32>();
}


Xibridge_client::Xibridge_client(const char *addr, uint32 serial, uint32 version, uint32 send_tmout, uint32 recv_tmout) :
_server_protocol_version(version),
_dev_num(serial),
_last_client_error(0),
_last_server_error(0),
_send_tmout(send_tmout),
_recv_tmout(recv_tmout),
_conn_id(conn_id_invalid)
{
   //bindy will be used to create  _bindy = new Bindy(key_file_path, false, false) // some init actions // call_back - to resv messages//
  // client 
   memset(_host, 0, MAXHOST + 1);
   int len = strlen(addr);
   memcpy(_host, addr, len > MAXHOST ? MAXHOST : len);
}

bool Xibridge_client::_send_and_receive(bvector &req)
{
	// send 
	bool is_ok = Bindy_helper::instance() -> send_bindy_data(_conn_id, req);
	if (!is_ok)
	{
		_last_client_error = ERR_SEND_DATA;
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
		_last_server_error = ERR_NO_ANSWER;
		return FALSE;
	}

	return TRUE;

}

bool Xibridge_client::open_connection_device()
{
	auto conn = Bindy_helper::instance() -> connect(_host, this);
	if (conn == conn_id_invalid)
	{
	   _last_client_error = ERR_NO_CONNECTION;
	   return false;
	}

	AProtocol *proto = create_appropriate_protocol(_server_protocol_version);

	if (proto == nullptr) 
	{
		_last_client_error = ERR_NO_PROTOCOL;
		return false;
	}

	bvector req = proto->create_open_request(_dev_num, _recv_tmout);

	if (_send_and_receive(req))
	{
		bvector green, grey;
		uint32 pckt, serial;
		MBuf buf(_recv_message.data(), _recv_message.size());
		if (proto -> get_data_from_bindy_callback(buf, green, grey, pckt, serial) == false)
		{
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}

}