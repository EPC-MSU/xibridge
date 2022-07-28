#ifndef _BINDY_HELPER_H
#define _BINDY_HELPER_H
#include <map>
#include <mutex> 

#include "../misc/defs.h"
#include "../misc/protocols.h"
#include "xibridge_client.h"

/**
 * Helper class for using Bindy as a client
*/
class Xibridge_client;
class Bindy_helper
{
	friend class Xibridge_client;
public:

	~Bindy_helper(){ shutdown_bindy(); };
	static bindy::Bindy * instance_bindy();
	conn_id_t connect(Xibridge_client *pcl);
	static void shutdown_bindy();
	static Bindy_helper *instance() { return &_bhelper; }
	bool is_connected(conn_id_t conn_id) const;
	bool send_bindy_data(conn_id_t conn_id, bvector data);
	void disconnect(conn_id_t conn_id);
private:
	static std::mutex _global_mutex;  // mutex of  global resource initialization

	static void callback_data_bindy(conn_id_t conn_id, std::vector<uint8_t> data);
	static void on_bindy_disconnect(conn_id_t conn_id);

	static bindy::Bindy *_pbindy;
	static std::mutex _map_mutex;
	static std::map<conn_id_t, Xibridge_client *> _map;
	static Bindy_helper _bhelper;
};


#endif