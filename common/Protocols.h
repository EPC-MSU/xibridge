#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H
#include <vector>
#include <string>
#include "ext_dev_id.h"
#include "defs.h"
#include "utils.h"


/*
* C++ struct to keep device identifiers of 2 types
*/
struct DevId {
	DevId(uint32 id) :
	_dev_id(id),
	_is_new(false)
	{};

	DevId(const ExtDevId &e_id) :
		_dev_id_new(e_id),
		_is_new(true)
	{};

	DevId(bool is_n = false) : _is_new(is_n){};
	uint32 _dev_id;
		
	ExtDevId _dev_id_new;

	bool _is_new;
};

  
/* * structure to connect packet type and command schema
 *   command schema ia a string like this "v_p_0_d_0_0_x"
 *   v - version, p - packet type, 0 - 32-bit zero, d - 32-bit non-zero, x - array bytes of any length,
 *   l - 32-bit length + byte array of thislength, b - 0 or 1 32-bit, u -any 32-bit number, I - extended device identifier
*/
typedef struct _sm
{
	uint32 pkt_type;
	const char * schema;
	/* *
	   * Checks if data matches the schema 
	
	*/
	bool is_match(const uint8 *data, int len, uint32 proto, uint32 dev_num) const;
	static const struct _sm &get_schema(uint32 pckt, const struct _sm * _ss);

} cmd_schema;


/**
* \russian
*  ласс AProtocol
* јбстрактный класс, представл€ет собой протокол без конкретики
* Ёкземпл€ры не должны создаватьс€
  \endrussian
*/
class AProtocol
{
public:
	/*
	 * Prepares protocol formatted data FROM Bindy callback into some separated arrays and fields
	 * Gets protocol results data (green in Wiki) and device  data (light blue in Wiki), packet type and serial of the device (at server)
	 * @param [out] green_data - data could be interpritited by any of the protocols
	 * @param [out] grey_data - data direct from a device
	 * @param [out] pckt_type - packet type in terms of the protocol
	 * @param [out] serial - serial port or slot number of the device at server side
	 */
	bool get_data_from_bindy_callback(MBuf &cmd,
		bvector &green_data,
		bvector &grey_data,
		uint32 &pckt_type,
		DevId & devid);

	static uint32 get_version_of_cmd(const bvector& cmd) { return (uint32)(cmd.size() > 3 ? cmd[3] : 0); }

	virtual bvector create_client_request(uint32 pckt, DevId devid, uint32 tmout, const bvector *data = nullptr) = 0;
	virtual bvector create_open_request(DevId devid, uint32 tmout) = 0;
	virtual bvector create_close_request(DevId devid, uint32 tmout) = 0;
	virtual bvector create_version_request(uint32 tmout) { return bvector(); }
	virtual bvector create_enum_request(uint32 tmout) { return bvector(); }
	virtual bvector create_cmd_request(DevId devid, uint32 tmout, const bvector *data = nullptr, uint32 resp_length = 0);
	
	virtual bool translate_response(uint32 pckt, const bvector& green) = 0;

	virtual uint32 version() = 0;
	virtual const cmd_schema *get_cmd_shema() = 0;  // pure virtual
protected:
	AProtocol(bool is_server): _is_server(is_server), _is_inv_pckt(false), _is_inv_pcktfmt(false) {};
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &green_data,
		bvector &grey_data,
		uint32 pckt) = 0;

	virtual bvector create_cmd_req_proxy(DevId devid, uint32 tmout, const bvector & data) = 0;
	
	bool _is_server;

	bool _is_inv_pckt; // to send error packet to the other side

	bool _is_inv_pcktfmt; // to send error packet to the other side

	bool _is_device_broken; 

};

/**
* \russian
*  ласс Protocol1
* ѕредставл€ет конкретный протокол версии 1, исп. в ximc-сервере
* 
* \endrussian
*/
class Protocol1 : public AProtocol
{
public:
	Protocol1(bool isserver):AProtocol(isserver) {};
    virtual bvector create_client_request(uint32 pckt, DevId devid, uint32 tmout, const bvector *data = nullptr)
    {
        if (devid._is_new) return bvector();
        return create_client_request(pckt, devid._dev_id, tmout, data);
    }
	
	virtual bvector create_open_request(DevId devid, uint32 /*tmout*/)
	{
		return create_client_request(pkt1_open_req, devid, 0);
	};

	virtual bvector create_close_request(DevId devid, uint32 tmout)
	{
		return create_client_request(pkt1_close_req, devid, 0);
	};

	virtual bvector create_enum_request(uint32 /*tmout*/)
	{
		return create_client_request(pkt1_enum_req, DevId(), 0);
	};

	
	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }
	virtual bool translate_response(uint32 pckt, const bvector& green) ;
