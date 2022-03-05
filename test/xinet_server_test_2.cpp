#include <zf_log.h>
#include <../common/protocols.h>
#include <../client/xibridge.h>
#include "urmc-min/urmc_min.h"
#include "../common/utils.h"


bool test_connect_2()
{
	if (!xibridge_init("keyfile.sqlite"))
	{
		ZF_LOGE("Cannot initalize xibridge system!");
		return FALSE;
	}
	unsigned int version = xibridge_detect_protocol_version("127.0.0.1");
	unsigned int connection = xibridge_open_device_connection("127.0.0.1", 9, version);
	unsigned char resp[72];
	bool ginf_ok = xibridge_device_request_response(connection, (const unsigned char *)"ginf", 4, resp, 72);

	// разобрать структуру
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
	char man[16];
	mresp.memread((uint8 *)man, 16, 16);
	ZF_LOGD("Manufacture: \n", man);
	
	char prod_name[16]; 
	mresp.memread((uint8 *)prod_name, 16, 16);
	ZF_LOGD("Manufacture: \n", prod_name);

	char contr_name[16];
	mresp.memread((uint8 *)contr_name, 16, 16);
	ZF_LOGD("Manufacture: \n", contr_name);
	
	xibridge_close_device_connection(connection);

	xibridge_shutdown();

	return true;
}
