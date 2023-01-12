#include <cstring>
#include <cstdlib>
#include <cassert>
#include <map>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <functional>
#include <ctype.h>
#include "../common/utils.h"
#include "../common/protocols.h"
#include "../client/xibridge_client.h" // ERROR CODES

#ifndef _WIN32 
#include <execinfo.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#else
//
#endif


/*
* Supervisor option.
* It may not work properly on windows now.
*/
// #define ENABLE_SUPERVISOR

#include <zf_log.h>

#include "xib-serial/xib_com.h"
#include "bindy/bindy.h"
#include "bindy/tinythread.h"
#include "common.hpp"
#include "platform.h"
#include "mapserialurpc.h"

#ifdef ENABLE_SUPERVISOR
#include "supervisor.hpp"

Supervisor supervisor;
#endif


bindy::Bindy * pb = NULL;


#define SEND_WAIT_TIMEOUT_MS 5000

MapSerialUrpc msu;

// xibridge-server protocol3 support object - to map DevId to some serial number

std::map <DevId, uint32_t> _server3_devid_serial;
 
// for xibridge_server  - matching complex DevId and serial port simple id
static uint32_t get_serial_from_DevId(const DevId &devid)
{
    // temp. simple trnasformation while enumerate does not exit 
    return devid.id();
}

void send_error_pckt_proto3(conn_id_t conn_id, uint32_t err)
{
    uint32_t errp;
    bvector answer = Protocol3(&errp, true).create_error_response(err);
    pb->send_data(conn_id, answer);
}

uint32_t urpc_errors_to_xibridge(xib_result_t res)
{
    uint32_t err;
    switch (res)
    {
    case xib_result_error:
        err = ERR_DEVICE_ERR;
        break;
    case xib_result_value_error:
        err = ERR_DEVICE_ERR_VAL;
        break;
    case xib_result_timeout:
        err = ERR_RECV_TIMEOUT;
        break;
    case xib_result_nodevice:
        err = ERR_DEVICE_LOST;
        break;
    default: 
        err = 0;
    }

    return err;
}


