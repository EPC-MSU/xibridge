#include "urpc.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <zf_log.h>

#include "uri.h"
#include "synchronizer.h"
#include "devserial.h"

#ifdef _MSC_VER
    #pragma warning( disable : 4311 ) // because we may cast 64-bit handle ptrs to uint32_t to use as pseudo-ids
#endif

enum urpc_device_type_t
{
    URPC_DEVICE_TYPE_SERIAL,
    URPC_DEVICE_TYPE_UNKNOWN
};


struct urpc_device_t
{
    struct urpc_synchronizer_t *sync;
    enum urpc_device_type_t type;
    union
    {
        struct urpc_device_serial_t *serial;
    } impl;
};

static enum urpc_device_type_t get_device_type_from_uri(const struct urpc_uri_t *parsed_uri)
{
    if (!portable_strcasecmp(parsed_uri->scheme, "com"))
    {
        return URPC_DEVICE_TYPE_SERIAL;
    }
    return URPC_DEVICE_TYPE_UNKNOWN;
}

// can be called from any thread;
struct urpc_device_t *
urpc_device_create(
    const char *uri
)
{
    struct urpc_uri_t parsed_uri;
    memset(&parsed_uri, 0, sizeof(struct urpc_uri_t));
    if (urpc_uri_parse(uri, &parsed_uri))
    {
        ZF_LOGE("unknown device URI %s", uri);
        return NULL;
    }
    ZF_LOGD("URI %s resolved to dt '%s', host '%s' and path '%s' param '%s'='%s'", uri, parsed_uri.scheme, parsed_uri.host, parsed_uri.path, parsed_uri.paramname, parsed_uri.paramvalue);

    struct urpc_device_t *device = malloc(sizeof(struct urpc_device_t));
    if (device == NULL)
    {
        ZF_LOGE("failed to allocate memory for device");
        goto device_malloc_failed;
    }

    if ((device->sync = urpc_syncronizer_create()) == NULL)
    {
        ZF_LOGE("failed to create synchronizer");
        goto synchronizer_create_failed;
    }

    device->type = get_device_type_from_uri(&parsed_uri);
    switch (device->type)
    {
        case URPC_DEVICE_TYPE_SERIAL:
//            if(strlen(parsed_uri.host) != 0 || strlen(parsed_uri.path) == 0)
//            {
//                ZF_LOGE("Unknown device URI, only path should be specified");
//                goto device_impl_create_failed;
//            }
            if ((device->impl.serial = urpc_device_serial_create(parsed_uri.path)) == NULL)
            {
                ZF_LOGE("failed to create serial device");
                goto device_impl_create_failed;
            }
            break;
        default:
            ZF_LOGE("unknown device type");
            goto device_impl_create_failed;
    }

    return device;

device_impl_create_failed:
    urpc_synchronizer_destroy(device->sync);

synchronizer_create_failed:
    free(device);

device_malloc_failed:
    return NULL;
}

// can be called from any thread;
// calling this function after urpc_device_destroy is undefined behaviour (where 'after' is defined by languages' memory model)
urpc_result_t urpc_device_send_request(
    struct urpc_device_t *device,
    const char cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len
)
{
    if (device == NULL)
    {
        return urpc_result_nodevice;
    }

    urpc_result_t result;

    if (urpc_synchronizer_acquire(device->sync) != 0)
    {
        ZF_LOGE("can't acquire device lock");
        return urpc_result_nodevice;
    }

    switch (device->type)
    {
        case URPC_DEVICE_TYPE_SERIAL:
            result = urpc_device_serial_send_request(device->impl.serial, cid, request, request_len, response, response_len);
            break;
        default:
            result = urpc_result_error;
            break;
    }

    if (urpc_synchronizer_release(device->sync) != 0)
    {
        ZF_LOGE("can't release device lock");
        return urpc_result_error;
    }

    return result;
}

    // can be called from any thread;
    // calling this function after urpc_device_destroy is undefined behaviour (where 'after' is defined by languages' memory model)
    urpc_result_t urpc_device_send_request_base(
    struct urpc_device_t *device,
        const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len
        )
    {
        if (device == NULL)
        {
            return urpc_result_nodevice;
        }

        urpc_result_t result;

        if (urpc_synchronizer_acquire(device->sync) != 0)
        {
            ZF_LOGE("can't acquire device lock");
            return urpc_result_nodevice;
        }

        switch (device->type)
        {
        case URPC_DEVICE_TYPE_SERIAL:
            result = urpc_device_serial_send_request_base(device->impl.serial, request, request_len, response, response_len);
            break;
        default:
            result = urpc_result_error;
            break;
        }

        if (urpc_synchronizer_release(device->sync) != 0)
        {
            ZF_LOGE("can't release device lock");
            return urpc_result_error;
        }

        return result;
    }

// can be called from any thread; will return only after all in-flight requests has been completed;
// calling this function more then once per device is undefined behaviour
urpc_result_t urpc_device_destroy(
    struct urpc_device_t **device_ptr
)
{
    struct urpc_device_t *device = *device_ptr;
    if (device == NULL)
    {
        return urpc_result_nodevice;
    }

    if (urpc_synchronizer_destroy(device->sync) != 0)
    {
        ZF_LOGE("can't destroy device lock");
        return urpc_result_error;
    }

    urpc_result_t result;
    switch (device->type)
    {
        case URPC_DEVICE_TYPE_SERIAL:
            result = urpc_device_serial_destroy(&device->impl.serial);
            break;
        default:
            result = urpc_result_error;
            break;
    }
    if (result != urpc_result_ok)
    {
        return result;
    }
    free(device);
    *device_ptr = NULL;
    return urpc_result_ok;
}
