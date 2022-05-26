#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H
#include "defs.h"
#include "../client/xibridge.h"
#include "utils.h"


/**
    * C++ struct to keep device identifiers with implicit constructors
*/
struct DevId {
	
	DevId(const xibridge_device_t &e_id) :
		_dev_id(e_id)
	{
	};

    DevId(uint32_t just_id = 0)
	{
		_dev_id.id = just_id;
		_dev_id.PID = _dev_id.VID = 0;
		_dev_id.reserve = 0;
	}

	xibridge_device_t _dev_id;
};

  
/** 
    * structure to connect packet type and command schema
    * command schema ia a string like this "v_p_0_d_0_0_x"
    * v - version, p - packet type, 0 - 32-bit zero, d - 32-bit non-zero, x - array bytes of any length,
    * l - 32-bit length + byte array of thislength, b - 0 or 1 32-bit, u -any 32-bit number, I - extended device identifier
*/
typedef struct _sm
{
	uint32_t pkt_type;
	const char * schema;
	/* *
	   * Checks if data matches the schema 
	
	*/
	bool is_match(const uint8_t *data, int len, uint32_t proto, uint32_t dev_num) const;
	static const struct _sm &get_schema(uint32_t pckt, const struct _sm * _ss);

} cmd_schema;


/**
* \russian
* Абстракный класс для поддержки различных протоколов обмена
  \endrussian
*/
class AProtocol
{
public:
	virtual bool is_device_id_extended() = 0;
/**
	 * Prepares protocol formatted data FROM Bindy callback into some separated arrays and fields
	 * Gets protocol results data (green in Wiki) and device  data (light blue in Wiki), packet type and serial of the device (at server)
     * In case of enumeration response data will contain array of DevId-s
	 * @param [out] res_data - data could be interpritited by any of the protocols
	 * @param [out] data - data direct from a device
	 * @param [out] pckt_type - packet type in terms of the protocol
	 * @param [out] devid - device identifier of the device at server side
*/
	bool get_data_from_bindy_callback(MBuf &cmd,
		bvector &res_data,
		bvector &data,
		uint32_t &pckt_type);

	static uint32_t get_version_of_cmd(const bvector& cmd) { return (uint32_t)(cmd.size() > 3 ? cmd[3] : 0); }

	virtual bvector create_client_request(uint32_t pckt, DevId devid, uint32_t tmout, const bvector *data = nullptr) = 0;
	virtual bvector create_open_request(DevId devid, uint32_t tmout) = 0;
	virtual bvector create_close_request(DevId devid, uint32_t tmout) = 0;
	virtual bvector create_version_request(uint32_t tmout) { return bvector(); }
	virtual bvector create_enum_request(uint32_t tmout) { return bvector(); }
	virtual bvector create_cmd_request(DevId devid, uint32_t tmout, const bvector *data = nullptr, uint32_t resp_length = 0) = 0;
	
	virtual bool translate_response(uint32_t pckt, const bvector& res_data) = 0;

	virtual uint32_t version() = 0;
	virtual const cmd_schema *get_cmd_shema() = 0;  // pure virtual
    uint32_t get_result_error() const { return _res_err; }
protected:
	AProtocol(uint32_t *perror, bool is_server): _is_server(is_server), _perror(perror) {};
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &res_data,
		bvector &data,
		uint32_t pckt) = 0;

	//virtual bvector create_cmd_req_proxy(DevId devid, uint32_t tmout, const bvector & data) = 0;
	
	bool _is_server;
		
    uint32_t _res_err;   //at response stage the result of an operation or error from other side

	uint32_t  * _perror; // pointer to error field to write error code

};

/**
* \russian
* ����� Protocol1
* ������������ ���������� �������� ������ 1, ���. � ximc-�������
* 
* \endrussian
*/
class Protocol1 : public AProtocol
{
public:
	Protocol1(uint32_t * perror, bool isserver):AProtocol(perror, isserver) {};
	virtual bool is_device_id_extended() {
		return false;
	};

	virtual bvector create_client_request(uint32_t pckt, DevId devid, uint32_t tmout, const bvector *data = nullptr)
    {
        return create_client_request(pckt, devid._dev_id.id, tmout, data);
    }
	
