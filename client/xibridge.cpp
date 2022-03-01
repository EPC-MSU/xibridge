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
	unsigned int serial, unsigned int proto)
{
	Xibridge_client * cl = new Xibridge_client(addr, serial, proto);
	if (!cl->open_connection_device())
	{

		cl->disconnect();
		delete cl;
		return 0;
	}
	else
	    return (unsigned int) cl -> conn_id();
}

/*
Функция закрытия данного подключения
*/
extern void xibridge_close_device_connection(unsigned int conn_id)
{
	Xibridge_client::xibridge_close_connection_device(conn_id);
}

extern unsigned int  xibridge_detect_protocol_version(const char *addr)
{
	return Xibridge_client::xibridge_detect_protocol_version(addr, 5000, 15000);
}
