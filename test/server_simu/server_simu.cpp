﻿#include <iostream>
#include "../common/defs.h"
#include <../common/protocols.h>
#include <bindy/bindy-static.h>
#include <zf_log.h>

/*
* Request-response commands ids - needed for testing
*/
#define PROTO_1_OPEN  1
#define PROTO_1_CLOSE 2 
#define PROTO_1_RAW   0 
#define PROTO_1_ENUM  3

#define PROTO_1_OPEN_RESP  255
#define PROTO_1_CLOSE_RESP 254 
#define PROTO_1_RAW_RESP    0 
#define PROTO_1_ENUM_RESP  253

#define PROTO_2_OPEN  1
#define PROTO_2_CLOSE 2
#define PROTO_2_CMD   3

#define PROTO_2_OPEN_RESP  255
#define PROTO_2_CLOSE_RESP 254
#define PROTO_2_CMD_RESP   253

#define PROTO_3_OPEN  1
#define PROTO_3_CLOSE 2
#define PROTO_3_CMD   3
#define PROTO_3_ENUM  4
#define PROTO_3_VER   5

#define PROTO_3_OPEN_RESP  255
#define PROTO_3_CLOSE_RESP 254
#define PROTO_3_CMD_RESP   253
#define PROTO_3_ENUM_RESP  252
#define PROTO_3_VER_RESP   251

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
    // to do - use DevId
    static uint32_t serials[];

    static bool is_serial_ok(uint32_t serial);

private:
    bindy::Bindy *_pbindy;
    static xibridge_version_t _version;
};

uint32_t server_simu::serials[] = { 3, 7, 12 };

xibridge_version_t server_simu::_version = xibridge_version_invalid;

bool server_simu::is_serial_ok(uint32_t serial)
{
    for (auto _serial : serials)
    {
        if ( _serial == serial) return true;
    }
    return false;
}

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
    Hex32 z, ddev;
    HexIDev3 edev;
    uint32_t err_p;
    if (protocol_ver == 1)
    {
        Protocol1 p1(&err_p, true);
        switch (command_code)
        {
        case PROTO_1_ENUM:
        {
                             ZF_LOGD("From %u received Protocol1 enum request packet.", conn_id);
                             // according to simple scenario - sending 3 serials
                             const cmd_schema cmd_schema_simple = { 0, "v_p_0_u" };
                             int len = cmd_schema_simple.get_plain_command_length();
                             uint32_t nd = (uint32_t)sizeof(server_simu::serials) / sizeof(uint32_t);
                             MBuf resp(len + 180 * nd); // response  buffer
                             resp << Hex32(1) << Hex32(PROTO_1_ENUM_RESP) << Hex32(uint32_t(0x0)) << Hex32(nd);
                             for (auto dev : server_simu::serials)
                             {
                                 resp << Hex32(dev, true); resp.mseek(180 - sizeof(uint32_t));
                             }
                             pb->send_data(conn_id, resp.to_vector());
                             break;
        }
        case PROTO_1_RAW:
        {
                            ZF_LOGD("From %u received Protocol1 command request packet.", conn_id);
                            mbuf >> z >> ddev;
                            const cmd_schema cmd_schema_simple = { PROTO_1_RAW_RESP, "v_p_0_d_0_0_0" };
                            bvector answer = cmd_schema_simple.gen_plain_command(protocol_ver, DevId(ddev), 0, 0);
                            pb->send_data(conn_id, answer);

                            break;
        }
        case PROTO_1_OPEN:
        {
                             ZF_LOGD("From %u received Protocol1 open device request packet.", conn_id);
                             mbuf >> z >> ddev;
                             const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_1_OPEN_RESP, p1.get_cmd_shema());
                             bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), server_simu::is_serial_ok((uint32_t)ddev) ? 1 : 0, 0);
                             pb->send_data(conn_id, answer);

                             break;
        }
        case PROTO_1_CLOSE:
        {
                              ZF_LOGD("From %u received Protocol1 open device request packet.", conn_id);
                              mbuf >> z >> ddev;
                              const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_1_CLOSE_RESP, p1.get_cmd_shema());
                              bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), 1, 0);
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
    if (protocol_ver == 2)
    {
        Protocol1 p2(&err_p, true);
        switch (command_code)
        {
        case PROTO_2_CMD:
        {
                            ZF_LOGD("From %u received Protocol2 command request packet.", conn_id);
                            mbuf >> z >> ddev;
                            const cmd_schema cmd_schema_simple = { PROTO_2_CMD_RESP, "v_p_0_d_0_0_0" };
                            bvector answer = cmd_schema_simple.gen_plain_command(protocol_ver, DevId(ddev), 0, 0);
                            pb->send_data(conn_id, answer);
                            break;
        }
        case PROTO_2_OPEN:
        {
                             ZF_LOGD("From %u received Protocol2 open device request packet.", conn_id);
                             mbuf >> z >> ddev;
                             const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_2_OPEN_RESP, p2.get_cmd_shema());
                             bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), server_simu::is_serial_ok((uint32_t)ddev) ? 1 : 0, 0);
                             pb->send_data(conn_id, answer);

                             break;
        }
        case PROTO_2_CLOSE:
        {
                              ZF_LOGD("From %u received Protocol2 open device request packet.", conn_id);
                              mbuf >> z >> ddev;
                              const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_2_CLOSE_RESP, p2.get_cmd_shema());
                              bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), 1, 0);
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
#if defined(WIN32) || defined(WIN64)
    WSADATA wsaData;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        ZF_LOGE("WSAStartup failed: %d\n", iResult);
        return 1;
    }
#endif
    return 0;
}

void start_server()
{
    std::cout << "=== Welcome to xibridge server-simulator! ===" << std::endl;
    std::cout << "It uses real sockets on your local computer, so you must specify the server address as 127.0.0.1 in your testing soft." << std::endl;
    std::cout << "Non real devices used. So, available serial numbers for fake devices are ";
    for (auto dev : server_simu::serials)
    {
        std::cout << dev;
        if (dev != server_simu::serials[sizeof(server_simu::serials)/sizeof(uint32_t)-1])
        std::cout << ", ";
    }
   
    std::cout << ". Other serial numbers will  be considered as non existing and open function will return error." << std::endl;
    std::cout << "=== Good luck! ===" << std::endl;
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

int main()
{
    zf_log_set_output_level(ZF_LOG_DEBUG);
    start_server();
    return 0;
}