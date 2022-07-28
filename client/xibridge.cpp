#include "../misc/defs.h"
#include "xibridge.h"
#include "../misc/defs.h"
#include "xibridge_client.h"

xibridge_version_t xibridge_version()
{
	return Xibridge_client::xi_get_version();
}

xibridge_version_t xibridge_get_max_protocol_version()
{
	return Xibridge_client::xi_get_max_protocol_version();
}

uint32_t xibridge_set_base_protocol_version(xibridge_version_t ver)
{
	return Xibridge_client::xi_set_base_protocol_version(ver);
}

xibridge_version_t xibridge_get_connection_protocol_version(const xibridge_conn_t *pconn)
{
	return Xibridge_client::xi_get_connection_protocol_version(pconn);
}

uint32_t xibridge_open_device_connection(const char *xi_net_uri,  xibridge_conn_t *pconn)
{
    uint32_t res_err;
    if (pconn == nullptr) return ERR_NULLPTR_PARAM;
    *pconn = xibridge_conn_invalid;
	Xibridge_client * cl = new Xibridge_client(xi_net_uri);

    
    // making opening logic more complex
    uint32_t ncount = xibridge_get_max_protocol_version().major;
    while (ncount--)
    {

		if (!cl->open_connection())
		{
label_noconn:
            res_err = cl->get_last_error();
			delete cl;
			return res_err;
		}
        cl->clr_errors();
		cl->open_device();
		res_err = cl->get_last_error();
		if (res_err == ERR_ANOTHER_PROTOCOL)  // another protocol required
        {
			// second and final chance
            // if (ncount) ncount--;
            cl->clr_errors();
			cl->disconnect();
			if (!cl->open_connection()) goto label_noconn;
			cl->open_device();
			res_err = cl->get_last_error();
            break;
        }
		else if (res_err == ERR_RECV_TIMEOUT)  // may try another protocol - go far
        {
            cl->decrement_server_protocol_version();
            //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
			cl->disconnect();
            //std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
	    else // either all right or some non timeout error - exit anyway
	    {
				break;
		}
    }
	if (res_err)
	{
		cl->disconnect();
		delete cl;
	}
	else
		*pconn = cl->to_xibridge_conn_t();
    return res_err;
}

uint32_t xibridge_close_device_connection(const xibridge_conn_t *pconn)
{
	return Xibridge_client::xi_close_connection_device(pconn);
}


uint32_t xibridge_device_request_response(
	                                          const xibridge_conn_t *pconn, 
                                              const uint8_t *req, 
                                              uint32_t req_len, 
                                              uint8_t *resp, 
                                              uint32_t resp_len
										  )
{
	return Xibridge_client::xi_request_response(pconn, req, req_len, resp, resp_len);
}

const char * xibridge_get_err_expl(uint32_t err_no)
{
	return Xibridge_client::xi_get_err_expl(err_no);
}

// to do - adapter using 
uint32_t xibridge_enumerate_adapter_devices(
	                                            const char *addr, 
	                                            const char *adapter,
	                                            char **ppresult,
	                                            uint32_t *pcount
											)
{

	uint32_t res_err;
    if (ppresult == nullptr || pcount == nullptr) return ERR_NULLPTR_PARAM;
    *pcount = 0;
    *ppresult = nullptr;
    Xibridge_client * cl = new Xibridge_client(addr, adapter == NULL ? "" : adapter);
   
    // making opening logic more complex
    uint32_t ncount = xibridge_get_max_protocol_version().major;
    while (ncount--)
    {
        cl->clr_errors();

		if (!cl->open_connection())
		{
label_noconn:
            res_err = cl->get_last_error();
			delete cl;
            return res_err;
		}
        cl->exec_enumerate(ppresult, pcount);
        res_err = cl->get_last_error();
		
		if (res_err == ERR_ANOTHER_PROTOCOL)  // another protocol required
		{
			// second and final chance
			// if (ncount) ncount--;
			cl->clr_errors();
			cl->disconnect();
            if (!cl->open_connection()) goto label_noconn;
 			cl->exec_enumerate(ppresult, pcount);
			res_err = cl->get_last_error();
			break;  
		}
     
        else if (res_err == ERR_RECV_TIMEOUT || res_err == ERR_NO_PROTOCOL)  // may another protocol
        {
                cl->decrement_server_protocol_version();
				cl->disconnect();
        }
		else
		{
			// emumerated just fine or not - going out
			break;
		}
		
    }
    cl->disconnect();
    delete cl;
    return res_err;
}

void xibridge_free_enumerate_devices(char *presult)
{
    Xibridge_client::xi_free_enumerate_devices(presult);

}
