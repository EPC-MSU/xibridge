#ifndef URPC_H
#define URPC_H

#include <stdio.h>
#include <stdint.h>

struct urpc_device_t;
typedef struct urpc_device_t *urpc_device_handle_t;
typedef int urpc_result_t;

#define URPC_XINET_VERSION_MAJOR  1
#define URPC_XINET_VERSION_MINOR  1
#define URPC_XINET_VERSION_BUGFIX 3

#define URPC_CID_SIZE 4
#define URPC_CRC_SIZE 2

#define urpc_result_ok 0
#define urpc_result_error (-1)
#define urpc_result_value_error (-3)
#define urpc_result_nodevice (-4)
#define urpc_result_timeout (-5)

#ifdef __cplusplus
extern "C" {
#endif


/* Open a port by URI
 * examples:
*   com:///COM3
*   com:///\\.\COM12
*   com:COM3
*   com:\\.\COM12
*   com:///dev/tty/ttyACM34
*   emu:///var/lib/ximc/virtual56.dat
*   emu:///c:/temp/virtual56.dat
*   emu:///c:/temp/virtual56.dat?serial_impl=123
*   xi-net://127.0.0.1/7890ABCD
*   xi-net://remote.ximc.ru/7890ABCD
 */
urpc_device_handle_t
urpc_device_create(
    const char *uri
);

urpc_result_t
urpc_device_send_request(
    urpc_device_handle_t device,
    const char cid[URPC_CID_SIZE],
    const uint8_t *request,
    uint8_t request_len,
    uint8_t *response,
    uint8_t response_len
);

urpc_result_t
urpc_device_destroy(
    urpc_device_handle_t *device_ptr
);

#ifdef __cplusplus
}
#endif

#endif //URPC_H
