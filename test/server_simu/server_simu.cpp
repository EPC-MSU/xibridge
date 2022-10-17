#include <iostream>
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
                            const cmd_schema_t cmd_schema_simple = { 0, "v_p_0_u" };
                            int len = cmd_schema_simple.get_plain_command_length();
                            uint32_t nd = (uint32_t)sizeof(server_simu::serials) / sizeof(uint32_t);
                            MBuf resp(len + 180 * nd); // response  buffer
                            resp << Hex32(1) << Hex32(PROTO_1_ENUM_RESP) << Hex32(uint32_t(0x0)) << Hex32(nd);
                            for (auto dev : server_simu::serials)
                            {
                                 resp << Hex32(dev, true); resp.mseek(180 - sizeof(uint32_t));
                            }
                            pb->send_data(conn_id, resp.to_vector());
                            ZF_LOGD("Answered with 3 devices (enum resp protocol 1) to client conn_id: %u", conn_id);
                            break;
        }
        case PROTO_1_RAW:
        {
                            ZF_LOGD("From %u received Protocol1 command request packet.", conn_id);
                            mbuf >> z >> ddev;
                            const cmd_schema_t cmd_schema_simple = { PROTO_1_RAW_RESP, "v_p_0_d_0_0_0" };
                            bvector answer = cmd_schema_simple.gen_plain_command(protocol_ver, DevId(ddev), 0, 0);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD("Answered 0 to client conn_id: %u (protocol1)", conn_id);
                            break;
        }
        case PROTO_1_OPEN:
        {
                            ZF_LOGD("From %u received Protocol1 open device request packet.", conn_id);
                            mbuf >> z >> ddev;
                            const cmd_schema_t & cm1 = cmd_schema_t::get_schema(PROTO_1_OPEN_RESP, p1.get_cmd_schema());
                            uint32_t one_zero = server_simu::is_serial_ok((uint32_t)ddev) ? 1 : 0;
                            bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), one_zero, 0);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD(one_zero ? "New connection added conn_id=%u + ... (protocol 1)" : "Sent No to conn_id=%u!", conn_id);
                            break;
        }
        case PROTO_1_CLOSE:
        {
                            ZF_LOGD("From %u received Protocol1 open device request packet.", conn_id);
                            mbuf >> z >> ddev;
                            const cmd_schema_t & cm1 = cmd_schema_t::get_schema(PROTO_1_CLOSE_RESP, p1.get_cmd_schema());
                            bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), 1, 0);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD("To connection %u close device response packet sent (protocol 1).", conn_id);
                            break;

        }

        default:
        {
                            ZF_LOGD("Unknown packet code (protocol 1).");
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
                            const cmd_schema_t cmd_schema_simple = { PROTO_2_CMD_RESP, "v_p_0_d_0_0_0" };
                            bvector answer = cmd_schema_simple.gen_plain_command(protocol_ver, DevId(ddev), 0, 0);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD("Answered 0 to client conn_id: %u (protocol 2)", conn_id);
                            break;
        }
        case PROTO_2_OPEN:
        {
                            ZF_LOGD("From %u received Protocol2 open device request packet.", conn_id);
                            mbuf >> z >> ddev;
                            const cmd_schema_t & cm1 = cmd_schema_t::get_schema(PROTO_2_OPEN_RESP, p2.get_cmd_schema());
                            uint32_t one_zero = server_simu::is_serial_ok((uint32_t)ddev) ? 1 : 0;
                            bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), one_zero, 0);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD(one_zero ? "New connection added conn_id = %u + ...(protocol 2)" : "Sent No to conn_id = %u!", conn_id);
                            break;
        }
        case PROTO_2_CLOSE:
        {
                            ZF_LOGD("From %u received Protocol2 open device request packet.", conn_id);
                            mbuf >> z >> ddev;
                            const cmd_schema_t & cm1 = cmd_schema_t::get_schema(PROTO_2_CLOSE_RESP, p2.get_cmd_schema());
                            bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), 1, 0);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD("To connection %u close device response packet sent (protocol 2).", conn_id);
                            break;

        }


        default:
        {
                            ZF_LOGD("Unknown packet code (protocol 3).");
                            break;
        }
        }
    }

    if (protocol_ver == 3)
    {
        Protocol3 p3(&err_p, true);
        switch (command_code)
        {
        case PROTO_3_ENUM:
        {
                            ZF_LOGD("From %u received Protocol1 enum request packet.", conn_id);
                            // according to simple scenario - sending 3 serials
                            const cmd_schema_t cmd_schema_simple = { 0, "v_p_0_0_0_0_0_0_u" };
                            int len = cmd_schema_simple.get_plain_command_length();
                            uint32_t nd = (uint32_t)sizeof(server_simu::serials) / sizeof(uint32_t);
                            MBuf resp(len + nd * sizeof(xibridge_device_t));
                            resp << Hex32(3) << Hex32(PROTO_3_ENUM_RESP) << Hex32((uint32_t)0x00) << 
                            Hex32((uint32_t)0x00) << Hex32((uint32_t)0x00) << Hex32((uint32_t)0x00) 
                            << Hex32((uint32_t)0x00) << Hex32((uint32_t)0x00) << Hex32(nd);
                            for (auto dev : server_simu::serials)
                            {
                                DevId d(dev);
                                resp << HexIDev3(&d);
                            }
                            resp << Hex32((uint32_t)0x0);
                            pb->send_data(conn_id, resp.to_vector());
                            ZF_LOGD("Answered with 3 devices (enum resp protocol 3) to client conn_id: %u", conn_id);
                            break;
        }
        case PROTO_3_CMD:
        {
                            ZF_LOGD("From %u received Protocol3 command request packet.", conn_id);
                            mbuf >> z >> edev;
                            const cmd_schema_t cmd_schema_simple = { PROTO_3_CMD_RESP, "v_p_0_I_0_0_u_0" };
                            bvector answer = cmd_schema_simple.gen_plain_command(protocol_ver, DevId(ddev), 0, 1);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD("Answered just with no data to client conn_id: %u (protocol 3)", conn_id);
                            break;
        }

        case PROTO_3_OPEN:
        {
                            ZF_LOGD("From %u received Protocol3 open device request packet.", conn_id);
                            mbuf >> z >> edev;
                            const cmd_schema_t & cm1 = cmd_schema_t::get_schema(PROTO_3_OPEN_RESP, p3.get_cmd_schema());
                            uint32_t one_zero = server_simu::is_serial_ok(edev.toDevId().id()) ? 1 : 0;
                            bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), one_zero, 0);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD(one_zero ? "New connection added conn_id = %u + ...(protocol 3)" : "Sent No to conn_id = %u!", conn_id);
                            break;
        }
        case PROTO_3_CLOSE:
        {
                            ZF_LOGD("From %u received Protocol3 close device request packet.", conn_id);
                            mbuf >> z >> edev;
                            const cmd_schema_t & cm1 = cmd_schema_t::get_schema(PROTO_3_CLOSE_RESP, p3.get_cmd_schema());
                            bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), 1, 0);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD("To connection %u close device response packet sent (protocol 3).", conn_id);
                            break;

        }
        case PROTO_3_VER:
        {
                            ZF_LOGD("From %u received Protocol3 server version request packet.", conn_id);
                            const cmd_schema_t & cm1 = cmd_schema_t::get_schema(PROTO_3_VER_RESP, p3.get_cmd_schema());
                            bvector answer = cm1.gen_plain_command(protocol_ver, DevId(ddev), 0, 3);
                            pb->send_data(conn_id, answer);
                            ZF_LOGD("To connection %u server version response packet sent (protocol 3).", conn_id);
                            break;
                              
        }


        default:
        {
                            ZF_LOGD("Unknown packet code (protocol 3).");
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

#ifndef _WIN32
#include <sys/types.h>
#include <unistd.h>
#include <sstream>

bool is_already_started()
{
    // get this process pid
    pid_t pid = getpid();
    // compose a bash command that:
    // check if another process with the same name 
    // but with different pid is running
    std::stringstream command;
    command << "ps -eo pid,comm | grep urpc_xinet_serv | grep -v ' " << pid << " '";
    int isRuning = system(command.str().c_str());
    if (isRuning == 0) {
        std::cout << "Another process (urpc_xinet_server) already running. Exit." << std::endl;
        return true;
    }
    return false;
}
#else
static HANDLE _h_already_started;
bool is_already_started()
{
    const char szUniqueNamedMutex[] = "urpc_xinet_server_m";
    _h_already_started = CreateMutex(NULL, TRUE, szUniqueNamedMutex);
    if (ERROR_ALREADY_EXISTS == GetLastError())
    {
        std::cout << "Another process (server_simu or urpc_xinet_server) already running. Press a key to exit!" << std::endl;
        std::cin.get(); // To avoid console closing
        return true;
    }
    return false;
}

void release_already_started_mutex()
{
    ReleaseMutex(_h_already_started); // Explicitly release mutex
    CloseHandle(_h_already_started); // close handle before terminating
}
#endif

void start_server()
{
    if (is_already_started())
        return;
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
#ifdef _WIN32
    release_already_started_mutex();
#endif
}

ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

int main()
{
    zf_log_set_output_level(ZF_LOG_DEBUG);
    start_server();
    return 0;
}
