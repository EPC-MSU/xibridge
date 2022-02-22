#include <zf_log.h>
#include <../common/protocols.h>
#include <../client/xibridge.h>
  

bool test_connect_2()
{
	if (!xibridge_init("keyfile.sqlite"))
	{
		ZF_LOGE("Cannot initalize xibridge system!");
		return FALSE;
	}

	unsigned int connection = xibridge_open_device_connection("127.0.0.1", 3, 2);

	xibridge_close_device_connection(connection);

	xibridge_shutdown();
}
