#ifndef _XIBRIDGE_CLIENT_H
#define  _XIBRIDGE_CLIENT_H

#include "../Common/defs.h"

#include "../Common/Protocols.h"

#include "Bindy_helper.h"

#include <vector>


#define MAXHOST 64 

/*
* Defines client errors (errors to take place at the client side)
*/
#define ERR_NO_PROTOCOL -1
#define ERR_NO_CONNECTION -2
#define ERR_SEND_TIMEOUT  -3
#define ERR_NO_BINDY    -4
#define ERR_SEND_DATA -5


/*
* Defines protocol and server errors
*/

#define ERR_NO_ANSWER -1


/** 
 * Class to communicate with xibridge-server
 */
class Xibridge_client
{
	friend class Bindy_helper;
public:

	/* *
	* Функция запроса версии xibridge
	*/
	static int xibridge_major_version()
	{
		return 1;
	};

	/* *
	* Функция запроса версии протокола
	*/
	static int xibridge_protocol_version()
	{
		return 3;
	}
	
	/* This constructor to create Xibridge_client with defined addr, device number and version number
	 *@param addr - xibridge server address
	 *@param dev_num - device number (COM-port or slot)
	 *@param version - protocol version
	*/
	Xibridge_client(const char *addr, uint32 serial, uint32 protocol_version = 0, uint32 send_tmout = 5000, uint32 recv_tmout = 5000);

	/**
	   * This static member function sets up connecton-specific data
	   * Must be called once per this lib usage 
	   * Multiple clients can be created then
	*/


	void set_protocol_version(uint32 v) { _server_protocol_version = v; };

	/**
	* This static member function to recognize server protocol version
	*/
	static uint32  xibridge_detect_protocol_version(uint32 timeout_1,  uint32 timeout_all);

	/**
	* *This static member function to setup network and bindy - once per application
	*/
	static bool xibridge_init(const char *key_file_path);
	
	/**
	   * This static member function shutdown network and bindy - once per application
	   * Must be called once per this lib usage 
	*/
	static void xibridge_shutdown();
	
	bool open_connection_device();

	bool is_connected();

	void disconnect();
	
	bool close_connection_device();

	bvector  send_data_and_receive(bvector data);
	/*
	*
	*@param [out] extra_enum_data - extra data for enumerated device, = nullptr if extra info is not desired
	*@param [out] extra_size - reference to extra data size
	*/
	static std::vector<uint32> enumerate_dev_numbers(uint8 *extra_enum_data, uint32& extra_size);

	uint32 get_server_protocol_version() const {return _server_protocol_version;}

	static uint32 get_self_protocol_version() { return xibridge_protocol_version(); }

	uint32 get_last_client_error() const { return _last_client_error; };

	void set_last_client_error(uint32 err) { _last_client_error = err; }

	uint32 get_last_server_error() const { return _last_client_error; };

	void clr_errors() { _last_client_error = _last_server_error = 0; }

	const char *get_last_client_error_expl() const { return ""; }

	const char *get_last_server_error_expl() const { return ""; }

private:

	bool _send_and_receive(bvector &req);

	uint32 _dev_num;    // device number in xibridge server enumeration
    uint32 _server_protocol_version; // commincation protocol version
	uint32 _send_tmout;
	uint32 _recv_tmout;

	conn_id_t _conn_id; // id of bindy connection
	
	std::condition_variable _is_recv; //
	bool _is_really_recv;
	std::mutex _mutex_recv;
	bvector _recv_message; // for bindy-callback
	std::mutex _mutex_message;

	uint32 _last_server_error;  // last error code of protocol
	uint32 _last_client_error; //  last client side error
	char _host[MAXHOST+1];
};

#endif