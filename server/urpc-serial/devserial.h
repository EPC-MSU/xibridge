#ifndef URPC_DEVSERIAL_DEVSERIAL_H
#define URPC_DEVSERIAL_DEVSERIAL_H

#include "../urpc.h"


struct urpc_device_serial_t;

struct urpc_device_serial_t *
urpc_device_serial_create(
    const char *path
);

urpc_result_t
urpc_device_serial_send_request(
    struct urpc_device_serial_t *device,
    const char request_cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len
);

urpc_result_t
urpc_device_serial_destroy(
    struct urpc_device_serial_t **device_ptr
);

#endif //URPC_DEVSERIAL_DEVSERIAL_H
