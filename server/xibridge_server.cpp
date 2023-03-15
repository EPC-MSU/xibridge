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
#include "../inc/client/version.h"
#include "platform.h"

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
#include "devid2usb.h"
#include "mapdevidphnd.h"

#ifdef ENABLE_SUPERVISOR
#include "supervisor.hpp"

Supervisor supervisor;
#endif


bindy::Bindy * pb = NULL;


#define SEND_WAIT_TIMEOUT_MS 5000

MapDevIdPHandle msu;

void send_error_pckt_proto3(conn_id_t conn_id, uint32_t err)
{
    uint32_t errp;
    bvector answer = Protocol3(&errp, true).create_error_response(err);
    pb->send_data(conn_id, answer);
}

uint32_t xib_errors_to_xibridge(xib_result_t res)
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
    uint32_t err_p;
    bvector req_data, resp_data;
    uint32_t resp_len;
    DevId dev_id;

    if (protocol_ver == URPC_XINET_PROTOCOL_VERSION)
    {
        Protocol2 p2(&err_p, true);
        bvector cid;
        p2.get_data_from_request(mbuf, cid, req_data, dev_id, resp_len);
#ifdef ENABLE_SUPERVISOR
        /*
        * Capture and release (in destructor) serial number
        * if it is captured many times, but never freed, the supervisor will kill this device
        */
        SupervisorLock _s = SupervisorLock(&supervisor, std::to_string(dev_id.id()));
#endif

        switch (command_code) {
        case URPC_COMMAND_REQUEST_PACKET_TYPE: {
                                                   ZF_LOGD("From %u received command Protocol2 request packet.", conn_id);
                                                   resp_data.resize(resp_len);
                                                   char ccid[URPC_CID_SIZE];
                                                   memcpy(ccid, cid.data(), URPC_CID_SIZE);
                                                   xib_result_t result = msu.urpc_operation_send_request(
                                                       dev_id,
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
                                                       added = msu.open_if_not(conn_id, dev_id);

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
                                                        msu.remove_conn_or_remove_device(conn_id, dev_id, false);
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
                                                           dev_id,
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
                                                           uint32_t err = xib_errors_to_xibridge(result);
                                                           send_error_pckt_proto3(conn_id, err);
                                                       }
                                                       break;
        }

        case XIBRIDGE_OPEN_DEVICE_REQUEST_PACKET_TYPE: {
                                                           ZF_LOGD("From %u received open device request packet Protocol3.", conn_id);

                                                           added = msu.open_if_not(conn_id, dev_id);

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
                                                            msu.remove_conn_or_remove_device(conn_id, dev_id, false);
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
        case XIBRIDGE_ENUM_REQUEST_PACKET_TYPE: {
                                                       ZF_LOGD("From %u received enum request packet.", conn_id);
                                                       std::vector<DevId> sv = MapDevIdPHandle::get_devid_2_usb_confor() ->enumerate_dev();
                                                       bvector answer = p3.create_enum_response(sv);
                                                       pb->send_data(conn_id, answer);
                                                       ZF_LOGD("To connection %u enum response packet sent.", conn_id);
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
    msu.remove_conn_or_remove_device(conn_id, UINT32_MAX, false);
}

void print_help(char *argv[])
{
#if ZF_LOG_LEVEL <= ZF_LOG_DEBUG
    std::cout <<
        "Usage: " << argv[0] << " [keyfile] [debug] [bvvu|by_com_addr|by_serial|by_serialpidvid]"
        << std::endl
        << "Examples: " << std::endl
        << argv[0] << std::endl
        << argv[0] << " ximc" << std::endl
        << argv[0] << " debug bvvu" << std::endl
        << argv[0] << " ~/keyfile.sqlite by_serial" << std::endl
        << argv[0] << " ~/keyfile.sqlite debug by_com_addr" << std::endl
        << "Debug logging will be disabled by default, bvvu-style usb port matching configuration selected by default" << std::endl;
#else
    std::cout << "Usage: " << argv[0] << " keyfile"
        << std::endl
        << "Examples: " << std::endl
        << argv[0] << std::endl
        << argv[0] << " ~/keyfile.sqlite" << std::endl;
#endif

    std::cout << "Press a key to exit" << std::endl;

}


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

    xibridge_version_t ver = XIBRIDGE_VERSION;
    std::cout << "=== Xibridge Server "
        << (int) ver.major << "."
        << (int) ver.minor << "."
        << (int) ver.bagfix << " "
        << "===" << std::endl;

    std::cout << "=== xi-net protocols v.2 and v.3 supported ===" << std::endl;

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
        
    ADevId2UsbConfor *pdevid_usb_conf = nullptr;
    // checking for urpc or ximc or ximc_ext presents in cmd and processing  
    const char *susb_m = "bvvu";
    if (argc > 1)
    {
        const char * s = argv[argc - 1];
        bool urpc = false; 
        bool ximc = false;
        bool ximc_ext = false;
        if ((urpc = strcmp(s, "urpc") == 0) || (ximc = strcmp(s, "ximc") == 0) || (ximc_ext = strcmp(s, "ximc_ext") == 0) || // for compatibility
            strcmp(s, "by_com_addr") == 0 || strcmp(s, "by_serial") == 0 || strcmp(s, "by_serialpidvid") == 0                // new option vals
            || strcmp(s, "bvvu") == 0)
        {
            argc--;
            
            if (urpc) s = "by_com_addr";
            if (ximc) s = "by_serial";
            if (ximc_ext) s = "by_serialpidvid";

#ifdef __APPLE__
            if (strcmp(s, "bvvu") == 0 || strcmp(s, "by_com_addr") == 0)
            {
                throw std::runtime_error(s == "'bvvu' |'by_com_addr' modes are not supported no Mac OS!");
            }
#endif
            susb_m = s;

            pdevid_usb_conf = create_appropriate_dev_id_2_usb_configurator(s);
        }
    }

    if (pdevid_usb_conf == nullptr)
    {
        pdevid_usb_conf = create_appropriate_dev_id_2_usb_configurator(susb_m);
    }
    std::cout << "=== The " << susb_m << " style configuration is selected as the usb port matching configuration ===" << std::endl;

    MapDevIdPHandle::set_devid_2_usb_confor(pdevid_usb_conf);
    bool is_keyfile_supplied = false;
    if (argc > 1)
    {
        strlwr_portable(argv[1]);
        if (!(is_keyfile_supplied = strcmp(argv[1], "debug") != 0) || (argc > 2 && strcmp(argv[2], "debug") == 0))
        {
            zf_log_set_output_level(ZF_LOG_DEBUG);
        }
     }
    ADevId2UsbConfor::print_sp_ports();
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
    if (pdevid_usb_conf != nullptr) delete pdevid_usb_conf;
    return 0;
}
