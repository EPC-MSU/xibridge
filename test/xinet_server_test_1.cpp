#include <zf_log.h>
#include <../common/protocols.h>
#include <../client/xibridge.h>
#include "urmc-min/urmc_min.h"
#include "../common/utils.h"

// from ximc.h
typedef struct
{
	float Speed;	/**< \english Target speed. \endenglish \russian Заданная скорость. \endrussian */
	float Accel;	/**< \english Motor shaft acceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian Ускорение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). \endrussian */
	float Decel;	/**< \english Motor shaft deceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian Торможение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). \endrussian */
	float AntiplaySpeed;	/**< \english Speed in antiplay mode. \endenglish \russian Скорость в режиме антилюфта. \endrussian */
	unsigned int MoveFlags;	/**< \english \ref flagset_moveflags "Flags of the motion parameters". \endenglish \russian \ref flagset_moveflags "Флаги параметров движения". \endrussian */
} move_settings_calb_t;

//

// to run with ximc-xinet-server
bool test_connect_1()
{
	if (!xibridge_init("keyfile.sqlite"))
	{
		ZF_LOGE("Cannot initalize xibridge system!");
		return FALSE;
	}
	unsigned int version = xibridge_detect_protocol_version("127.0.0.1");
	unsigned int connection = xibridge_open_device_connection("127.0.0.1", 9, version);
	
	move_settings_calb_t resp_s;
	bool _ok = xibridge_device_request_response(connection, (const unsigned char *)"XIR", 3, (unsigned char *)&resp_s, sizeof(resp_s));

	// разобрать структуру
	// to do - sync 
	// urmc_get_identity_information_t  info;

	ZF_LOGD("Speed: %f\n", resp_s.Speed);
	ZF_LOGD("Accelerartion: %f\n", resp_s.Accel);
		
	xibridge_close_device_connection(connection);

	xibridge_shutdown();

	return true;
}
