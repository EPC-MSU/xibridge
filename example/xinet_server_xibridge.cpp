#include <thread>
#include <../common/protocols.h>
#include <../xibridge.h>
#include "urmc-min/urmc_min.h"
#include "../common/utils.h"

#define TH_NUM 5

static char  _DEV_IP[256];

/* 
* In fact it's the same urpc-device server, but via protocol 3 

*/
bool xinet_xibridge_usage_example(const char * ip, uint32_t dev_num)
{
    sprintf(_DEV_IP, "xi-net://%s/%x", ip, dev_num);
    xibridge_conn_t conn;

    xibridge_version_t real_server_version;

    uint32_t err = xibridge_get_server_last_protocol_version(_DEV_IP, &real_server_version);

    if (err)
    {
        printf("Cannot get xibridge vesrion (execute version request) %s: %s\n", _DEV_IP, xibridge_get_err_expl(err));
        return false;
    }

    xibridge_set_base_protocol_version(real_server_version);

    err = xibridge_open_device_connection(_DEV_IP, &conn);
    if (err)
    {
        printf ("Cannot open %s: %s\n", _DEV_IP, xibridge_get_err_expl(err));
        xibridge_close_device_connection(&conn);
        return false;
    }
    unsigned char resp[sizeof(urmc_get_identity_information_t) + sizeof(uint32_t)];
   
    uint32_t ginf_err = xibridge_device_request_response(&conn, (const unsigned char *)"ginf", 4, resp, sizeof(urmc_get_identity_information_t)+sizeof(uint32_t));
    if (ginf_err)
    {
        printf ("Cannot execute ginf: %s\n", xibridge_get_err_expl(ginf_err));
        xibridge_close_device_connection(&conn);
        return false;
    }
    
    uint32_t urpc_res = (uint32_t)*resp;
    
    printf("Urpc return code: %d\n", (int)(urpc_res >> 24));
    urmc_get_identity_information_t  &info = *(urmc_get_identity_information_t *)(resp + sizeof(uint32_t));
   
    printf("Manufacture: %s\n", (char *)info.Manufacturer);
    printf("Product name: %s\n", (char *)info.ProductName);
    printf("Controller: %s\n", (char *)info.ControllerName);

    unsigned char resp_st[sizeof(urmc_status_impl_t)+sizeof(uint32_t)];
    uint32_t gets_err = xibridge_device_request_response(&conn, (const unsigned char *)"gets", 4, resp_st, sizeof(urmc_status_impl_t)+sizeof(uint32_t));
    if (gets_err)
    {
        printf("Cannot execute gets: %s\n", xibridge_get_err_expl(gets_err));
        xibridge_close_device_connection(&conn);
        return false;
    }
    
    urmc_status_impl_t &status = *(urmc_status_impl_t *)(resp_st + sizeof(uint32_t));
    printf("Current position: %u\n", status.CurPosition);
    printf("Current speed: %u\n", status.CurSpeed);

    xibridge_close_device_connection(&conn);
    return true;
}

static void thread_body(int thread_num)
{
    xibridge_conn_t conn;
    printf("Thread %u: openning connection... \n", thread_num);
    xibridge_open_device_connection(_DEV_IP, &conn);
    printf("Thread %u: connection opened, conn_id: %u \n", thread_num, conn.conn_id);
    unsigned char resp[sizeof(urmc_get_identity_information_t)+sizeof(uint32_t)];
    printf("Thread %u: sending ginf... \n", thread_num);
    uint32_t ginf_err = xibridge_device_request_response(&conn, (const unsigned char *)"ginf", 4, resp, sizeof(urmc_get_identity_information_t)+sizeof(uint32_t));
    
    printf("Thread %u: ginf return %s\n", thread_num, 
             ginf_err == 0 ? "true" : "false");
    printf("Thread %u: closing connection %u... \n", thread_num, conn.conn_id);
    xibridge_close_device_connection(&conn);
    printf("Thread %u: Connection %u closed \n", thread_num, conn.conn_id);
}

void xinet_xibridge_threads()
{
    std::thread *pthreads[TH_NUM];

    for (auto i = 0; i < TH_NUM; i++)
    {
        pthreads[i] = new std::thread(thread_body, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    for (auto i = 0; i < TH_NUM; i++)
    {
        pthreads[i]->join();
    }

    for (auto i = 0; i < TH_NUM; i++)
    {
        delete pthreads[i];
    }

    // to finish all threads
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}