// ========================================================
void callback_data(conn_id_t conn_id, std::vector<uint8_t> data) {
    ZF_LOGD("From %u received packet of length: %lu.", conn_id, data.size());

    if (data.size() < 16) { // We need at least the protocol version and command code... and serial too
        ZF_LOGE("From %u received incorrect data packet: Size: %lu, expected at least 16.", conn_id, data.size());
        throw std::runtime_error("Incorrect data packet");
    }

    bool added;
    MBuf mbuf(data.data(), data.size());
    Hex32 proto;
    mbuf >> proto;
    mbuf.mseek(-(int)sizeof(uint32_t));
    uint32_t protocol_ver = (uint32_t)proto;
    uint32_t command_code = AProtocol::get_pckt_of_cmd(data);
    uint32_t err_p, serial;
    bvector req_data, resp_data;
    uint32_t resp_len;
    DevId dev_id;

    if (protocol_ver == URPC_XINET_PROTOCOL_VERSION)
    {
        Protocol2 p2(&err_p, true);
        bvector cid;
        p2.get_data_from_request(mbuf, cid, req_data, dev_id, resp_len);
        serial = get_serial_from_DevId(dev_id);
#ifdef ENABLE_SUPERVISOR
        /*
        * Capture and release (in destructor) serial number
        * if it is captured many times, but never freed, the supervisor will kill this device
        */
        SupervisorLock _s = SupervisorLock(&supervisor, std::to_string(serial));
#endif

        switch (command_code) {
        case URPC_COMMAND_REQUEST_PACKET_TYPE: {
                                                   ZF_LOGD("From %u received command Protocol2 request packet.", conn_id);
                                                   resp_data.resize(resp_len);
                                                   char ccid[URPC_CID_SIZE];
                                                   memcpy(ccid, cid.data(), URPC_CID_SIZE);
                                                   xib_result_t result = msu.urpc_operation_send_request(
                                                       serial,
                                                       ccid,
                                                       req_data.data(),
                                                       (uint8_t)req_data.size(),
                                                       resp_data.data(),
                                                       (uint8_t)resp_len
                                                       );

                                                    bvector answer = p2.create_cmd_response((uint32_t)result, dev_id, &resp_data);
                                                    pb->send_data(conn_id, answer);
                                                 
                                                    if (result == xib_result_nodevice)
                                                       ZF_LOGE("The operation_urpc_send_reqest returned urpc_result_nodevic (conn_id = %u).", conn_id);
                                                    break;
        }

        case URPC_OPEN_DEVICE_REQUEST_PACKET_TYPE: {
                                                       ZF_LOGD("From %u received open device request packet Protocol 2.", conn_id);
                                                       added = msu.open_if_not(conn_id, serial);

                                                       bvector answer = p2.create_open_response(dev_id, added);
                                                       pb->send_data(conn_id, answer);

                                                       if (added)
                                                       {
                                                           ZF_LOGD("New connection added conn_id=%u + ...", conn_id);
                                                       }
                                                       msu.log();
                                                       break;
        }
        case URPC_CLOSE_DEVICE_REQUEST_PACKET_TYPE: {
                                                        ZF_LOGD("From %u received close device request packet Protocol 2.", conn_id);
                                                        msu.remove_conn_or_remove_urpc_device(conn_id, serial, false);
                                                        ZF_LOGD("Connection or Device removed ordinary with conn_id=%u + ...", conn_id);
                                                        msu.log();
                                                        bvector answer = p2.create_close_response(dev_id, 0);
                                                        pb->send_data(conn_id, answer);
                                                        ZF_LOGD("To connection %u close device response packet sent.", conn_id);
                                                        break;
        }
        default: {
                     ZF_LOGD("Unknown packet code.");
                     break;
        }
        }
    }
    else if (protocol_ver == XIBRIDGE_XINET_PROTOCOL_VERSION)
    {
        Protocol3 p3(&err_p, true);
        if (!p3.get_data_from_request(mbuf, req_data, dev_id, resp_len))
        {
            ZF_LOGE("From %u received bad packet, error: %u. Error protocol3 pckt will be sent to client.", conn_id, err_p);
            send_error_pckt_proto3(conn_id, err_p);
            return;
        }

        serial = get_serial_from_DevId(dev_id);

        switch (command_code)
        {

#ifdef ENABLE_SUPERVISOR
            /*
            * Capture and release (in destructor) serial number
            * if it is captured many times, but never freed, the supervisor will kill this device
            */
            SupervisorLock _s = SupervisorLock(&supervisor, std::to_string(serial));
#endif

        case XIBRIDGE_COMMAND_REQUEST_PACKET_TYPE: {
                                                       ZF_LOGD("From %u received Protocol3 command request packet.", conn_id);
                                                       // !!! to do - error processing
                                                       resp_data.resize(resp_len);
                                                       xib_result_t result = msu.operation_send_request(
                                                           serial,
                                                           req_data.data(),
                                                           (uint8_t)req_data.size(),
                                                           resp_data.data(),
                                                           (uint8_t)resp_len
                                                           );
                                                       // according to Protocol 3
                                                       if (result == xib_result_ok)
                                                       {
                                                           bvector answer = p3.create_cmd_response(dev_id, &resp_data);
                                                           pb->send_data(conn_id, answer);
                                                           ZF_LOGD("Answered just with data to client conn_id: %u (protocol 3)", conn_id);
                                                       }
                                                       else
                                                       {
                                                           uint32_t err = urpc_errors_to_xibridge(result);
                                                           send_error_pckt_proto3(conn_id, err);
                                                       }
                                                       break;
        }

        case XIBRIDGE_OPEN_DEVICE_REQUEST_PACKET_TYPE: {
                                                           ZF_LOGD("From %u received open device request packet Protocol3.", conn_id);

                                                           added = msu.open_if_not(conn_id, serial);

                                                           bvector answer = p3.create_open_response(dev_id, added);
                                                           pb->send_data(conn_id, answer);

                                                           if (added)
                                                           {
                                                               ZF_LOGD("New connection added conn_id=%u + ...", conn_id);
                                                           }
                                                           msu.log();
                                                           break;
        }
        case XIBRIDGE_CLOSE_DEVICE_REQUEST_PACKET_TYPE: {
                                                            ZF_LOGD("From %u received close device request packet Protocol3.", conn_id);
                                                            msu.remove_conn_or_remove_urpc_device(conn_id, serial, false);
                                                            ZF_LOGD("Connection or Device removed ordinary with conn_id=%u + ...", conn_id);
                                                            msu.log();
                                                            bvector answer = p3.create_close_response(dev_id, 0);
                                                            pb->send_data(conn_id, answer);
                                                            ZF_LOGD("To connection %u close device response packet sent.", conn_id);
                                                            break;
        }

        case XIBRIDGE_VERSION_REQUEST_PACKET_TYPE: {
                                                       ZF_LOGD("From %u received version request packet.", conn_id);
                                                       bvector answer = p3.create_version_response();
                                                       pb->send_data(conn_id, answer);
                                                       ZF_LOGD("To connection %u version response packet sent.", conn_id);
                                                       break;
        }
        default: {
                     ZF_LOGD("Unknown packet code.");
                     break;
        }
        }
    }
    else
    {
        ZF_LOGE("From %u received packet with not supported protocol version: %u.", conn_id, protocol_ver);
        send_error_pckt_proto3(conn_id, ERR_NO_PROTOCOL);
        ZF_LOGD("To connection %u error response packet Protocol 3 sent.", conn_id);
    }
}
// ========================================================

