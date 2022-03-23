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
#define ERR_NO_PROTOCOL 1
#define ERR_NO_CONNECTION 2
#define ERR_SEND_TIMEOUT  3
#define ERR_NO_BINDY    4
#define ERR_SEND_DATA 5
/*
* Defines protocol and server errors
*/
#define ERR_RECV_TIMEOUT 6

/*
* The wide used tiemout value to use by default
*/
#define TIMEOUT_HISTORICAL 3000

/*
When no answerback data len known this time value is used to control answer arrival
*/
//#define TIMEOUT_WAIT_STANDARD 100 

/*
When no answerback data len known this buffer value is used to control answer data size
*/
//#define MAX_ANSWER_LEN 512
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
	
	/*
	 Функция установки весрии протокола сервера для данного подключения
    */
	static void xibridge_set_server_protocol(unsigned int conn_id, unsigned int proto);

	/*
	* Функция получения версии протокола сервера для данного подключения
	*/
	static unsigned int xibridge_get_server_protocol(unsigned int conn_id);

	/*
	* Функция закрытия данного подключения
	*/
	static void xibridge_close_connection_device(unsigned int conn_id);

	/*
	* Функция по получению номеров (компортов, устройств) доступных на xibridge-сервере
	*/
	static void xibridge_enumerate_devices(const char *addr, 
		                                   unsigned int proto, 
										   unsigned int *result, 
										   unsigned int *pcount,
										   unsigned int* last_errno = nullptr);

	/* This constructor to create Xibridge_client with defined addr, device number and version number
	 *@param addr - xibridge server address
	 *@param dev_num - device number (COM-port or slot)
	 *@param version - protocol version
	*/
	Xibridge_client(const char *addr, 
		            unsigned int serial,  
		            unsigned int proto_ver = 0, 
				    unsigned int send_tmout = TIMEOUT_HISTORICAL, 
					unsigned int recv_tmout = TIMEOUT_HISTORICAL);

	/**
	   * This static member function sets up connecton-specific data
	   * Must be called once per this lib usage 
	   * Multiple clients can be created then
	*/


	void set_server_protocol_version(uint32 v) { _server_protocol_version = v; };

	/**
	* This static member function to recognize server protocol version
	*/
	static uint32  xibridge_detect_protocol_version(const char *addr, uint32 timeout_1, uint32 timeout_all);

	/**
	* This static member function to setup network and bindy - once per application
	*/
	static bool xibridge_init(const char *key_file_path);
	
	/**
	* This static member function shutdown network and bindy - once per application
	* Must be called once per this lib usage 
	*/
	static void xibridge_shutdown();

	/**
	* This static member function executes response-request to device
	@param conn_id - connection id
	@param req - request data
	@param req_len - request length
	@param resp - buffer for reponse data
	@resp_len - optional response length in bytes
	@err_no - optional pointer to err number variable
	*/
	static bool xibridge_request_response(unsigned int conn_id, 
		                                  const unsigned char *req, 
										  int req_len, 
										  unsigned char *resp, 
										  int resp_len );

	/*
	* This static member function returns last error text according to xibridge classification
	*/
	static void xibridge_get_err_expl(char * s, int len, bool is_russian, unsigned int err_no);

	/*
	* This static member function  returns last err number of the connection conn_id
	*/
	static unsigned int xibridge_get_last_err_no(unsigned int conn_id);
	
	bool open_connection_device();

	bool is_connected();

	void disconnect();
	
	bool close_connection_device();

	conn_id_t conn_id() const { return _conn_id; }

	bvector  send_data_and_receive(bvector data, uint32 resp_length);
	/*
	*
	*@param [out] extra_enum_data - extra data for enumerated device, = nullptr if extra info is not desired
	*@param [out] extra_size - reference to extra data size
	*/
	static std::vector<uint32> enumerate_dev_numbers(uint8 *extra_enum_data, uint32& extra_size);

	uint32 get_server_protocol_version() const {return _server_protocol_version;}

	static uint32 get_self_protocol_version() { return xibridge_protocol_version(); }

	uint32 get_last_error() const { return _last_error; };

	void clr_errors() { _last_error = 0; }

	void get_error_expl(char * s, int len, bool is_russian = false) const;

	uint32 get_dev_num() const {
		return _dev_num;
	}

	uint32 get_resv_tmout() const { return _recv_tmout; }

	uint32 get_proto_version_of_the_recv_message() const { return AProtocol::get_version_of_cmd(_recv_message); }

private:
	
	bool _send_and_receive(bvector &req);
	void _set_last_error(uint32 err) { _last_error = err; }

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

	uint32 _last_error;  // last error 
	// uint32 _last_client_error; //  last client side error
	char _host[MAXHOST+1];
};

#endif