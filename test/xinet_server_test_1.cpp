#include <zf_log.h>
#include <../common/protocols.h>
#include <../client/xibridge.h>
#include "ximc-min/ximc_min.h"
#include "../common/utils.h"

// to run with ximc-xinet-server
bool test_connect_1()
{
	if (!xibridge_init("keyfile.sqlite"))
	{
		ZF_LOGE("Cannot initalize xibridge system!");
		return FALSE;
	}
	unsigned int version = xibridge_detect_protocol_version("127.0.0.1", 3000, 5000);

	unsigned int res_err, last_err;

	unsigned int connection = xibridge_open_device_connection("127.0.0.1", 9, version, TIMEOUT_3000, &last_err);
    

	move_settings_calb_t resp_s;
	int _ok = xibridge_device_request_response(connection, (const unsigned char *)"XIR", 3, (unsigned char *)&resp_s, sizeof(resp_s), &res_err);

	ZF_LOGD("Speed: %f\n", resp_s.Speed);
	ZF_LOGD("Accelerartion: %f\n", resp_s.Accel);
		
	xibridge_close_device_connection(connection);
	return true;
}

static void thread_body(int thread_num)
{
	
	if (!xibridge_init("keyfile.sqlite"))
	{
	ZF_LOGE("Thread %u: cannot initalize xibridge system!", thread_num);
	return;
	}
	
	unsigned int res_err, last_err;
	//unsigned int version = xibridge_detect_protocol_version("127.0.0.1", 3000, 5000);
	ZF_LOGD("Thread %u: openning connection... \n", thread_num);
	unsigned int connection = xibridge_open_device_connection("127.0.0.1", 9, 1, TIMEOUT_3000, &last_err);
	unsigned char resp[72];
	ZF_LOGD("Thread %u: sending xir... \n", thread_num);
	move_settings_calb_t resp_s;
	int _ok = xibridge_device_request_response(connection, (const unsigned char *)"XIR", 3, (unsigned char *)&resp_s, sizeof(resp_s), &res_err);

	ZF_LOGD("Thread %u: xir return %s\n", thread_num,
		_ok != 0 ? "true" : "false");

	xibridge_close_device_connection(connection);
}

void test_connect_1_threads()
{
	for (auto i = 0; i < 10; i++)
	{
		std::thread th(thread_body, i);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}


}