void callback_disc(conn_id_t conn_id) {
    // if there is an ordinary case - no connection to process in server structures (it is all alredy done); if there is an extra case - the connection will be deleted here  
    msu.remove_conn_or_remove_urpc_device(conn_id, UINT32_MAX, false);
}

void print_help(char *argv[])
{
#if ZF_LOG_LEVEL <= ZF_LOG_DEBUG
    std::cout <<
        "Usage: " << argv[0] << " [keyfile] [debug]"
        << std::endl
        << "Examples: " << std::endl
        << argv[0] << std::endl
        << argv[0] << " debug" << std::endl
        << argv[0] << " ~/keyfile.sqlite" << std::endl
        << argv[0] << " ~/keyfile.sqlite debug" << std::endl
        << "Debug logging will be disabled by default" << std::endl;
#else
    std::cout << "Usage: " << argv[0] << " keyfile"
        << std::endl
        << "Examples: " << std::endl
        << argv[0] << std::endl
        << argv[0] << " ~/keyfile.sqlite" << std::endl;
#endif

    std::cout << "Press a key to exit" << std::endl;

}

#ifndef _WIN32
void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 32);

    // print out all the frames to stderr
    ZF_LOGE("IN SIGNAL HANDLER: signal no %d:\n", sig);
    ZF_LOGE("Stack trace...");
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    ZF_LOGE("End of stack trace.");
    exit(1);
}

#ifdef __APPLE__

bool is_already_started()
{
    return false;
}

#else

#define SOCKET_NAME "SOCKET_TO_BE_USED_AS_NAMED_MUTEX"
#ifndef UNIX_PATH_MAX                                                           
#define UNIX_PATH_MAX (108)                                                   
#endif
#define MIN(A,B) A<B?A:B

