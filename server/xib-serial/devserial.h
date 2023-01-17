#ifndef URPC_DEVSERIAL_DEVSERIAL_H
#define URPC_DEVSERIAL_DEVSERIAL_H

#include "xib_com.h"

struct sp_port*
device_serial_create(
    const char *path
);

// urpcserver emulating response-request
xib_result_t urpc_device_serial_send_request(
struct sp_port *device,
    const char request_cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len
    );

xib_result_t
device_serial_send_request_base(
    struct sp_port *device,
        const uint8_t *request,
        uint8_t request_len,
        uint8_t *response,
        uint8_t response_len
        );

xib_result_t
device_serial_destroy(
    struct sp_port *device
);

#endif //URPC_DEVSERIAL_DEVSERIAL_H
