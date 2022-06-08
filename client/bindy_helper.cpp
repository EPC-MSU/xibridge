#include <zf_log.h>
#include "../common/defs.h"
#include <bindy/bindy-static.h>
#include "../common/utils.h"
#include "bindy_helper.h"

//const data
#define XINET_BINDY_USER "root-user"
const bindy::aes_key_t Bindy_helper::_xinet_bindy_key = { 32, 87, 139, 134, 41, 227, 202, 19, 235, 29, 48, 119, 189, 61, 211, 135 };


// static data members init
bindy::Bindy *Bindy_helper::_pbindy = nullptr;
bmap Bindy_helper::_map;
std::mutex Bindy_helper::_map_mutex;
std::mutex Bindy_helper::_global_mutex;

 
Bindy_helper Bindy_helper::_bhelper;
  
bindy::Bindy *Bindy_helper::instance_bindy()
{
    std::unique_lock<std::mutex> lock(Bindy_helper::_global_mutex);
	if (_pbindy != nullptr)
		return _pbindy; // assumes old bindy is alive
	try {
	 	bindy::Bindy::initialize_network();
        _pbindy = new bindy::Bindy("", false, false); // is_server == false, is_buffered == false
        // HACK: we assume that the server has such user as master set - add it to in-memory keyfile
        
		bindy::user_id_t uid{ XINET_BINDY_USER };
	    _pbindy->add_user_local(XINET_BINDY_USER, _xinet_bindy_key, uid);
        _pbindy -> set_master_local(uid);
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

conn_id_t  Bindy_helper::connect(Xibridge_client *pcl)
{
	bindy::Bindy *pb = instance_bindy();
	conn_id_t conn = conn_id_invalid;
	if (pb == nullptr)
	{
		pcl -> _set_last_error(ERR_NO_BINDY);
		return conn_id_invalid;;
	}
	try {
	    conn = instance_bindy()->connect(pcl -> _host/*, pcl -> _adapter*/);

		if (conn != conn_id_invalid)
		{
			_map_mutex.lock();
			_map.insert(std::make_pair(conn, pcl));
			_map_mutex.unlock();
		}
	}
	catch (std::exception &ex)
	{
		
		pcl->_set_last_error(ERR_SET_CONNECTION);
		//pcl->_set_add_err_text(ex.what());
		ZF_LOGE("Catch exception at bindy connect, addr: %s, text: %s", pcl->_host, ex.what());
	}
	catch (...)
	{
		pcl->_set_last_error(ERR_SET_CONNECTION);
		ZF_LOGE("Catch exception at bindy connect, addr: %s.", pcl->_host);
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
	catch (std::exception &ex)
	{
		ZF_LOGE("Catch exception at bindy disconnect, conn_id: %u, text: %s", conn_id, ex.what());
	}
	catch (...)
	{
		ZF_LOGE("Catch exception at bindy disconnect, conn_id: %u", conn_id);
	}
}

bool Bindy_helper::send_bindy_data(conn_id_t conn_id, bvector data)
{
	if (!is_connected(conn_id))
	{
		return false;
	}
	bindy::Bindy *pb = instance_bindy();
	try
	{
		_pbindy -> send_data(conn_id, data);
		return true;
	}
	catch (std::exception &ex)
	{
		ZF_LOGE("Catch exception at bindy send_data, conn_id: %u, text: %s", conn_id, ex.what());
        return false;
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