bool is_already_started()
{
    struct sockaddr_un SockAddr;
    int AddrLen;
    int Socket = socket(PF_UNIX, SOCK_STREAM, 0);
    if (Socket == -1)
    {
        std::cout << "Unable to open communication socket because of " << strerror(errno) << ". Exit." << std::endl;
        return true;
    }
    else
    {
        SockAddr.sun_family = AF_UNIX;
        memset(&SockAddr.sun_path, 0, UNIX_PATH_MAX);
        memcpy(&SockAddr.sun_path[1], SOCKET_NAME, MIN(strlen(SOCKET_NAME), UNIX_PATH_MAX));
        AddrLen = sizeof (SockAddr);
        if (bind(Socket, (struct sockaddr *) &SockAddr, AddrLen))
        {
            std::cout << "Another process (urpc_xinet_server) already running. Exit." << std::endl;
            return true;
        }
    }
    return false;
}
#endif
#else
static HANDLE _h_already_started;
bool is_already_started()
{
    const char szUniqueNamedMutex[] = "urpc_xinet_server_m";
    _h_already_started = CreateMutex(NULL, TRUE, szUniqueNamedMutex);
    if (ERROR_ALREADY_EXISTS == GetLastError())
    {
        std::cout << "Another process (urpc_xinet_server) already running. Press a key to exit!" << std::endl;
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

//the next function id not C standard, not supported in non win, the next is manual definition  
char *strlwr_portable(char *str)
{
    unsigned char *p = (unsigned char *)str;

    while (*p) {
        *p = tolower((unsigned char)*p);
        p++;
    }

    return str;
}


ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;



int main(int argc, char *argv[])
{

    if (is_already_started())
        return 0;
#ifndef _WIN32
    signal(SIGSEGV, handler);   // install our handler  
#endif
    std::cout << "=== Xibridge Server "
        << 1 << "."
        << 0 << "."
        << 0 << " "
        << "===" << std::endl;

    std::cout << "=== Protocol v.2 and v.3 supported (except enumeration) ===" << std::endl;

    bool exit = false;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            //if any param is something like "help" 
            char *s = argv[i];
            strlwr_portable(s);
            if (strcmp(s, "-help") == 0 || strcmp(s, "help") == 0
                || strcmp(s, "--help") == 0 || strcmp(s, "-h") == 0
                || strcmp(s, "--h") == 0)
                exit = true;
        }
    }
    if (exit)
    {
        print_help(argv);
        std::cin.get(); // To avoid console closing
#ifdef _WIN32
        release_already_started_mutex();
#endif
        return 0;
    }

    int res = initialization();
    if (res)
    {
#ifdef _WIN32
        release_already_started_mutex();
#endif
        return res;
    }

    zf_log_set_output_level(ZF_LOG_WARN);
    bool is_keyfile_supplied = false;
    if (argc > 1)
    {
        strlwr_portable(argv[1]);
        if (!(is_keyfile_supplied = strcmp(argv[1], "debug") != 0) || (argc > 2 && strcmp(argv[2], "debug") == 0))
        {
            zf_log_set_output_level(ZF_LOG_DEBUG);
        }
    }

    try
    {

        bindy::Bindy bindy(is_keyfile_supplied ? argv[1] : "", true, false);
        pb = &bindy;

#ifdef ENABLE_SUPERVISOR
        if (argc > 2)
        {
            if (strcmp(argv[2], "disable_supervisor") == 0)
            {
                supervisor.stop();
            }
            else if (strcmp(argv[2], "enable_supervisor") == 0)
            {
                ; // already enabled
            }
            else
            {
                print_help(argv);
#ifdef _WIN32
                release_already_started_mutex();
#endif
                return 0;
            }
        }
        if (argc == 4)
        {
            supervisor.set_limit(std::stoi(argv[3]));
        }
#endif

        ZF_LOGI("Starting server...");

        bindy.connect();
        bindy.set_handler(&callback_data);
        bindy.set_discnotify(&callback_disc);
    }
    catch (std::exception &ex)
    {
        std::cout << "Exception catched: " << ex.what() << std::endl
            << "Server stopped" << std::endl;
    }
#ifdef _WIN32
    release_already_started_mutex();
#endif
    return 0;
}
