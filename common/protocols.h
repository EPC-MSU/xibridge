#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H
#include "defs.h"
#include "../xibridge.h"
#include "utils.h"

/*
    * class to keep device identifiers with implicit constructors
*/
class DevId {
public:
    
    DevId(const xibridge_device_t &e_id) :
        _dev_id(e_id)
    {
    };

    DevId(
        uint32_t id = 0,
        uint16_t pid = 0,
        uint16_t vid = 0,
        uint32_t reserve = 0)
    {
        _dev_id.id = id;
        _dev_id.PID = pid;
        _dev_id.VID = vid;
        _dev_id.reserve = reserve;
    }

    bool operator == (const DevId& devid)
    {
        return _dev_id.id == devid._dev_id.id && _dev_id.PID == devid._dev_id.PID
            && _dev_id.VID == devid._dev_id.VID && _dev_id.reserve == devid._dev_id.reserve;
    }

    uint32_t id() const 
    { 
        return _dev_id.id; 
    }

    uint32_t PID() const
    {
        return _dev_id.PID;
    }

    uint32_t VID() const
    {
        return _dev_id.VID;
    }

    uint32_t reserve() const
    {
        return _dev_id.reserve;
    }

    xibridge_device_t to_xibridge_device_t()  const
    {
        return _dev_id;
    }

private:

    xibridge_device_t _dev_id;
};

/* 
    * structure to connect packet type and command schema
    * command schema is a string like this "v_p_0_d_0_0_x"
    * v - version, p - packet type, 0 - 32-bit zero, d - 32-bit non-zero, x - array bytes of any length,
    * l - 32-bit length + byte array of thislength, b - 0 or 1 32-bit, u -any 32-bit number, I - extended device identifier
*/
typedef struct _sm_t
{
    uint32_t pkt_type;
    const char * schema;
/* 
    * Checks if data matches the schema 
*/
    bool is_match(
        const uint8_t *data, 
        int len,
        uint32_t proto, 
        const DevId& devid
    ) const;
/*
    * Gets command data length according to the schema when command needs no data
*/
    int get_plain_command_length() const;

/*
  * Generates byte vector data according to the schema when command needs no data
*/
    bvector gen_plain_command(
        uint32_t proto, 
        const DevId &pdev, 
        uint32_t zero_one, 
        uint32_t some
    ) const;

    static const struct _sm_t &get_schema(
                            uint32_t pckt, 
                            const struct _sm_t * _ss
                        );

} cmd_schema_t;


/*
    * Abstruct class to provide using of several types of protocols  
*/
class MBuf;
class AProtocol
{
public:
    virtual bool is_device_id_extended() = 0;
/*
    * Prepares protocol formatted data FROM Bindy callback into some separated arrays and fields
    * Gets protocol results data (green in Wiki) and device  data (light blue in Wiki), packet type and serial of the device (at server)
    * In case of enumeration response data will contain array of DevId-s
    * @param [out] res_data - data could be interpritited by any of the protocols
    * @param [out] data - data direct from a device
    * @param [out] pckt_type - packet type in terms of the protocol
    * @param [out] devid - device identifier of the device at server side
*/
    bool get_data_from_bindy_callback(
        MBuf &cmd,
        bvector &res_data,
        bvector &data,
        uint32_t &pckt_type
    );

    static uint32_t get_version_of_cmd(const bvector& cmd)
    { 
        return (uint32_t)(cmd.size() > 3 ? cmd[3] : 0); 
    }

    static uint32_t get_pckt_of_cmd(const bvector& cmd);
    
    virtual bvector create_client_request(
                uint32_t pckt, 
                const DevId &devid, 
                uint32_t tmout, 
                const bvector *data = nullptr
            ) = 0;

    virtual bvector create_open_request(
                const DevId &devid, 
                uint32_t tmout
            ) = 0;
    virtual bvector create_close_request(
                const DevId &devid, 
                uint32_t tmout
            ) = 0;
    virtual bvector create_version_request(uint32_t /*tmout*/) 
    { return bvector(); }

    virtual bvector create_enum_request(uint32_t /*tmout*/) 
    { return bvector(); }
    
    virtual bvector create_cmd_request(
                const DevId &devid, 
                uint32_t tmout, 
                const bvector *data = nullptr, 
                uint32_t resp_length = 0
            ) = 0;
    
    virtual bool translate_response(
                uint32_t pckt, 
                const bvector& res_data
            ) = 0;

