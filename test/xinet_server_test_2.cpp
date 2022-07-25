#include <zf_log.h>
#include <thread>
#include <../common/protocols.h>
#include <../client/xibridge.h>
#include "urmc-min/urmc_min.h"
#include "../common/utils.h"

// to run with urpc-xinet-server !!! the local urpc-connect
// to run with ximc-xinet-server
// !!! select the right address every time as tested
static char  _DEV_IP[256];
;
bool test_connect_2(const char * ip, uint32_t dev_num)
{
  
    sprintf(_DEV_IP, "xi-net://%s/%ux", ip, dev_num);
	xibridge_conn_t conn;
    uint32_t err = xibridge_open_device_connection(_DEV_IP, &conn);
    if (err)
    {
        ZF_LOGE("Cannot open %s: %s", _DEV_IP, xibridge_get_err_expl(err));
        return false;
    }
	unsigned char resp[72+4];
   
	uint32_t ginf_err = xibridge_device_request_response(&conn, (const unsigned char *)"ginf", 4, resp, 72+4);
	if (ginf_err)
	{
		ZF_LOGE("Cannot execute ginf: %s", xibridge_get_err_expl(ginf_err));
		return false;
	}
    
    // urmc_get_identity_information_t  info;
	Hex32  urpc_res;
	MBuf mresp(resp, 72+4);
	
	mresp >> urpc_res;
	ZF_LOGD("Urpc return code: %d\n", (int)urpc_res);

    char man[16 + 1];
    memset(man, 0, 16 + 1);
	mresp.memread((uint8_t *)man, 16, 16);
	ZF_LOGD("Manufacture: %s\n", man);
	
    memset(man, 0, 16 + 1);
	mresp.memread((uint8_t *)man, 16, 16);
	ZF_LOGD("Product name: %s\n", man);

    memset(man, 0, 16 + 1);
	mresp.memread((uint8_t *)man, 16, 16);
	ZF_LOGD("Controller: %s\n", man);

    uint32_t gets_err = xibridge_device_request_response(&conn, (const unsigned char *)"gets", 4, resp, 48 + 4);
    if (gets_err)
    {
        ZF_LOGE("Cannot execute gets: %s", xibridge_get_err_expl(gets_err));
        return false;
    }
	
	xibridge_close_device_connection(&conn);

	return true;
}

static void thread_body(int thread_num)
{
	xibridge_conn_t conn;
	ZF_LOGD("Thread %u: openning connection... \n", thread_num);
    uint32_t err = xibridge_open_device_connection(_DEV_IP, &conn);
	ZF_LOGD("Thread %u: connection opened, conn_id: %u \n", thread_num, conn.conn_id);
	unsigned char resp[72+4];
	ZF_LOGD("Thread %u: sending ginf... \n", thread_num);
	uint32_t ginf_err = xibridge_device_request_response(&conn, (const unsigned char *)"ginf", 4, resp, 72+4);
	
	ZF_LOGD("Thread %u: ginf return %s\n", thread_num, 
		     ginf_err == 0 ? "true" : "false");
	ZF_LOGD("Thread %u: closing connection %u... \n", thread_num, conn.conn_id);
	xibridge_close_device_connection(&conn);
	ZF_LOGD("Thread %u: Connection %u closed \n", thread_num, conn.conn_id);
}

void test_connect_2_threads()
{
#define TH_NUM 5

	std::thread *pthreads[TH_NUM];

	for (auto i = 0; i < TH_NUM; i++)
	{
		pthreads[i] = new std::thread(thread_body, i);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	for (auto i = 0; i < TH_NUM; i++)
	{
		pthreads[i]->join();
	}

	for (auto i = 0; i < TH_NUM; i++)
	{
		delete pthreads[i];
	}
}