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

uint32_t xibridge_init()
{
	return Xibridge_client::xi_init();
}

uint32_t xibridge_set_base_protocol_verion(xibridge_version_t ver)
{
	return Xibridge_client::xi_set_base_protocol_version(ver);
}

xibridge_version_t xibridge_get_connection_protocol_version(const xibridge_conn_t *pconn)
{
	return Xibridge_client::xi_get_connection_protocol_version(pconn);
}

uint32_t xibridge_open_device_connection(const char *xi_net_uri,  xibridge_conn_t *pconn)
{
    uint32_t res_err, answer_version;
    if (pconn == nullptr) return ERR_NULLPTR_PARAM;
    *pconn = { 0, {0,0,0} };
	Xibridge_client * cl = new Xibridge_client(xi_net_uri);
    if (!cl->open_connection())
    {
        res_err = cl -> get_last_error();
        delete cl;
        return res_err;
    }
    // making opening logic more complex
    uint32_t ncount = xibridge_get_max_protocol_version().major;
    while (ncount--)
    {
        cl->clr_errors();
        bool result = cl->open_device(answer_version);
        res_err = cl->get_last_error();
        if (res_err == ERR_ANOTHER_PROTOCOL)  // another protocol required
        {
            if (ncount) ncount--;
            cl->clr_errors();
            if (!cl->open_device(answer_version))
                break;
            // opened just fine
           *pconn = cl->to_xibridge_conn_t();
            return 0;
         }
        else
        {
            if (res_err == ERR_RECV_TIMEOUT)  // may another protocol
            {
                cl->decrement_server_protocol_version();
            }
            else
                break;
        }
    }
    cl->disconnect();
    delete cl;
    return res_err;
}

uint32_t xibridge_close_device_connection(xibridge_conn_t conn)
{
	return Xibridge_client::xi_close_connection_device(conn);
}

/*
* Функция выполнения запроса
* To do timeout ???
*/
uint32_t xibridge_device_request_response(xibridge_conn_t conn, 
                                     const unsigned char *req, 
                                     int req_len, 
                                     unsigned char *resp, 
                                     int resp_len)
{
	return Xibridge_client::xi_request_response(conn, req, req_len, resp, resp_len);
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

// to do - adapter using 
uint32_t xibridge_enumerate_adapter_devices(const char *addr, const char */*adapter*/,
	                                    char **ppresult,
	                                    uint32_t *pcount)
{

    uint32_t res_err, answer_version;
    if (ppresult == nullptr || pcount) return ERR_NULLPTR_PARAM;
    pcount = 0;
    *ppresult = nullptr;
    Xibridge_client * cl = new Xibridge_client(addr);
    if (!cl->open_connection())
    {
        res_err = cl->get_last_error();
        delete cl;
        return res_err;
    }
    // making opening logic more complex
    uint32_t ncount = xibridge_get_max_protocol_version().major;
    while (ncount--)
    {
        cl->clr_errors();
        bool result = cl->exec_enumerate(ppresult, pcount, answer_version);
        res_err = cl->get_last_error();
        if (res_err == ERR_ANOTHER_PROTOCOL || res_err == ERR_NO_PROTOCOL)  // another protocol required
        {
            if (ncount) ncount--;
            cl->clr_errors();
            if (!cl->exec_enumerate(ppresult, pcount, answer_version))
                break;
            // emumerated just fine
            cl->close_connection_device();
            delete cl;
            return 0;
        }
        else
        {
            if (res_err == ERR_RECV_TIMEOUT)  // may another protocol
            {
                cl->decrement_server_protocol_version();
            }
            else
                break;
        }
    }
    cl->disconnect();
    delete cl;
    return res_err;
}
