#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H
#include <vector>
#include <string>
#include "defs.h"
#include "utils.h"



/* * structure to connect packet type and command schema
 *   command schema ia a string like this "v_p_0_d_0_0_x"
 *   v - version, p - packet type, 0 - 32-bit zero, d - 32-bit non-zero, x - array bytes of any length,
 *   l - 32-bit length + byte array of thislength, b - 0 or 1 32-bit, u -any 32-bit numver
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
* ����� AProtocol
* ����������� �����, ������������ ����� �������� ��� ����������
* ���������� �� ������ �����������
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
		uint32 &serial);

	virtual bvector create_client_request(uint32 pckt, uint32 serial, uint32 tmout, const bvector *data = nullptr) = 0;
	virtual bvector create_open_request(uint32 serial, uint32 tmout) = 0;
	virtual bvector create_close_request(uint32 serial, uint32 tmout) = 0;
	virtual bvector create_version_request(uint32 tmout) { return bvector(); }
	virtual bvector create_enum_request(uint32 tmout) { return bvector(); }
	virtual bvector create_cmd_request(uint32 serial, uint32 tmout, const bvector *data = nullptr) = 0;

	virtual bool translate_response(uint32 pckt, const bvector& green) = 0;

	virtual uint32 version() = 0;
	virtual const cmd_schema *get_cmd_shema() = 0;  // pure virtual
protected:
	AProtocol(bool is_server): _is_server(is_server), _is_inv_pckt(false), _is_inv_pcktfmt(false) {};
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &green_data,
		bvector &grey_data,
		uint32 pckt) = 0;

	bool _is_server;

	bool _is_inv_pckt; // to send error packet to the other side

	bool _is_inv_pcktfmt; // to send error packet to the other side

	bool _is_device_broken; 

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
	Protocol1(bool isserver):AProtocol(isserver) {};
	virtual bvector create_client_request(uint32 pckt, uint32 serial, uint32 tmout, const bvector *data = nullptr);
	
	virtual bvector create_open_request(uint32 serial, uint32 /*tmout*/) 
	{
		return create_client_request(pkt1_open_req, serial, 0);
	};

	virtual bvector create_close_request(uint32 serial, uint32 tmout)
	{
		return create_client_request(pkt1_close_req, serial, 0);
	};

	virtual bvector create_enum_request(uint32 /*tmout*/)
	{
		return create_client_request(pkt1_enum_req, 0, 0);
	};

	virtual bvector create_cmd_request(uint32 serial, uint32 /*tmout*/, const bvector *data = nullptr)
	{
		return create_client_request(pkt1_raw, serial, 0, data);
	}
	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }
	virtual bool translate_response(uint32 pckt, const bvector& green) ;
protected:
	virtual uint32 version() { return 1; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &green_data,
		bvector &grey_data,
		uint32 pckt);
private:
	/**
	* Ver.1 protocol packets type
	*/
    enum Pkt_types1
	{
		pkt1_raw = 0x0,             // ��� ������ � ������ � ������� ���� � ��� ��
		pkt1_open_req = 0x1,
		pkt1_open_resp = 0xFF,
		pkt1_close_req = 2,
		pkt1_close_resp = 0xFE,
		pkt1_enum_req = 0x3,
		pkt1_enum_resp = 0xFD,
		pkt1_error_ntf = 0x4
	};

	static cmd_schema _cmd_shemas[9];
	
};

class Protocol2 : public AProtocol
{
public:
	Protocol2(bool isserver) :AProtocol(isserver) {};
	virtual bvector create_client_request(uint32 pckt, uint32 serial, uint32 tmout, const bvector *data = nullptr);
	virtual bvector create_open_request(uint32 serial, uint32 tmout)
	{
		return create_client_request(pkt2_open_req, serial, 0);
	};
	virtual bvector create_close_request(uint32 serial, uint32 tmout)
	{
		return create_client_request(pkt2_close_req, serial, 0);
	};

	virtual bvector create_cmd_request(uint32 serial, uint32 /*tmout*/, const bvector *data = nullptr)
	{
		return create_client_request(pkt2_cmd_req, serial, 0, data);
	}
	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }

	virtual bool translate_response(uint32 pckt, const bvector& green);
protected:
	
	virtual uint32 version() { return 2; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &green_data,
		bvector &grey_data,
		uint32 pckt);
	
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
};

class Protocol3 : public AProtocol
{
public:
	Protocol3(bool isserver) :AProtocol(isserver) {};
	virtual bvector create_client_request(uint32 pckt, uint32 serial, uint32 tmout, const bvector* data = nullptr);
	virtual bvector create_open_request(uint32 serial, uint32 tmout)
	{
		return create_client_request(pkt3_open_req, serial, tmout);
	};
	virtual bvector create_close_request(uint32 serial, uint32 tmout)
	{
		return create_client_request(pkt3_close_req, serial, tmout);
	};
	virtual bvector create_version_request(uint32 tmout)
	{
		return create_client_request(pkt3_ver_req, 0, tmout);
	}
    virtual bvector create_cmd_request(uint32 serial, uint32 tmout, const bvector *data = nullptr)
	{
		return create_client_request(pkt3_cmd_req, serial, tmout, data);
	}

	virtual bvector create_enum_request(uint32 tmout)
	{
		return create_client_request(pkt3_enum_req, 0, tmout);
	};

	virtual const cmd_schema *get_cmd_shema() { return _cmd_shemas; }

	virtual bool translate_response(uint32 pckt, const bvector& green);
protected:
	
	virtual uint32 version() { return 3; }
	virtual bool get_spec_data(MBuf&  mbuf,
		bvector &green_data,
		bvector &grey_data,
		uint32 pckt);

	
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



