#include "devserial.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <zf_log.h>

#include <libserialport.h>

#include "platform.h"


struct urpc_device_serial_t
{
   // urpc_handle_t handle;
   struct sp_port * handle_port;
};

static uint8_t errv_cid[URPC_CID_SIZE] = { 'e', 'r', 'r', 'v' };
static uint8_t errd_cid[URPC_CID_SIZE] = { 'e', 'r', 'r', 'd' };

static urpc_result_t command_port_send(struct sp_port *handle_port, const uint8_t *command, size_t command_len)
{
    enum sp_return result;
    result = sp_blocking_write(handle_port, command, command_len, URPC_PORT_TIMEOUT);
    if (result < 0)
    {
        if (result == SP_ERR_FAIL)
        {
            return urpc_result_nodevice;
        }
        else
        {
            return sp_flush(handle_port, SP_BUF_OUTPUT) != SP_OK ? urpc_result_nodevice : urpc_result_error;
        }
    }

    return urpc_result_ok;
}

static int command_port_receive(struct sp_port *handle_port, uint8_t *response, size_t response_len)
{
    enum sp_return result;
   
    result = sp_blocking_read(handle_port, response, response_len, URPC_PORT_TIMEOUT);
    if (result < 0)
    {
        if (result == SP_ERR_FAIL )
        {
            return urpc_result_nodevice;
        }
        else
        {
            return sp_flush(handle_port, SP_BUF_INPUT) != SP_OK ? urpc_result_nodevice : urpc_result_error;
        }
    }

    ZF_LOGD_MEM(response, (unsigned int)response_len, "response ");

    return urpc_result_ok;
}

static int wallclock_diff(time_t sec_beg, int msec_beg, time_t sec_end, int msec_end)
{
    int delta = 0;
    if (sec_end > sec_beg)
    {
        /* be cautious */
        delta = (int) (sec_end - sec_beg);
    }
    delta *= 1000;
    delta += msec_end - msec_beg;
    if (delta < 0)
    {
        delta = 0;
    }
    return delta;
}

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

static int send_synchronization_zeroes(struct sp_port *handle_port)
{
    int received = URPC_ZEROSYNC_BURST_SIZE;
    uint8_t zeroes[URPC_ZEROSYNC_BURST_SIZE];
    memset(zeroes, 0, URPC_ZEROSYNC_BURST_SIZE);

    ZF_LOGI("zerosync: sending sync zeroes");


    if (command_port_send(handle_port, zeroes, URPC_ZEROSYNC_BURST_SIZE) != urpc_result_ok)
    {
        ZF_LOGE("zerosync: command_port_send sync failed");
        return 1;
    }

    while (received > 0)
    {
        if (command_port_receive(handle_port, zeroes, 1) != urpc_result_ok)
        {
            ZF_LOGE("zerosync: command_port_receive can't get uint8_ts");
            return 1;
        }
        if (zeroes[0] == 0)
        {
            ZF_LOGI("zerosync: got a zero, done");
            return 0;
        }
        --received;
    }

    return 1;
}

static int zerosync(struct sp_port *handle_port)
{
    int retry_counter = URPC_ZEROSYNC_RETRY_COUNT;

    ZF_LOGI("zerosync: started");
    while (retry_counter > 0)
    {
        if (send_synchronization_zeroes(handle_port) == 0)
        {
            ZF_LOGI("zerosync: completed");
            return 0;
        }
        --retry_counter;
    }
    ZF_LOGE("zerosync: synchronization attempts failed, device is lost");
    return 1;
}

static urpc_result_t receive(struct sp_port *handle_port, uint8_t *response, size_t len)
{
    urpc_result_t result;
    int delta_time = 0;

    time_t sec_beg, sec_cur, sec_prev;
    int msec_beg, msec_cur, msec_prev;

    urpc_get_wallclock(&sec_beg, &msec_beg);
    sec_prev = sec_beg;
    msec_prev = msec_beg;

    do
    {
        result = command_port_receive(handle_port, response, len);
        urpc_get_wallclock(&sec_cur, &msec_cur);

        if (result == urpc_result_timeout)
        {
            ZF_LOGI("receive: receive timed out, requesting data from buffer one more time");
            if (wallclock_diff(sec_prev, msec_prev, sec_cur, msec_cur) < URPC_ZEROSYNC_RETRY_DELAY)
            {
                ZF_LOGI("receive: timed out too fast, wait a little");
                urpc_msec_sleep(URPC_ZEROSYNC_RETRY_DELAY);
            }
        }
        else
        {
            return result;
        }

        delta_time = wallclock_diff(sec_beg, msec_beg, sec_cur, msec_cur);
        sec_prev = sec_beg;
        msec_prev = msec_beg;
        ZF_LOGI("receive: passed %d msec, needed at least %d msec", delta_time, URPC_ZEROSYNC_TRIGGER_TIMEOUT);
    } while (delta_time < URPC_ZEROSYNC_TRIGGER_TIMEOUT);

    // All retries
    ZF_LOGE("receive: receive finally timed out");
    if ((result = zerosync(handle_port)) != 0)
    {
        ZF_LOGE("receive: zerosync failed.");
        result = urpc_result_nodevice;
    }

    return result;
}


