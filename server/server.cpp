#include <cstring>
#include <cstdlib>
#include <cassert>
#include <map>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <functional>
#include <ctype.h>

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

#include "../urpc.h"
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

class CommonDataPacket {
public:
    bool send_data() {
        if (pb == NULL) {
            return false;
        }
        try {
            adaptive_wait_send(pb, conn_id, reply, SEND_WAIT_TIMEOUT_MS);
        } catch (const std::exception &) {
            // Logged in adaptive_wait_send()
            return false;
        }
        return true;
    }

protected:
    std::vector<uint8_t> reply;
    conn_id_t conn_id;
};


template <int PacketId>
class DataPacket : public CommonDataPacket { }; // Still allows us to instantiate common packet, which is wrong

template <>
class DataPacket <URPC_OPEN_DEVICE_RESPONSE_PACKET_TYPE> : public CommonDataPacket {
public:
    DataPacket(conn_id_t conn_id, uint32_t serial, bool opened_ok) {
        this->conn_id = conn_id;

        int len = sizeof(urpc_xinet_common_header_t) + sizeof(uint32_t);
        reply.resize(len);
        std::fill(reply.begin(), reply.end(), 0x00);

        write_uint32(&reply.at(0), URPC_XINET_PROTOCOL_VERSION);
        write_uint32(&reply.at(4), URPC_OPEN_DEVICE_RESPONSE_PACKET_TYPE);
        write_uint32(&reply.at(12), serial);
        write_bool(&reply.at(len - 1), opened_ok);
    }
};

template <>
class DataPacket <URPC_CLOSE_DEVICE_RESPONSE_PACKET_TYPE> : public CommonDataPacket {
public:
    DataPacket(conn_id_t conn_id, uint32_t serial) {
        this->conn_id = conn_id;

        int len = sizeof(urpc_xinet_common_header_t) + sizeof(uint32_t);
        reply.resize(len);
        std::fill(reply.begin(), reply.end(), 0x00);

        write_uint32(&reply.at(0), URPC_XINET_PROTOCOL_VERSION);
        write_uint32(&reply.at(4), URPC_CLOSE_DEVICE_RESPONSE_PACKET_TYPE);
        write_uint32(&reply.at(12), serial);
    }
};

template <>
class DataPacket <URPC_COMMAND_RESPONSE_PACKET_TYPE> : public CommonDataPacket {
public:
    DataPacket(conn_id_t conn_id, uint32_t serial, urpc_result_t result, uint8_t *ptr, uint32_t size) {
        this->conn_id = conn_id;

        int len = sizeof(urpc_xinet_common_header_t) + sizeof(result) + size;
        reply.resize(len);
        std::fill (reply.begin(), reply.end(), 0x00);

        write_uint32(&reply.at(0), URPC_XINET_PROTOCOL_VERSION);
        write_uint32(&reply.at(4), URPC_COMMAND_RESPONSE_PACKET_TYPE);
        write_uint32(&reply.at(12), serial);
        write_uint32(&reply.at(sizeof(urpc_xinet_common_header_t)), (uint32_t)result);
        write_bytes(reply.data() + sizeof(urpc_xinet_common_header_t)+sizeof(result), ptr, size);
    }
};