protected:
	virtual uint32 version() { return 1; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &green_data,
		bvector &grey_data,
		uint32 pckt);
	virtual bvector create_cmd_req_proxy(DevId devid, uint32 /*tmout*/, const bvector &data)
	{
		return create_client_request(pkt1_raw, devid, 0, &data);
	}
private:
	/**
	* Ver.1 protocol packets type
	*/
    enum Pkt_types1
	{
		pkt1_raw = 0x0,             // тип пакета в ответе и запросе один и тот же
		pkt1_open_req = 0x1,
		pkt1_open_resp = 0xFF,
		pkt1_close_req = 2,
		pkt1_close_resp = 0xFE,
		pkt1_enum_req = 0x3,
		pkt1_enum_resp = 0xFD,
		pkt1_error_ntf = 0x4
	};

	static cmd_schema _cmd_shemas[9];

    bvector create_client_request(uint32 pckt, uint32 serial, uint32 tmout, const bvector *data);
};

class Protocol2 : public AProtocol
{
public:
	Protocol2(bool isserver) :AProtocol(isserver) {};
	virtual bvector create_client_request(uint32 pckt, DevId devid, uint32 tmout, const bvector *data = nullptr)
    {
        if (devid._is_new) return bvector();
        return create_client_request(pckt, devid._dev_id, tmout, data);
    }
	
    virtual bvector create_open_request(DevId devid, uint32 tmout)
	{
		return create_client_request(pkt2_open_req, devid, 0);
	};
	virtual bvector create_close_request(DevId devid, uint32 tmout)
	{
		return create_client_request(pkt2_close_req, devid, 0);
	};
	
	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }

	virtual bool translate_response(uint32 pckt, const bvector& green);
protected:
	
	virtual uint32 version() { return 2; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &green_data,
		bvector &grey_data,
		uint32 pckt);
	virtual bvector create_cmd_req_proxy(DevId devid, uint32 /*tmout*/, const bvector &data)
	{
		return create_client_request(pkt2_cmd_req, devid, 0, &data);
	}
private:
	/**
	* Ver.2 protocol packets type
	*/
	enum Pkt_types2
	{
		pkt2_cmd_req = 0x3,
		pkt2_cmd_resp = 0xFD,
		pkt2_open_req = 0x1,
		pkt2_open_resp = 0xFF,
		pkt2_close_req = 2,
		pkt2_close_resp = 0xFE,

	};

	static cmd_schema _cmd_shemas[7];

    virtual bvector create_client_request(uint32 pckt, uint32 serial, uint32 tmout, const bvector *data);
};

class Protocol3 : public AProtocol
{
public:
	Protocol3(bool isserver) :AProtocol(isserver) {};
	virtual bvector create_client_request(uint32 pckt, DevId devid, uint32 tmout, const bvector* data = nullptr);
	virtual bvector create_open_request(DevId devid, uint32 tmout)
	{
		return create_client_request(pkt3_open_req, devid, tmout);
	};
	virtual bvector create_close_request(DevId devid, uint32 tmout)
	{
		return create_client_request(pkt3_close_req, devid, tmout);
	};
	virtual bvector create_version_request(uint32 tmout)
	{
		return create_client_request(pkt3_ver_req, DevId(true), tmout);
	}
	
	virtual bvector create_enum_request(uint32 tmout)
	{
		return create_client_request(pkt3_enum_req, DevId(true), tmout);
	};

	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }

	virtual bool translate_response(uint32 pckt, const bvector& green);

    virtual bvector create_cmd_request(DevId devid, uint32 tmout, const bvector *data = nullptr, uint32 resp_length = 0);
    

protected:
	
	virtual uint32 version() { return 3; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &green_data,
		bvector &grey_data,
		uint32 pckt);
	virtual bvector create_cmd_req_proxy(DevId devid, uint32 /*tmout*/, const bvector &data)
	{
		return create_client_request(pkt3_cmd_req, devid, 0, &data);
	}
	
private:
	/**
	* Ver.3 protocol packets type
	*/
	enum Pkt_types3
	{
		pkt3_ver_req = 0x5,
		pkt3_ver_resp = 0xFB,
		pkt3_cmd_req = 0x3,
		pkt3_cmd_resp = 0xFD,
		pkt3_open_req = 0x1,
		pkt3_open_resp = 0xFF,
		pkt3_close_req = 2,
		pkt3_close_resp = 0xFE,
		pkt3_enum_req = 0x4,
		pkt3_enum_resp = 0xFC,
		pkt3_error_resp = 0xFA
	};

   
	static cmd_schema _cmd_shemas[12];
	
};


/*
* Virtual constructor
*/
extern AProtocol *create_appropriate_protocol(int version_number);

#endif



