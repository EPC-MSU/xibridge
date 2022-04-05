#include <zf_log.h>
#include <thread>
#include <../common/protocols.h>
#include <../client/xibridge.h>
#include "urmc-min/urmc_min.h"
#include "../common/utils.h"

// to run with urpc-xinet-server !!! the local urpc-connect
bool test_connect_2()
{
	if (!xibridge_init("keyfile.sqlite"))
	{
		ZF_LOGE("Cannot initalize xibridge system!");
		return FALSE;
	}
	unsigned int res_err, last_err;
	unsigned int version = xibridge_detect_protocol_version("127.0.0.1", 3000, 5000);
	unsigned int connection = xibridge_open_device_connection("127.0.0.1", 9, version, TIMEOUT_3000, &last_err);
	unsigned char resp[72];
   
	int ginf_ok = xibridge_device_request_response(connection, (const unsigned char *)"ginf", 4, resp, 72, &res_err);

	// ��������� ���������
	// to do - sync 
	// urmc_get_identity_information_t  info;
	
	MBuf mresp(resp, 72);
	/*
	p = out_buffer;
	for (i = 0; i<16; i++) output->Manufacturer[i] = pop_uint8_t(&p);
	for (i = 0; i<16; i++) output->ProductName[i] = pop_uint8_t(&p);
	for (i = 0; i<16; i++) output->ControllerName[i] = pop_uint8_t(&p);
	output->HardwareMajor = pop_uint8_t(&p);
	output->HardwareMinor = pop_uint8_t(&p);
	output->HardwareBugfix = pop_uint16_t(&p);
	output->BootloaderMajor = pop_uint8_t(&p);
	output->BootloaderMinor = pop_uint8_t(&p);
	output->BootloaderBugfix = pop_uint16_t(&p);
	output->FirmwareMajor = pop_uint8_t(&p);
	output->FirmwareMinor = pop_uint8_t(&p);
	output->FirmwareBugfix = pop_uint16_t(&p);
	output->SerialNumber = pop_uint32_t(&p);
	*/
	
    char man[16 + 1];
    memset(man, 0, 16 + 1);
	mresp.memread((uint8 *)man, 16, 16);
	ZF_LOGD("Manufacture: %s\n", man);
	
    memset(man, 0, 16 + 1);
	mresp.memread((uint8 *)man, 16, 16);
	ZF_LOGD("Product name: %s\n", man);

    memset(man, 0, 16 + 1);
	mresp.memread((uint8 *)man, 16, 16);
	ZF_LOGD("Controller: %s\n", man);
	
	xibridge_close_device_connection(connection);

	//xibridge_shutdown();

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
	unsigned int connection = xibridge_open_device_connection("127.0.0.1", 9, 2, TIMEOUT_3000, &last_err);
	unsigned char resp[72];
	ZF_LOGD("Thread %u: sending ginf... \n", thread_num);
	int ginf_ok = xibridge_device_request_response(connection, (const unsigned char *)"ginf", 4, resp, 72, &res_err);
	
	ZF_LOGD("Thread %u: ginf return %s\n", thread_num, 
		     ginf_ok != 0 ? "true" : "false");

	xibridge_close_device_connection(connection);
}

void test_connect_2_threads()
{
	for (auto i = 0; i < 10; i++)
	{
		//std::thread th(thread_body, i);
		thread_body(i);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}


}