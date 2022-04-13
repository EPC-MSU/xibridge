#include <vector>
#include <zf_log.h>

#include <bindy/bindy-static.h>
#include "../Common/utils.h"
#include "Bindy_helper.h"


// static data members init
bindy::Bindy *Bindy_helper::_pbindy = nullptr;
const char *Bindy_helper::_keyfile = nullptr;
bmap Bindy_helper::_map;
std::mutex Bindy_helper::_map_mutex;
std::mutex Bindy_helper::_global_mutex;

 
Bindy_helper Bindy_helper::_bhelper;

/*Bindy_helper::Bindy_helper(const char *keyfile)
{
	_keyfile = keyfile;
}
*/

bool Bindy_helper::set_keyfile(const char *keyfile)
{
	// not possible to change active keyfile once installed at bindy really init
    // if _pbindy is not init
	if (keyfile != nullptr && (_pbindy == nullptr || (_keyfile == nullptr || strcmp(_keyfile, keyfile) == 0))
	{
		_keyfile = keyfile;
		return true;
	}
	else
	{
		return false;
	}
}
  
bindy::Bindy *Bindy_helper::instance_bindy()
{
	if (_pbindy != nullptr)
		return _pbindy; // assumes old bindy is alive
	if (_keyfile == nullptr)
		return nullptr; // can't work without key set
	try {
	 	bindy::Bindy::initialize_network();

		_pbindy = new bindy::Bindy(_keyfile, false, false); // is_server == false, is_buffered == false
		_pbindy -> set_handler(&callback_data_bindy);
	}
	catch (...){
		_pbindy  = nullptr;
		return nullptr;
	}

	return _pbindy;

}

void Bindy_helper::shutdown_bindy()
{ 
	if (_pbindy == nullptr) return; // no initial-n done
	bindy::Bindy::shutdown_network(); 
	delete _pbindy; 
	_pbindy = nullptr; 
}

conn_id_t  Bindy_helper::connect(const char *addr, Xibridge_client *pcl, const char *adapter_addr)
{
	bindy::Bindy *pb = instance_bindy();
	conn_id_t conn = conn_id_invalid;
	if (pb == nullptr)
	{
		pcl -> _set_last_error(ERR_NO_BINDY);
		return conn_id_invalid;;
	}
	try {
	    conn = instance_bindy()->connect(addr/*, adapter_addr*/);

		if (conn != conn_id_invalid)
		{
			_map_mutex.lock();
			_map.insert(std::make_pair(conn, pcl));
			_map_mutex.unlock();
		}
	}
	catch (...)
	{
		ZF_LOGE("Catch exception at bindy connect, addr: %s", addr);
	}
	return conn;
}

void  Bindy_helper::disconnect(conn_id_t conn_id)
{
	bindy::Bindy *pb = instance_bindy();
	if (pb == nullptr || conn_id == conn_id_invalid) return;
	try {
		pb->disconnect(conn_id);

		_map_mutex.lock();
		if (_map.find(conn_id) != _map.cend())
			_map.erase(conn_id);
		_map_mutex.unlock();
	} 
	catch (...)
	{
		ZF_LOGE("Catch exception at bindy disconnect, conn_id: %u", conn_id);
	}
}

bool Bindy_helper::send_bindy_data(conn_id_t conn_id, bvector data)
{
	bindy::Bindy *pb = instance_bindy();
	try
	{
		_pbindy -> send_data(conn_id, data);
		return TRUE;
	}
	catch (...)
	{
		ZF_LOGE("Catch exception at bindy send_data, conn_id: %u", conn_id);
		return false;
	}
}

void Bindy_helper::callback_data_bindy(conn_id_t conn_id, bvector data) 
{
	_map_mutex.lock();
		
	if (_map.find(conn_id) != _map.cend())
	{
		Xibridge_client *pcl = _map.at(conn_id);
		pcl->_mutex_recv.lock();
		pcl->_recv_message.assign(data.cbegin(), data.cend());
		pcl->_is_really_recv = true;
		pcl->_is_recv.notify_all();
		pcl->_mutex_recv.unlock();
	}
	_map_mutex.unlock();
}

void Bindy_helper::on_bindy_disconnect(conn_id_t conn_id)
{
	_map_mutex.lock();

	if (_map.find(conn_id) != _map.cend())
	{
		Xibridge_client *pcl = _map.at(conn_id);
		pcl->_conn_id = conn_id_invalid;
		_map.erase(conn_id);
	}
	_map_mutex.unlock();
}

bool Bindy_helper::is_connected(conn_id_t conn_id) const
{
	bool ret;
	_map_mutex.lock();
	ret = _map.find(conn_id) != _map.cend();
	_map_mutex.unlock();
	return ret;
}