// ========================================================
void callback_data(conn_id_t conn_id, std::vector<uint8_t> data) {
    ZF_LOGD("From %u received packet of length: %lu.", conn_id, data.size());

    if (data.size() < 16) { // We need at least the protocol version and command code... and serial too
        ZF_LOGE( "From %u received incorrect data packet: Size: %lu, expected at least 16.", conn_id, data.size() );
        throw std::runtime_error( "Incorrect data packet" );
    }

    bool added;
    uint32_t protocol_ver;
    uint32_t command_code;
    uint32_t serial;
    read_uint32(&protocol_ver, &data[0]);
    if(URPC_XINET_PROTOCOL_VERSION != protocol_ver) {
        ZF_LOGE( "From %u received packet with not supported protocol version: %u.", conn_id, protocol_ver );
        return;
    }

    read_uint32(&command_code, &data[4]);
    read_uint32(&serial, &data[12]); // strictly speaking it might read junk in case of enumerate_reply or something else which does not have the serial... if someone sends us such packet

    #ifdef ENABLE_SUPERVISOR
    /*
     * Capture and release (in destructor) serial number
     * if it is captured many times, but never freed, the supervisor will kill this device
     */
    SupervisorLock _s = SupervisorLock(&supervisor, std::to_string(serial));
    #endif

    switch (command_code) {
        case URPC_COMMAND_REQUEST_PACKET_TYPE: {
            ZF_LOGD( "From %u received command request packet.", conn_id );

            char cid[URPC_CID_SIZE];
            std::memcpy(cid, &data[sizeof(urpc_xinet_common_header_t)], sizeof(cid));

            uint32_t response_len;
            read_uint32(&response_len, &data[sizeof(urpc_xinet_common_header_t) + sizeof(cid)]);

            size_t request_len;
            request_len = data.size() - sizeof(urpc_xinet_common_header_t) - sizeof(cid) - sizeof(response_len);
            std::vector<uint8_t> response(response_len);

            urpc_result_t result = msu.operation_urpc_send_request(
                    serial,
                    cid,
                    request_len ? &data[sizeof(urpc_xinet_common_header_t) + sizeof(cid) + sizeof(response_len)] : NULL,
                    (uint8_t)request_len,
                    response.data(),
                    (uint8_t)response_len
                );
                      
            DataPacket<URPC_COMMAND_RESPONSE_PACKET_TYPE>
                    response_packet(conn_id, serial, result, response.data(), response_len);
            if (result == urpc_result_nodevice)
                ZF_LOGE("The operation_urpc_send_reqest returned urpc_result_nodevic (conn_id = %u).", conn_id);
            if (!response_packet.send_data())               
                ZF_LOGD("To %u command response packet send failed.", conn_id);
            break;
        }

        case URPC_OPEN_DEVICE_REQUEST_PACKET_TYPE: {
            ZF_LOGD( "From %u received open device request packet.", conn_id );
            added = msu.open_if_not(conn_id, serial);
            DataPacket<URPC_OPEN_DEVICE_RESPONSE_PACKET_TYPE> response_packet(conn_id, serial, added);

            if (!response_packet.send_data()) {
                ZF_LOGE("To %u open device response packet sending error.", conn_id);
            } else {
                ZF_LOGD("To %u open device response packet sent.", conn_id);
            }

            if (added)
            {
                ZF_LOGD("New connection added conn_id=%u + ...", conn_id);
            }
            msu.log();
            break;
        }
        case URPC_CLOSE_DEVICE_REQUEST_PACKET_TYPE: {
            ZF_LOGD( "From %u received close device request packet.", conn_id );
            msu.remove_conn_or_remove_urpc_device(conn_id, serial, false);
            ZF_LOGD("Connection or Device removed ordinary with conn_id=%u + ...", conn_id);
            msu.log();
            DataPacket<URPC_CLOSE_DEVICE_RESPONSE_PACKET_TYPE>
                    response_packet(conn_id, serial);
            response_packet.send_data();
            ZF_LOGD( "To connection %u close device response packet sent.", conn_id);
            break;
        }
        default: {
            ZF_LOGD( "Unknown packet code." );
            break;
        }
    }
}
// ========================================================

void callback_disc(conn_id_t conn_id) {
// if there is an ordinary case - no connection to process in server structures (it is all alredy done); if there ia an extra case - the connection will be deleted here  
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

#define SOCKET_NAME "SOCKET_TO_BE_USED_AS_NAMED_MUTEX"
#ifndef UNIX_PATH_MAX                                                           
  #define UNIX_PATH_MAX (108)                                                   
#endif
#define MIN(A,B) A<B?A:B

bool is_already_started()
{
    struct sockaddr_un SockAddr;
    int AddrLen;
    int Socket = socket (PF_UNIX, SOCK_STREAM, 0);                                    
    if (Socket == -1) 
    {                                                           
        std::cout << "Unable to open communication socket because of " << strerror (errno) << ". Exit." << std::endl;                                                             
        return true;                                                                  
    }     
    else 
    {                                                                      
        SockAddr.sun_family = AF_UNIX;                                              
        memset (&SockAddr.sun_path, 0, UNIX_PATH_MAX);                              
        memcpy (&SockAddr.sun_path [1], SOCKET_NAME, MIN (strlen (SOCKET_NAME), UNIX_PATH_MAX));
        AddrLen = sizeof (SockAddr);                                                
        if (bind (Socket, (struct sockaddr *) &SockAddr, AddrLen)) 
        {                
            std::cout<< "Another process (urpc_xinet_server) already running. Exit." << std::endl;                                                              
            return true;                                                                
        }
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
    std::cout << "=== uRPC XiNet Server "
              << URPC_XINET_VERSION_MAJOR << "."
              << URPC_XINET_VERSION_MINOR << "."
              << URPC_XINET_VERSION_BUGFIX << " "
              << "===" << std::endl;

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
