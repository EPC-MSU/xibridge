#include <zf_log.h>
#include <../common/protocols.h>
#include <../client/xibridge.h>
#include "ximc-min/ximc_min.h"
#include "../common/utils.h"

// to run with ximc-xinet-server
// !!! select the right address every time as tested
static const char * const _DEV_IP = "xi-net://172.16.130.38/1f50";

bool test_connect_1()
{
    uint32_t err = xibridge_init();

    if (err)
    {
        ZF_LOGE("Cannot initalize xibridge system: %s", xibridge_get_err_expl(err));
        return FALSE;
    }

    xibridge_conn_t conn;
    //xibridge_set_base_protocol_version({ 1, 0, 0 });
    err = xibridge_open_device_connection(_DEV_IP, &conn);
    if (err)
    {
        ZF_LOGE("Cannot open device: %s, error: %s", _DEV_IP, xibridge_get_err_expl(err));
        xibridge_close_device_connection(&conn);
        return FALSE;
    }

    //move_settings_calb_t resp_s;
    uint32_t serial;

    uint32_t xir_err = xibridge_device_request_response(&conn, (const unsigned char *)"GSER", 4, (unsigned char *)&serial, sizeof(uint32_t));
    if (xir_err)
    {
        ZF_LOGE("Cannot execute xir: %s", xibridge_get_err_expl(err));
        return FALSE;
    }

    //ZF_LOGD("Speed: %f\n", resp_s.Speed);
    //ZF_LOGD("Accelerartion: %f\n", resp_s.Accel);

    ZF_LOGD("Serial: %u", serial);

    xibridge_close_device_connection(&conn);
    char  *pdata; uint32_t count;

    xibridge_enumerate_adapter_devices(_DEV_IP, "", &pdata, &count);
    ZF_LOGD("Count of enumerated devices: %u", count);
    if (count)
    {
        const char *p = pdata;
        for (int i = 0; i < count;  i++)
        {
            ZF_LOGD("Enumerated device #%d: URI: %s", i+1, p);
            p = strchr(p, 0) + 1;
        }
    }
    xibridge_free_enumerate_devices(pdata);

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
	
	ZF_LOGD("Thread %u: openning connection... \n", thread_num);
    xibridge_conn_t conn;
    err = xibridge_open_device_connection(_DEV_IP, &conn);
	ZF_LOGD("Thread %u: connection opened, conn_id: %u \n", thread_num, conn.conn_id);
	ZF_LOGD("Thread %u: sending GSER... \n", thread_num);
	//move_settings_calb_t resp_s;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    uint32_t serial;
  	err = xibridge_device_request_response(&conn, (const unsigned char *)"GSER", 3, (unsigned char *)&serial, sizeof(uint32_t));

	ZF_LOGD("Thread %u: GSER return %s\n", thread_num,
		err == 0 ? "true" : "false");
	ZF_LOGD("Thread %u: closing connection %u... \n", thread_num, conn.conn_id);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	xibridge_close_device_connection(&conn);
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
