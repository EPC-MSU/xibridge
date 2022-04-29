#include <zf_log.h>
#include <../common/protocols.h>
#include <../client/xibridge.h>
#include "ximc-min/ximc_min.h"
#include "../common/utils.h"

// to run with ximc-xinet-server
bool test_connect_1()
{
    uint32_t err = xibridge_init();

	if (err)
	{
        ZF_LOGE("Cannot initalize xibridge system: %s", xibridge_get_err_expl(err));
		return FALSE;
	}
	//uint32_t version = xibridge_detect_protocol_version("127.0.0.1", 3000, 5000);

	uint32_t res_err, last_err;
    xibridge_conn_t conn;
	err = xibridge_open_device_connection("xi-net://192.168.0.87/1f50", TIMEOUT, &conn);
	if (err)
	{
		ZF_LOGE("Cannot open device: %s, error: %s", "xi-net://192.168.0.14/1f50", xibridge_get_err_expl(err));
		xibridge_close_device_connection(conn);
		return FALSE;
	}
    
    move_settings_calb_t resp_s;
	uint32_t xir_err = xibridge_device_request_response(conn, (const unsigned char *)"XIR", 3, (unsigned char *)&resp_s, sizeof(resp_s));
	if (xir_err)
	{
		ZF_LOGE("Cannot execute xir: %s", xibridge_get_err_expl(err));
		return FALSE;
	}

	ZF_LOGD("Speed: %f\n", resp_s.Speed);
	ZF_LOGD("Accelerartion: %f\n", resp_s.Accel);
		
	xibridge_close_device_connection(conn);
	return TRUE;
}

static void thread_body(int thread_num)
{
    uint32_t err = xibridge_init();

	if (err != 0)
	{
	  ZF_LOGE("Thread %u: cannot initalize xibridge system!", thread_num);
	  return;
	}
	//uint32_t version = xibridge_detect_protocol_version("127.0.0.1", 3000, 5000);
	ZF_LOGD("Thread %u: openning connection... \n", thread_num);
    xibridge_conn_t conn;
    err = xibridge_open_device_connection("xi - net://127.0.0.1/9", TIMEOUT, &conn);
	ZF_LOGD("Thread %u: connection opened, conn_id: %u \n", thread_num, conn.conn_id);
	unsigned char resp[72];
	ZF_LOGD("Thread %u: sending xir... \n", thread_num);
	move_settings_calb_t resp_s;
  	err = xibridge_device_request_response(conn, (const unsigned char *)"XIR", 3, (unsigned char *)&resp_s, sizeof(resp_s));

	ZF_LOGD("Thread %u: xir return %s\n", thread_num,
		err == 0 ? "true" : "false");
	ZF_LOGD("Thread %u: closing connection %u... \n", thread_num, conn.conn_id);
	xibridge_close_device_connection(conn);
	ZF_LOGD("Thread %u: Connection %u closed \n", thread_num, conn.conn_id);
}

void test_connect_1_threads()
{
#define TH_NUM 10

	std::thread *pthreads[TH_NUM];

	for (auto i = 0; i < TH_NUM; i++)
	{
		pthreads[i] = new std::thread(thread_body, i);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