	virtual bvector create_open_request(DevId devid, uint32_t /*tmout*/)
	{
		return create_client_request(pkt1_open_req, devid, 0);
	};

	virtual bvector create_close_request(DevId devid, uint32_t tmout)
	{
		return create_client_request(pkt1_close_req, devid, 0);
	};

	virtual bvector create_enum_request(uint32_t /*tmout*/)
	{
		return create_client_request(pkt1_enum_req, DevId(0), 0);
	};

    virtual bvector create_cmd_request(DevId devid, uint32_t tmout, const bvector *data = nullptr, uint32_t resp_length = 0);
	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }
	virtual bool translate_response(uint32_t pckt, const bvector& res_data) ;
protected:
	virtual uint32_t version() { return 1; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &res_data,
		bvector &data,
		uint32_t pckt);

private:
	/**
	* Ver.1 protocol packets type
	*/
    enum Pkt_types1
	{
		pkt1_raw = 0x0,             // пакеты протокола 1
		pkt1_open_req = 0x1,
		pkt1_open_resp = 0xFF,
		pkt1_close_req = 2,
		pkt1_close_resp = 0xFE,
		pkt1_enum_req = 0x3,
		pkt1_enum_resp = 0xFD,
		pkt1_error_ntf = 0x4
	};

	static cmd_schema _cmd_shemas[9];

    bvector create_client_request(uint32_t pckt, uint32_t serial, uint32_t tmout, const bvector *data);
};

class Protocol2 : public AProtocol
{
public:
	Protocol2(uint32_t * perror, bool isserver) :AProtocol(perror, isserver) {};
	virtual bool is_device_id_extended() {
		return false;
	};
	virtual bvector create_client_request(uint32_t pckt, DevId devid, uint32_t tmout, const bvector *data = nullptr)
    {
        return create_client_request(pckt, devid._dev_id.id, tmout, data);
    }
	
    virtual bvector create_open_request(DevId devid, uint32_t tmout)
	{
		return create_client_request(pkt2_open_req, devid, 0);
	};
	virtual bvector create_close_request(DevId devid, uint32_t tmout)
	{
		return create_client_request(pkt2_close_req, devid, 0);
	};
	
	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }

	virtual bool translate_response(uint32_t pckt, const bvector& res_data);

    virtual bvector create_cmd_request(DevId devid, uint32_t tmout, const bvector *data = nullptr, uint32_t resp_length = 0);
protected:
	
	virtual uint32_t version() { return 2; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &res_data,
		bvector &data,
		uint32_t pckt);
	
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
    const int URPC_CID_SIZE = 4;
    virtual bvector create_client_request(uint32_t pckt, uint32_t serial, uint32_t tmout, const bvector *data);
};

class Protocol3 : public AProtocol
{
public:
	Protocol3(uint32_t * perror, bool isserver) :AProtocol(perror, isserver) {};
	virtual bool is_device_id_extended() {
		return true;
	};
	virtual bvector create_client_request(uint32_t pckt, DevId devid, uint32_t tmout, const bvector* data = nullptr);
	virtual bvector create_open_request(DevId devid, uint32_t tmout)
	{
		return create_client_request(pkt3_open_req, devid, tmout);
	};
	virtual bvector create_close_request(DevId devid, uint32_t tmout)
	{
		return create_client_request(pkt3_close_req, devid, tmout);
	};
	virtual bvector create_version_request(uint32_t tmout)
	{
		return create_client_request(pkt3_ver_req, DevId(), tmout);
	}
	
	virtual bvector create_enum_request(uint32_t tmout)
	{
		return create_client_request(pkt3_enum_req, DevId(), tmout);
	};

	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }

	virtual bool translate_response(uint32_t pckt, const bvector& res_data);

    virtual bvector create_cmd_request(DevId devid, uint32_t tmout, const bvector *data = nullptr, uint32_t resp_length = 0);
    

protected:
	
	virtual uint32_t version() { return 3; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &res_data,
		bvector &data,
		uint32_t pckt);
		
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
extern AProtocol *create_appropriate_protocol(int version_number, uint32_t *perror);

#endif