    virtual uint32_t version() = 0;

/*
    *command schema is a string like this "v_p_0_d_0_0_x"
    * v - version, p - packet type, 0 - 32 - bit zero, d - 32 - bit non - zero, x - array bytes of any length,
    *l - 32 - bit length + byte array of thislength, b - 0 or 1 32 - bit, u - any 32 - bit number, I - extended device identifier
*/
    virtual const cmd_schema_t *get_cmd_schema() = 0;  // pure virtual
    uint32_t get_result_error() const 
    { return _res_err; }
protected:
    AProtocol(
        uint32_t *perror, 
        bool is_server
    ): 
    _is_server(is_server), _perror(perror) {};
    virtual bool get_spec_data(
                MBuf&  mbuf,
                bvector &res_data,
                bvector &data,
                uint32_t pckt
            ) = 0;

    bool _is_server;
        
    uint32_t _res_err;   // at response stage the result of an operation or error from other side

    uint32_t  * _perror; // pointer to error field to write error code
};

/*
    * \english
    * Class Protocol1 
    * Operates data according to version 1 protocol (ximc-xinet-server) 
    * \endenglish
    * \russian
    * Класс Protocol1
    * Для создания-обработки сообщений по протоколу версии 1(ximc-сервер)
    * \endrussian
*/
class Protocol1 : public AProtocol
{
public:
    Protocol1(
        uint32_t * perror, 
        bool isserver
    ):
    AProtocol(perror, isserver) {};
    virtual bool is_device_id_extended() 
    {
        return false;
    }

    virtual bvector create_client_request(
                uint32_t pckt, 
                const DevId &devid, 
                uint32_t tmout, 
                const bvector *data = nullptr
    )
    {
        return create_client_request(pckt, devid.id(), tmout, data);
    }
    
    virtual bvector create_open_request(
                const DevId &devid, 
                uint32_t /*tmout*/
    )
    {
        return create_client_request(pkt1_open_req, devid, 0);
    }

    virtual bvector create_close_request(
                const DevId &devid, 
                uint32_t /*tmout*/
    )
    {
        return create_client_request(pkt1_close_req, devid, 0);
    }

    virtual bvector create_enum_request(uint32_t /*tmout*/)
    {
        return create_client_request(pkt1_enum_req, DevId(0), 0);
    }

    virtual bvector create_cmd_request(
                const DevId &devid, 
                uint32_t tmout, 
                const bvector *data = nullptr, 
                uint32_t resp_length = 0
    );
    virtual const cmd_schema_t *get_cmd_schema() 
    { return _cmd_schemas; }
    virtual bool translate_response(
                uint32_t pckt, 
                const bvector& res_data
            );
protected:
    virtual uint32_t version() 
    { return 1; }
    virtual bool get_spec_data(
                MBuf&  mbuf,
                bvector &res_data,
                bvector &data,
                uint32_t pckt
            );

private:
/*
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

    static cmd_schema_t _cmd_schemas[9];
    bvector create_client_request(
        uint32_t pckt, 
        uint32_t serial, 
        uint32_t tmout, 
        const bvector *data
    );
};

/*
    * \english
    * Class Protocol1 
    * Operates data according to version 2 protocol (urpc-xinet-server) 
    * \endenglish
    * \russian
    * Класс Protocol2
    * Для создания-обработки сообщений по протоколу версии 2(urpc-сервер)
    * \endrussian
*/
class Protocol2 : public AProtocol
{
public:
    Protocol2(
        uint32_t * perror, 
        bool isserver
    ):
    AProtocol(perror, isserver) {};
    virtual bool is_device_id_extended() 
    {
        return false;
    };

     virtual bvector create_client_request(
                uint32_t pckt, 
                const DevId &devid, 
                uint32_t tmout, 
                const bvector *data = nullptr
            )
    {
        return create_client_request(pckt, devid.id(), tmout, data);
    }
    
    virtual bvector create_open_request(
                const DevId &devid, 
                uint32_t /*tmout*/
            )
    {
        return create_client_request(pkt2_open_req, devid, 0);
    };

    virtual bvector create_close_request(
                const DevId &devid, 
                uint32_t /*tmout*/
            )
    {
        return create_client_request(pkt2_close_req, devid, 0);
    };
    
    virtual const cmd_schema_t *get_cmd_schema() 
    { return _cmd_schemas; }

    virtual bool translate_response(
                uint32_t pckt, 
                const bvector& res_data
    );

    virtual bvector create_cmd_request(
                const DevId &devid, 
                uint32_t tmout, 
                const bvector *data = nullptr, 
                uint32_t resp_length = 0
            );
    
    bool get_data_from_request(MBuf &cmd,
        bvector &req_data,
        DevId &dev_id,
        uint32_t &resp_len);