struct urpc_device_serial_t *
urpc_device_serial_create(
    const char *path
)
{
    enum sp_return result;
    struct urpc_device_serial_t *device = malloc(sizeof(struct urpc_device_serial_t));
    if (device == NULL)
    {
        goto malloc_failed;
    }
    result = sp_get_port_by_name(path, & (device -> handle_port));
    if (result != SP_OK)
    {
        goto serial_port_open_failed;
    }
    if (sp_open(device->handle_port, SP_MODE_READ_WRITE) != SP_OK)
    {
        sp_free_port(device->handle_port);
        goto serial_port_open_failed;
    }

    return device;

serial_port_open_failed:
    
    free(device);

malloc_failed:
    return NULL;
}

urpc_result_t urpc_device_serial_send_request(
    struct urpc_device_serial_t *device,
    const char request_cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len
)
{
    assert(device != NULL);

    struct sp_port * handle_port = device->handle_port;
    if (request_len != 0 && !request)
    {
        ZF_LOGE("can't read from an empty buffer");
    }

    if (response_len != 0 && !response)
    {
        ZF_LOGE("can't write to empty buffer");
    }

    {
        urpc_result_t result;


        // send command
        result = command_port_send(handle_port, (const uint8_t *)request_cid, URPC_CID_SIZE);
        if (result != urpc_result_ok)
        {
            return result;
        }

        if (request_len != 0)
        {
            result = command_port_send(handle_port, request, request_len);
            if (result != urpc_result_ok)
            {
                return result;
            }
            uint16_t request_crc = get_crc(request, request_len);
            result = command_port_send(handle_port, (const uint8_t *)&request_crc, URPC_CRC_SIZE);
            if (result != urpc_result_ok)
            {
                return result;
            }
        }
    }

    {
        urpc_result_t result;
        uint16_t response_crc = 0;
        uint8_t response_cid[URPC_CID_SIZE];

        // read first uint8_t until it's non-zero
        do
        {
            if ((result = receive(handle_port, response_cid, 1)) != urpc_result_ok)
            {
                return result;
            }
        } while (response_cid[0] == 0);

        // read three uint8_ts
        if ((result = receive(handle_port, response_cid + 1, 3)) != urpc_result_ok)
        {
            return result;
        }

        // check is it an errv answer
        if (memcmp(errv_cid, response_cid, URPC_CID_SIZE) == 0)
        {
            ZF_LOGW("Response 'errv' received");
            sp_flush(handle_port, SP_BUF_BOTH);
            return urpc_result_value_error;
        }

        // check is it an errd answer
        if (memcmp(errd_cid, response_cid, URPC_CID_SIZE) == 0)
        {
            ZF_LOGW("Response 'errd' received");
            // flood the controller with zeroes
            zerosync(handle_port);
            sp_flush(handle_port, SP_BUF_BOTH);
            return urpc_result_error;
        }

        // check command uint8_ts
        if (memcmp(request_cid, response_cid, URPC_CID_SIZE) != 0)
        {
            // flood the controller with zeroes
            zerosync(handle_port);
            sp_flush(handle_port, SP_BUF_BOTH);
            return urpc_result_error;
        }

        if (response_len != 0)
        {
            // receive remaining uint8_ts
            if ((result = receive(handle_port, response, response_len)) != urpc_result_ok)
            {
                return result;
            }

            if ((result = receive(handle_port, (uint8_t *) &response_crc, URPC_CRC_SIZE)) != urpc_result_ok)
            {
                return result;
            }
            if (response_crc != get_crc(response, URPC_CRC_SIZE))
            {
                return result;
            }
        }
    }

    return urpc_result_ok;
}

urpc_result_t urpc_device_serial_destroy(
    struct urpc_device_serial_t **device_ptr
)
{
    struct urpc_device_serial_t *device = *device_ptr;
    assert(device != NULL);

    enum sp_return result = sp_close(device->handle_port);
    if (result != SP_OK)
    {
       sp_free_port(device->handle_port);
       return urpc_result_error;
    }
    sp_free_port(device->handle_port);
    free(device);

    *device_ptr = NULL;

    return urpc_result_ok;
}
