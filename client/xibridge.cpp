#include "xibridge.h"
#include "../Common/defs.h"
#include "xibridge_client.h"


int xibridge_major_version()
{
	return Xibridge_client::xibridge_major_version();
}

int xibridge_protocol_version()
{
	return Xibridge_client::xibridge_protocol_version();
}

bool xibridge_init(const char *key_file_path)
{
	return Xibridge_client::xibridge_init(key_file_path);
}

void xibridge_shutdown()
{
	Xibridge_client::xibridge_shutdown();
}

void xibridge_set_server_protocol_verion(unsigned int conn_id, unsigned int ver)
{
	Xibridge_client::xibridge_set_server_protocol(conn_id, ver);
}

unsigned int xibridge_get_server_protocol_version(unsigned int conn_id)
{
	return Xibridge_client::xibridge_get_server_protocol(conn_id);
}



unsigned int xibridge_open_device_connection(const char *addr,
	                                         unsigned int serial, 
											 unsigned int proto,
											 unsigned int *err_no)
{
	Xibridge_client * cl = new Xibridge_client(addr, serial, proto);
	if (!cl->open_connection_device())
	{
		cl->disconnect();
		if (err_no != nullptr)
			*err_no = cl -> get_last_error();
		delete cl;
		return 0;
	}
	else
	    return (unsigned int) cl -> conn_id();
}

/*
* Функция закрытия данного подключения
*/
void xibridge_close_device_connection(unsigned int conn_id)
{
	Xibridge_client::xibridge_close_connection_device(conn_id);
}

unsigned int  xibridge_detect_protocol_version(const char *addr)
{
	return Xibridge_client::xibridge_detect_protocol_version(addr, 5000, 15000);
}

/*
* Функция выполнения запроса
* To do timeout ???
*/
bool xibridge_device_request_response(unsigned int conn_id, const unsigned char *req, int req_len, unsigned char *resp, int resp_len)
{
	return Xibridge_client::xibridge_request_response(conn_id, req, req_len, resp, resp_len);
}

void xibridge_get_err_expl(char * s, int len, bool is_russian, unsigned int err_no)
{
	return Xibridge_client::xibridge_get_err_expl(s, len, is_russian, err_no);
}

unsigned int xibridge_get_last_err_no(unsigned int conn_id)
{
	return  Xibridge_client::xibridge_get_last_err_no(conn_id);
}
