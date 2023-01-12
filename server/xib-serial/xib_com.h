#ifndef XIB_COM_H
#define XIB_COM_H

#include <stdio.h>
#include <stdint.h>

struct xib_device_t;
typedef struct xib_device_t *xib_device_handle_t;
typedef int xib_result_t;


//urpc-server simulating mode
#define URPC_CID_SIZE 4
#define URPC_CRC_SIZE 2

#define xib_result_ok 0
#define xib_result_error (-1)
#define xib_result_value_error (-3)
#define xib_result_nodevice (-4)
#define xib_result_timeout (-5)

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
xib_device_handle_t
xib_com_device_create(
    const char *uri
);

xib_result_t
urpc_device_send_request(
xib_device_handle_t device,
const char cid[URPC_CID_SIZE],
const uint8_t *request,
uint8_t request_len,
uint8_t *response,
uint8_t response_len
);

xib_result_t
xib_com_device_send_request(
xib_device_handle_t device,
const uint8_t *request,
uint8_t request_len,
uint8_t *response,
uint8_t response_len
);

xib_result_t
xib_com_device_destroy(
    xib_device_handle_t *device_ptr
);

#ifdef __cplusplus
}
#endif

#endif //URPC_H
