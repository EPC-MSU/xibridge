#include <thread>
#include <../common/protocols.h>
#include <../xibridge.h>
#include "urmc-min/urmc_min.h"
#include "../common/utils.h"

#define TH_NUM 5

static char  _DEV_IP[256];

/* 
* In fact it's the same urpc-device, but via protocol 3 

*/

// urpc cid size
#define CID_SIZE 4
#define CRC_SIZE  2
static uint16_t get_crc(const uint8_t *pbuf, size_t n)
{
    uint16_t crc, carry_flag, a;
    size_t i, j;
    crc = 0xffff;
    for (i = 0; i < n; i++)
    {
        crc = crc ^ pbuf[i];
        for (j = 0; j < 8; j++)
        {
            a = crc;
            carry_flag = a & 0x0001;
            crc = crc >> 1;
            if (carry_flag == 1)
            {
                crc = crc ^ 0xA001;
            }
        };
    }
    return crc;
}

bool xinet_xibridge_usage_example(const char * ip, uint32_t dev_num)
{
    sprintf(_DEV_IP, "xi-net://%s/%x", ip, dev_num);
    xibridge_conn_t conn;

    xibridge_version_t real_server_version;

    uint32_t err = xibridge_get_server_last_protocol_version(_DEV_IP, &real_server_version);

    if (err)
    {
        printf("Cannot get xibridge version (execute version request) %s: %s\n", _DEV_IP, xibridge_get_err_expl(err));
        return false;
    }

    printf("Server last protocol version is %u.%u.%u\n", (unsigned int)real_server_version.major, (unsigned int)real_server_version.minor, (unsigned int)real_server_version.bagfix);
    xibridge_set_base_protocol_version(real_server_version);

    err = xibridge_open_device_connection(_DEV_IP, &conn);
    if (err)
    {
        printf ("Cannot open %s: %s\n", _DEV_IP, xibridge_get_err_expl(err));
        xibridge_close_device_connection(&conn);
        return false;
    }
    // urpc ginf sending...
    unsigned char resp[sizeof(urmc_get_identity_information_t) + CID_SIZE + CRC_SIZE];
    memset(resp, 0, sizeof(urmc_get_identity_information_t) + CID_SIZE + CRC_SIZE);

    uint8_t req_urpc[CID_SIZE + CRC_SIZE];
    memcpy(req_urpc, "ginf", CID_SIZE);
    uint16_t crc = get_crc(req_urpc, CID_SIZE);
    memcpy(req_urpc+CID_SIZE, (void *)&crc, CRC_SIZE);
   
    uint32_t ginf_err = xibridge_device_request_response(&conn, req_urpc, CID_SIZE + CRC_SIZE, resp, sizeof(urmc_get_identity_information_t) + CID_SIZE + CRC_SIZE);
    if (ginf_err)
    {
        printf ("Cannot execute ginf: %s\n", xibridge_get_err_expl(ginf_err));
        xibridge_close_device_connection(&conn);
        return false;
    }
         
    urmc_get_identity_information_t  &info = *(urmc_get_identity_information_t *)(resp + CID_SIZE);
   
    printf("Manufacture: %s\n", (char *)info.Manufacturer);
    printf("Product name: %s\n", (char *)info.ProductName);
    printf("Controller: %s\n", (char *)info.ControllerName);

    // urpc gets sending...
    unsigned char resp_st[sizeof(urmc_status_impl_t)+CID_SIZE + CRC_SIZE];
    memset(resp_st, 0, sizeof(urmc_status_impl_t)+CID_SIZE + CRC_SIZE);

    memcpy(req_urpc, "gets", CID_SIZE);
    crc = get_crc(req_urpc, CID_SIZE);
    memcpy(req_urpc + CID_SIZE, (void *)&crc, CRC_SIZE);

    uint32_t gets_err = xibridge_device_request_response(&conn, req_urpc, CID_SIZE + CRC_SIZE, resp_st, sizeof(urmc_status_impl_t) + CID_SIZE + CRC_SIZE);
    if (gets_err)
    {
        printf("Cannot execute gets: %s\n", xibridge_get_err_expl(gets_err));
        xibridge_close_device_connection(&conn);
        return false;
    }
    
    urmc_status_impl_t &status = *(urmc_status_impl_t *)(resp_st + CID_SIZE);
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
    unsigned char resp[sizeof(urmc_get_identity_information_t) + CID_SIZE];
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