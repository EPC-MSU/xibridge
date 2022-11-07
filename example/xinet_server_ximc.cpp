#include <../common/protocols.h>
#include <../xibridge.h>
#include "ximc-min/ximc_min.h"
#include "../common/utils.h"

#define TH_NUM 10

static char  _DEV_IP[256];

PACK(
struct _gets_re
{
    char gets[4];
    status_t status;
});

typedef struct _gets_re re_gets;

PACK(
struct _geng_re
{
    char gets[4];
    engine_settings_t settings;
});

typedef struct _geng_re re_geng;

bool xinet_ximc_usage_example(const char *ip, uint32_t dev_num)
{
    sprintf(_DEV_IP, "xi-net://%s/%x", ip, dev_num);
    char  *pdata; uint32_t count;

    uint32_t err = xibridge_enumerate_adapter_devices(ip, "", &pdata, &count);
    if (err)
    {
        printf("Cannot enumerare device: %s, error: %s\n", _DEV_IP, xibridge_get_err_expl(err));
        return false;
    }     
    printf("Count of enumerated devices: %u\n", count);
    if (count)
    {
        const char *p = pdata;
        for (int i = 0; i < (int)count; i++)
        {
            printf("Enumerated device #%d: URI: %s\n", i + 1, p);
            p = strchr(p, 0) + 1;
        }
    }
    xibridge_free_enumerate_devices(pdata);

    xibridge_conn_t conn;

    err = xibridge_open_device_connection(_DEV_IP, &conn);
    if (err)
    {
        printf("Cannot open device: %s, error: %s\n", _DEV_IP,                  xibridge_get_err_expl(err));
        xibridge_close_device_connection(&conn);
        return false;
    }
   
    re_gets status;
    uint32_t err_op = xibridge_device_request_response(&conn, (const uint8_t *)"gets", 4, (uint8_t *)&status, sizeof(re_gets));
    if (err_op)
    {
        printf("Cannot execute gets: %s\n", xibridge_get_err_expl(err_op));
        xibridge_close_device_connection(&conn);
        return false;
    }

    printf("Speed: %d\n", status.status.CurSpeed);
    
    re_geng settings;
    err_op = xibridge_device_request_response(&conn, (const uint8_t *)"geng", 4, (uint8_t *)&settings, sizeof(re_geng));
    if (err_op)
    {
        printf("Cannot execute geng: %s\n", xibridge_get_err_expl(err_op));
        xibridge_close_device_connection(&conn);
        return false;
    }
    printf("Nom voltage: %u\n", settings.settings.NomVoltage);
    xibridge_close_device_connection(&conn);
    return true;
}

static void thread_body(int thread_num)
{
    printf("Thread %u: openning connection... \n", thread_num);
    xibridge_conn_t conn;
    xibridge_open_device_connection(_DEV_IP, &conn);
    printf("Thread %u: connection opened, conn_id: %u \n", thread_num, conn.conn_id);
    printf("Thread %u: sending gets... \n", thread_num);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    status_t status;
    uint32_t err = xibridge_device_request_response(&conn, (const uint8_t *)"gets", 3, (uint8_t *)&status, sizeof(status_t));

    printf("Thread %u: gets return %s\n", thread_num,
        err == 0 ? "true" : "false");
    printf("Thread %u: closing connection %u... \n", thread_num, conn.conn_id);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    xibridge_close_device_connection(&conn);
    printf("Thread %u: Connection %u closed \n", thread_num, conn.conn_id);
}

void xinet_ximc_threads()
{
    std::thread *pthreads[TH_NUM];

    for (auto i = 0; i < TH_NUM; i++)
    {
        pthreads[i] = new std::thread(thread_body, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
