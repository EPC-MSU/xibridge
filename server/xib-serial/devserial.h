#ifndef URPC_DEVSERIAL_DEVSERIAL_H
#define URPC_DEVSERIAL_DEVSERIAL_H

#include "xib_com.h"


struct device_serial_t;

struct device_serial_t *
device_serial_create(
    const char *path
);


urpc_result_t
device_serial_send_request_base(
    struct device_serial_t *device,
        const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len
        );

urpc_result_t
device_serial_destroy(
    struct device_serial_t **device_ptr
);

#endif //URPC_DEVSERIAL_DEVSERIAL_H