    bvector create_cmd_response(
        uint32_t val,
        const DevId &devid,
        const bvector *data
        )
    {
        return create_server_response(pkt2_cmd_resp, val, &devid, data);
    }

    bvector create_open_response(
        const DevId &devid,
        uint32_t bool_val
        )
    {
        return create_server_response(pkt2_open_resp, bool_val, &devid);
    }

    bvector create_close_response(
        const DevId &devid,
        uint32_t bool_val
        )
    {
        return create_server_response(pkt2_close_resp, bool_val, &devid);
    }
protected:
    
    virtual uint32_t version() 
    { return 2; }
    virtual bool get_spec_data(
                MBuf&  mbuf,
                bvector &res_data,
                bvector &data,
                uint32_t pckt
            );
    
private:
/*
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

    static cmd_schema_t _cmd_schemas[7];
    static const int URPC_CID_SIZE; //  = 4 
   
    bvector create_client_request(
        uint32_t pckt,
        uint32_t serial,
        uint32_t tmout,
        const bvector *data
        );

    bvector create_server_response(
        uint32_t pckt,
        uint32_t bool_val = 0,
        const DevId *pdevid = nullptr,
        const bvector* pdata = nullptr
        );
};

/*
    * \english
    * Class Protocol3 
    * Operates data according to version 1 protocol (xibridge-xinet-server) 
    * \endenglish
    * \russian
    * Класс Protocol3
    * Для создания-обработки сообщений по протоколу версии 3(xibridge-сервер)
    * \endrussian
*/
class Protocol3 : public AProtocol
{
public:
    Protocol3(
        uint32_t * perror, 
        bool isserver
    ):
    AProtocol(perror, isserver) {};
    virtual bool is_device_id_extended() 
    {
        return true;
    }

    virtual bvector create_client_request(
                uint32_t pckt, 
                const DevId &devid, 
                uint32_t tmout, 
                const bvector* data = nullptr
            );
    virtual bvector create_open_request(
                const DevId &devid, 
                uint32_t /*tmout*/
            )
    {
        return create_client_request(pkt3_open_req, devid, 0);
    }

    virtual bvector create_close_request(
                const DevId &devid, 
                uint32_t /*tmout*/
            )
    {
        return create_client_request(pkt3_close_req, devid, 0);
    }

    virtual bvector create_version_request(uint32_t /*tmout*/)
    {
        return create_client_request(pkt3_ver_req, DevId(), 0);
    }
    
    virtual bvector create_enum_request(uint32_t /*tmout*/)
    {
        return create_client_request(pkt3_enum_req, DevId(), 0);
    };

    virtual const cmd_schema_t *get_cmd_schema() 
    { return _cmd_schemas; }

    virtual bool translate_response(
                uint32_t pckt, 
                const bvector& res_data
            );

    virtual bvector create_cmd_request(
                const DevId &devid, 
                uint32_t tmout, 
                const bvector *data = nullptr, 
                uint32_t resp_length = 0
            );

    bool get_data_from_request(MBuf &cmd,
        bvector &req_data,
        DevId &dev_id,
        uint32_t &resp_len);

   
    bvector create_cmd_response(
        const DevId &devid,
        const bvector *data
        )
    {
        return create_server_response(pkt3_cmd_resp, 0, &devid, data);
    }

    bvector create_open_response(
        const DevId &devid,
        uint32_t bool_val
        )
    {
        return create_server_response(pkt3_open_resp, bool_val, &devid);
    }

    bvector create_version_response()
    {
        return create_server_response(pkt3_ver_resp);
    }

    bvector create_close_response(
        const DevId &devid,
        uint32_t bool_val
        )
    {
        return create_server_response(pkt3_close_resp, bool_val, &devid);
    }

    bvector create_error_response(uint32_t val)
    {
        return create_server_response(pkt3_error_resp, val);
    }
 
protected:
    virtual uint32_t version() 
    { return 3; }
    virtual bool get_spec_data(
                MBuf&  mbuf,
                bvector &res_data,
                bvector &data,
                uint32_t pckt
            );
        
private:
/*
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
   
    static cmd_schema_t _cmd_schemas[12];

    bvector create_server_response(
        uint32_t pckt,
        uint32_t bool_val = 0,
        const DevId *pdevid = nullptr,
        const bvector* pdata = nullptr
        );
};

/*
    * Virtual protocol constructor
*/
extern AProtocol *create_appropriate_protocol(
           uint32_t version_number, 
           uint32_t *perror
       );

#endif
