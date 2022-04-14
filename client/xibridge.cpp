#include <zf_log.h>

#include "xibridge.h"
#include "../common/defs.h"
#include "xibridge_client.h"


// to make log level controlled
ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

xibridge_version_t xibridge_version()
{
	return Xibridge_client::xi_get_version();
}

xibridge_version_t xibridge_get_max_protocol_version()
{
	return Xibridge_client::xi_get_max_protocol_version();
}

uint32_t xibridge_init(const char *key_file_path)
{
	return Xibridge_client::xi_init(key_file_path);
}

uint32_t xibridge_set_connection_protocol_verion(xibridge_conn_t conn, xibridge_version_t ver)
{
	return Xibridge_client::xi_set_connection_protocol_version(conn, ver);
}

xibridge_version_t xibridge_get_connection_protocol_version(xibridge_conn_t conn)
{
	return Xibridge_client::xi_get_connection_protocol_version(conn);
}

uint32_t xibridge_open_device_connection(const char *xi_net_uri, unsigned int recv_timeout, xibridge_conn_t *pconn)
{
    uint32_t res_err;
	
	Xibridge_client * cl = new Xibridge_client(xi_net_uri, TIMEOUT, recv_timeout);
	if (!cl->open_connection_device())
	{
		cl->disconnect();
		res_err = cl -> get_last_error();
		delete cl;
		return res_err;
	}
    if (pconn != nullptr)
    {
        *pconn = cl -> to_xibridge_conn_t();
    }
    return 0;
}

uint32_t xibridge_close_device_connection(xibridge_conn_t conn)
{
	Xibridge_client::xi_close_connection_device(conn);
}

uint32_t  xibridge_detect_protocol_version(const char *addr, uint32_t send_timeout, uint32_t recv_timeout)
{
	return Xibridge_client::xi_detect_protocol_version(addr, send_timeout, recv_timeout);
}

/*
* Функция выполнения запроса
* To do timeout ???
*/
int xibridge_device_request_response(xibridge_conn_t conn, 
                                     const unsigned char *req, 
                                     int req_len, 
                                     unsigned char *resp, 
                                     int resp_len,
                                     uint32_t *res_err)
{
	return Xibridge_client::xi_request_response(conn, req, req_len, resp, resp_len, res_err) == false ? 0 : 1;
}

/*
* Для некоторых применений требуется просто читать буфер и смотреть на количество 
*/

const char * xibridge_get_err_expl(uint32_t err_no)
{
	return Xibridge_client::xi_get_err_expl(err_no);
}

uint32_t xibridge_get_last_err_no(xibridge_conn_t conn) 
{
	return  Xibridge_client::xi_get_last_err_no(conn);
}

int xibridge_enumerate_adapter_devices(const char *addr, const char *adapter,
	                                    unsigned char **result,
	                                    uint32_t *pcount, uint32_t timeout,
	                                    uint32_t* last_errno)
{

    return Xibridge_client::xibridge_enumerate_adapter_devices(addr, adapter,
                                                result,
                                                pcount, timeout,
                                                last_errno) == true ? 1 : 0;
}
