#include "devserial.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <zf_log.h>

#include <libserialport.h>

#define PORT_TIMEOUT 500

struct device_serial_t
{
   // urpc_handle_t handle;
   struct sp_port * handle_port;
};

static urpc_result_t command_port_send(struct sp_port *handle_port, const uint8_t *command, size_t command_len)
{
    enum sp_return result;
    result = sp_blocking_write(handle_port, command, command_len, PORT_TIMEOUT);
    if (result < 0)
    {
        if (result == SP_ERR_FAIL)
        {
            return urpc_result_nodevice;
        }
        else
        {
            return sp_flush(handle_port, SP_BUF_OUTPUT) != SP_OK ? urpc_result_nodevice : urpc_result_timeout;
        }
    }

    return urpc_result_ok;
}

static int command_port_receive(struct sp_port *handle_port, uint8_t *response, size_t response_len)
{
    enum sp_return result;
   
    result = sp_blocking_read(handle_port, response, response_len, PORT_TIMEOUT);
    if (result < 0)
    {
        if (result == SP_ERR_FAIL )
        {
            return urpc_result_nodevice;
        }
        else
        {
            return sp_flush(handle_port, SP_BUF_INPUT) != SP_OK ? urpc_result_nodevice : urpc_result_timeout;
        }
    }

    ZF_LOGD_MEM(response, (unsigned int)response_len, "response ");

    return urpc_result_ok;
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

struct device_serial_t *
device_serial_create(
    const char *path
)
{
    enum sp_return result;
    struct device_serial_t *device = malloc(sizeof(struct device_serial_t));
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

urpc_result_t device_serial_send_request_base(
struct device_serial_t *device,
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


    urpc_result_t result;
    // send command
    if (request_len != 0)
    {
        result = command_port_send(handle_port, request, request_len);
        if (result != urpc_result_ok)
        {
            return result;
        }
    }
    uint16_t response_crc = 0;

    if (response_len != 0)
    {
        // receive remaining uint8_ts
        if ((result = command_port_receive(handle_port, response, response_len)) != urpc_result_ok)
        {
            return result;
        }
    }
    return urpc_result_ok;
}

urpc_result_t device_serial_destroy(
    struct device_serial_t **device_ptr
)
{
    struct device_serial_t *device = *device_ptr;
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
