#include "xib_com.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <zf_log.h>
#include <libserialport.h>
#include "devserial.h"

#ifdef _MSC_VER
    #pragma warning( disable : 4311 ) // because we may cast 64-bit handle ptrs to uint32_t to use as pseudo-ids
#endif

// can be called from any thread;
xib_device_handle_t  xib_com_device_create(
    const char *uri
    )
{
    struct sp_port * dst = device_serial_create(uri);
    if (dst == NULL)
    {
        ZF_LOGE("failed to create serial device");
        return NULL;
    }
    return dst;
}

// can be called from any thread;
// calling this function after xib_device_destroy is undefined behaviour (where 'after' is defined by languages' memory model)
xib_result_t urpc_device_send_request(
    xib_device_handle_t device,
    const char cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len
    )
{
    if (device == NULL)
    {
        return xib_result_nodevice;
    }

    return urpc_device_serial_send_request(device, cid, request, request_len, response, response_len);
}

// can be called from any thread;
// calling this function after xib_device_destroy is undefined behaviour (where 'after' is defined by languages' memory model)
xib_result_t xib_com_device_send_request(
    xib_device_handle_t device,
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len
    )
{
    if (device == NULL)
    {
        return xib_result_nodevice;
    }
    return device_serial_send_request_base(device, request, request_len, response, response_len);
}

// can be called from any thread; will return only after all in-flight requests has been completed;
// calling this function more then once per device is undefined behaviour
xib_result_t xib_com_device_destroy(
    xib_device_handle_t device
    )
{
    return device_serial_destroy(device);
}
