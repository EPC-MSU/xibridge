#include <zf_log.h>
#include <../common/protocols.h>
#include <../client/xibridge.h>
#include "ximc-min/ximc_min.h"
#include "../common/utils.h"

// to run with ximc-xinet-server
// !!! select the right address every time as tested
static const char * const _DEV_IP = "xi-net://172.16.130.55/1f50";
static const char *const _IP = "172.16.130.55";


PACK(
struct _gets_re
{
    char gets[4];
    status_t status;
});

typedef struct _gets_re re_gets;

PACK(
struct _geng_re
{
    char gets[4];
    engine_settings_t settings;
});

typedef struct _geng_re re_geng;

bool test_connect_1()
{
    uint32_t err = xibridge_init();

    if (err)
    {
        ZF_LOGE("Cannot initalize xibridge system: %s", xibridge_get_err_expl(err));
        return false;
    }
    char  *pdata; uint32_t count;

    xibridge_enumerate_adapter_devices(_IP, "", &pdata, &count);
    ZF_LOGD("Count of enumerated devices: %u", count);
    if (count)
    {
        const char *p = pdata;
        for (int i = 0; i < (int)count; i++)
        {
            ZF_LOGD("Enumerated device #%d: URI: %s", i + 1, p);
            p = strchr(p, 0) + 1;
        }
    }
    xibridge_free_enumerate_devices(pdata);

    xibridge_conn_t conn;
    //xibridge_set_base_protocol_version({ 1, 0, 0 });
    err = xibridge_open_device_connection(_DEV_IP, &conn);
    if (err)
    {
        ZF_LOGE("Cannot open device: %s, error: %s", _DEV_IP, xibridge_get_err_expl(err));
        xibridge_close_device_connection(&conn);
        return false;
    }

   
    re_gets status;
    uint32_t err_op = xibridge_device_request_response(&conn, (const uint8_t *)"gets", 4, (uint8_t *)&status, sizeof(re_gets));
    if (err_op)
    {
        ZF_LOGE("Cannot execute gets: %s", xibridge_get_err_expl(err));
        return false;
    }

    ZF_LOGD("Speed: %d", status.status.CurSpeed);

    re_geng settings;
    err_op = xibridge_device_request_response(&conn, (const uint8_t *)"geng", 4, (uint8_t *)&settings, sizeof(re_geng));
    ZF_LOGD("Nom voltage: %u", settings.settings.NomVoltage);
    xibridge_close_device_connection(&conn);
    //char  *pdata; uint32_t count;

    xibridge_enumerate_adapter_devices(_IP, "", &pdata, &count);
    ZF_LOGD("Count of enumerated devices: %u", count);
    if (count)
    {
        const char *p = pdata;
        for (int i = 0; i < (int)count;  i++)
        {
            ZF_LOGD("Enumerated device #%d: URI: %s", i+1, p);
            p = strchr(p, 0) + 1;
        }
    }
    xibridge_free_enumerate_devices(pdata);

	return true;
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
	ZF_LOGD("Thread %u: sending gets... \n", thread_num);
	//move_settings_calb_t resp_s;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //uint32_t serial;
    status_t status;
  	err = xibridge_device_request_response(&conn, (const uint8_t *)"gets", 3, (uint8_t *)&status, sizeof(status_t));

	ZF_LOGD("Thread %u: gets return %s\n", thread_num,
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
