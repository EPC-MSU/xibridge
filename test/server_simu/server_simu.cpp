#include <iostream>
#include "../common/defs.h"
#include <../common/protocols.h>
#include <bindy/bindy-static.h>
#include <zf_log.h>

bindy::Bindy * pb = NULL;

/*
* server simulation object
*/
class server_simu
{
public:
    server_simu(bindy::Bindy *pb) :
        _pbindy(pb)
    {
        _version = { 3, 0, 0 };
    }

    static uint32_t last_server_version() { return _version.major; }

private:
    bindy::Bindy *_pbindy;
    static xibridge_version_t _version;
};

xibridge_version_t server_simu::_version = xibridge_version_invalid;

/*
* Requset commands ids - needed for testing
*/
#define PROTO_1_OPEN  1
#define PROTO_1_CLOSE 2 
#define PROTO_1_RAW   0 
#define PROTO_1_ENUM  3

#define PROTO_2_OPEN  1
#define PROTO_2_CLOSE 2
#define PROTO_2_CMD   3

#define PROTO_3_OPEN  1
#define PROTO_3_CLOSE 2
#define PROTO_3_CMD   3
#define PROTO_3_VER   5
#define PROTO_3_ENUM  4

/*
* Requset commands ids - needed for testing
*/
#define PROTO_1_OPEN  1
#define PROTO_1_CLOSE 2 
#define PROTO_1_RAW   0 
#define PROTO_1_ENUM  3

#define PROTO_2_OPEN  1
#define PROTO_2_CLOSE 2
#define PROTO_2_CMD   3

#define PROTO_3_OPEN  1
#define PROTO_3_CLOSE 2
#define PROTO_3_CMD   3
#define PROTO_3_VER   5
#define PROTO_3_ENUM  4

#define PROTO_1_OPEN_RESP  -1
#define PROTO_1_CLOSE_RESP -2 
#define PROTO_1_RAW_RESP    0 
#define PROTO_1_ENUM_RESP  -3

#define PROTO_2_OPEN  1
#define PROTO_2_CLOSE 2
#define PROTO_2_CMD   3

#define PROTO_3_OPEN  1
#define PROTO_3_CLOSE 2
#define PROTO_3_CMD   3
#define PROTO_3_VER   5
#define PROTO_3_ENUM  4





// == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
void callback_data(conn_id_t conn_id, std::vector<uint8_t> data) 
{
    ZF_LOGD("From %u received packet of length: %lu.", conn_id, data.size());

    if (data.size() < 16)
    { // We need at least the protocol version and command code... and serial too
        ZF_LOGE("From %u received incorrect data packet: Size: %lu, expected at least 16.", conn_id, data.size());
        throw std::runtime_error("Incorrect data packet");
    }

    uint32_t protocol_ver = AProtocol::get_version_of_cmd(data);
    uint32_t command_code = AProtocol::get_pckt_of_cmd(data);
    
    //uint32_t serial;

    if (protocol_ver > server_simu::last_server_version())
    {
        ZF_LOGE("From %u received packet with not supported protocol version: %u.", conn_id, protocol_ver);
        return;
    }

    MBuf mbuf(data.data(), data.size());
    mbuf.mseek(sizeof(uint32_t)* 2); // skip version and packet - already read
    Hex32 ddev;
    HexIDev3 edev;
    uint32_t err_p;
    if (protocol_ver == 1)
    {
        Protocol1 p1(&err_p, true);
        switch (command_code)
        {
        case PROTO_1_RAW:
        {
                            ZF_LOGD("From %u received Protocol1 command request packet.", conn_id);
                            mbuf >> ddev;
                            const cmd_schema cmd_schema_simple = { PROTO_1_RAW, "v_p_0_d_0_0_0" };
                            bvector answer = cmd_schema_simple.gen_plain_command(command_code, protocol_ver, DevId(ddev), 0, 0);
                            pb->send_data(conn_id, answer);

                            break;
        }
        case PROTO_1_OPEN:
        {
                             ZF_LOGD("From %u received Protocol1 open device request packet.", conn_id);
                             mbuf >> ddev;
                             const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_1_OPEN_RESP, p1.get_cmd_shema());
                             bvector answer = cm1.gen_plain_command(command_code, protocol_ver, DevId(ddev), 1, 0);
                             pb->send_data(conn_id, answer);

                             break;
        }
        case PROTO_2_CLOSE:
        {
                              ZF_LOGD("From %u received Protocol1 open device request packet.", conn_id);
                              mbuf >> ddev;
                              const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_1_CLOSE_RESP, p1.get_cmd_shema());
                              bvector answer = cm1.gen_plain_command(command_code, protocol_ver, DevId(ddev), 1, 0);
                              pb->send_data(conn_id, answer);
                              break;

        }

        default:
        {
                   ZF_LOGD("Unknown packet code.");
                   break;
        }
        }
    }
}
// ========================================================

int initialization()
{
    WSADATA wsaData;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        ZF_LOGE("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    return 0;
}

void start_server()
{
    initialization();

    try{
        bindy::Bindy bindy("", true, false);
        pb = &bindy;
        bindy.connect();
        server_simu smu(&bindy);
        bindy.set_handler(&callback_data);

    }
    catch (std::exception &e)
    {
        ZF_LOGE("Exception while server simulator initializing! Details: %s", e.what());
    }
    
}

ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

void main()
{
    zf_log_set_output_level(ZF_LOG_DEBUG);
    start_server();
   
}
