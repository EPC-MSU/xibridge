#include <cstring>
#include <cstdlib>
#include <cassert>
#include <map>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <functional>
#include <ctype.h>
#include <thread>
#include "../common/utils.h"
#include "../common/protocols.h"
#include "../client/xibridge_client.h" // ERROR CODES
#include "platform.h"
#include "xibridge_server_lib.h"

#include <zf_log.h>

#include "xib-serial/xib_com.h"
#include "bindy/bindy.h"
#include "bindy/tinythread.h"
#include "common.hpp"
#include "platform.h"
#include "devid2usb.h"
#include "mapdevidphnd.h"

#include "../inc/server/supervisor.h"

#ifdef XIBRIDGE_SERVER_ENABLE_SUPERVISOR
#include "supervisor.hpp"
Supervisor supervisor;
#endif


bindy::Bindy * pb = NULL;


#define SEND_WAIT_TIMEOUT_MS 5000

MapDevIdPHandle msu;

static std::thread *_pserver_thread;

static std::atomic<bool> wait;
static std::atomic<int> ret_code;

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

#ifdef XIBRIDGE_SERVER_ENABLE_SUPERVISOR
    /*
    * Capture and release (in destructor) serial number
    * if it is captured many times, but never freed, the supervisor will kill this device
    */
    SupervisorLock _s = SupervisorLock(&supervisor, std::to_string(dev_id.id()));
#endif

    if (protocol_ver == URPC_XINET_PROTOCOL_VERSION)
    {
        Protocol2 p2(&err_p, true);
        bvector cid;
        p2.get_data_from_request(mbuf, cid, req_data, dev_id, resp_len);
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
                                                           MapDevIdPHandle::notify_devs_rescan();
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
                                                            MapDevIdPHandle::notify_devs_rescan();
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

                                                       MapDevIdPHandle::notify_devs_rescan();
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

ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

int server_main(
    const char *keyfile, 
    const char *debug, 
    const char *dev2usb_mode, 
    bool is_console_app,
    void(*cb_devsrescanned_val)())
{
    if (is_already_started())
        return sm_err_allstarted;

#ifndef _WIN32
    signal(SIGSEGV, handler);   // install our handler  
#endif

    int res = initialization();
    if (res)
    {

#ifdef _WIN32
        release_already_started_mutex();
#endif
        return sm_err_initfailed;
    }

    zf_log_set_output_level(ZF_LOG_WARN);

#ifdef XIBRIDGE_SERVER_ENABLE_SUPERVISOR
     supervisor.set_limit(XIBRIDGE_SERVER_SUPERVISOR_LIMIT);
#endif

    ADevId2UsbConfor *pdevid_usb_conf = nullptr;
    // checking for urpc or ximc or ximc_ext presents in cmd and processing  
    const char *susb_m = "bvvu";
    if (dev2usb_mode != nullptr)
    {
        const char * s = dev2usb_mode;
        bool urpc = false;
        bool ximc = false;
        bool ximc_ext = false;
        if (strcmp(s, "by_com_addr") == 0 || strcmp(s, "by_serial") == 0 || strcmp(s, "by_serialpidvid") == 0                // new option vals
            || strcmp(s, "bvvu") == 0)
        {

#ifdef __APPLE__
            if (strcmp(s, "bvvu") == 0 || strcmp(s, "by_com_addr") == 0)
            {
                throw std::runtime_error("'bvvu' and 'by_com_addr' modes are not supported on Mac OS!");
            }
#endif
            susb_m = s;

            pdevid_usb_conf = create_appropriate_dev_id_2_usb_configurator(s);
        }


        pdevid_usb_conf = create_appropriate_dev_id_2_usb_configurator(susb_m);
    }

    MapDevIdPHandle::set_devid_2_usb_confor(pdevid_usb_conf);
    msu.set_devsrescanned(cb_devsrescanned_val);
    
    ADevId2UsbConfor::list_sp_ports();
    MapDevIdPHandle::notify_devs_rescan();

    if (is_console_app)
    {
        ADevId2UsbConfor::print_sp_ports();
    }
    bool is_keyfile_supplied = false;
    if (debug != nullptr)
    {
        char deb_opt[16];
        if (strlen(debug) < 16)
        {
            memset(deb_opt, 0, 16);
            memcpy(deb_opt, debug, strlen(debug));
             if (strcmp(deb_opt, "debug") == 0)
            {
                zf_log_set_output_level(ZF_LOG_DEBUG);
            }
        }
    }
    try
    {
        if (keyfile == nullptr) keyfile = "";
        bindy::Bindy bindy(keyfile, true, false);
        pb = &bindy;
        ZF_LOGI("Starting server...");
        bindy.connect();
        bindy.set_handler(&callback_data);
        bindy.set_discnotify(&callback_disc);
        if (!is_console_app)
        {
            wait = true;

            while (wait);
        }
    }
    catch (std::exception &ex)
    {
        ZF_LOGE("Exception catched: %s.\n Server stopped", ex.what());
     }

#ifdef _WIN32
    release_already_started_mutex();
#endif

    return 0;
}

void server_main_as_dev2usb_by_spv_min(void(*cb_devsrescanned_val)())
{
    int ret = server_main(nullptr, nullptr, "by_serialpidvid", false, cb_devsrescanned_val);
    ret_code.exchange(ret);

}

int start_server_thread_spv(void(*cb_devsrescanned_val)())
{
    ret_code = 0;
    _pserver_thread = new std::thread(server_main_as_dev2usb_by_spv_min, cb_devsrescanned_val);
    msec_sleep(50); // to set ret code if need
    return (int)ret_code;
}

void stop_server_thread()
{
    wait = false;
    _pserver_thread->join();
}

std::vector<std::string> enumerate_devs_opened()
{
    return msu.enumerate_devs_opened();
}

std::vector<std::string> enumerate_devs()
{
    return msu.enumerate_devs();
}
