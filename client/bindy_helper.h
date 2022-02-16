#ifndef _BINDY_HELPER_H
#define _BINDY_HELPER_H
#include <map>
#include <mutex>

#include "../common/defs.h"
#include "../common/protocols.h"
#include "xibridge_client.h"

/**
 * Helper class for using Bindy as a client
*/
class Bindy_helper
{
	friend class Xibridge_client;
public:

	//Bindy_helper(const char *keyfile);
	static bindy::Bindy * instance_bindy();
	conn_id_t connect(const char *addr, Xibridge_client *pcl);
	static void set_keyfile(const char *keyfile) { _keyfile = keyfile; }
	static void shutdown_bindy(){ bindy::Bindy::shutdown_network(); delete _pbindy; _pbindy = nullptr; }
	static Bindy_helper *instance() { return &_bhelper; }
	
	bool send_bindy_data(conn_id_t conn_id, bvector data);

private:
	static void callback_data_bindy(conn_id_t conn_id, std::vector<uint8_t> data);
	static void on_bindy_disconnect(conn_id_t conn_id);

	static bindy::Bindy *_pbindy;
	static const char *_keyfile;
	static std::mutex _map_mutex;
	static std::map<conn_id_t, Xibridge_client *> _map;

	static Bindy_helper _bhelper;
};


#endif