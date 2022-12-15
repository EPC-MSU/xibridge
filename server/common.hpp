#ifndef URPC_DEVXINET_COMMON_HPP
#define URPC_DEVXINET_COMMON_HPP

#include <chrono>
#include <cstdint>
#include <cstring>
#include <cstddef>

#include "bindy/bindy-static.h"

#define conn_id_invalid 0
#define URPC_XINET_PROTOCOL_VERSION 0x00000002

typedef uint32_t conn_id_t;

enum urpc_packet_type_t {
    URPC_OPEN_DEVICE_REQUEST_PACKET_TYPE = 0x00000001,
    URPC_OPEN_DEVICE_RESPONSE_PACKET_TYPE = 0x000000FF,
    URPC_CLOSE_DEVICE_REQUEST_PACKET_TYPE = 0x00000002,
    URPC_CLOSE_DEVICE_RESPONSE_PACKET_TYPE = 0x000000FE,
    URPC_COMMAND_REQUEST_PACKET_TYPE = 0x00000003,
    URPC_COMMAND_RESPONSE_PACKET_TYPE = 0x000000FD,
    URPC_ERROR_PACKET_TYPE = 0x00000004,
};

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

PACK(
struct urpc_xinet_common_header_t {
	uint32_t protocol_version;
	uint32_t packet_type;
	uint32_t reserved3;
	uint32_t serial;
	uint32_t reserved4 [2];
});

size_t adaptive_wait_send(bindy::Bindy* bindy, conn_id_t conn_id, std::vector<uint8_t> data, size_t timeout_ms);
inline void read_uint32(uint32_t * value, uint8_t * p) {
    *value = ((uint32_t)(p[0]<<24)) | ((uint32_t)(p[1]<<16)) | ((uint32_t)(p[2]<<8)) | ((uint32_t)(p[3]<<0));
}
inline void read_uint16(uint16_t * value, uint8_t * p) {
    *value = ((uint16_t)(p[0]<<8)) | ((uint16_t)(p[1]<<0));
}
inline void read_uint8(uint8_t * value, uint8_t * p) {
    *value = (uint8_t)p[0];
}
inline void write_uint16(uint8_t * p, uint16_t value) {
    *(p+0) = ((value & 0xFF00) >> 8);
    *(p+1) = ((value & 0x00FF) >> 0);
}
inline void write_uint32(uint8_t * p, uint32_t value) {
    *(p+0) = ((value & 0xFF000000) >> 24);
    *(p+1) = ((value & 0x00FF0000) >> 16);
    *(p+2) = ((value & 0x0000FF00) >> 8);
    *(p+3) = ((value & 0x000000FF) >> 0);
}
inline void write_bytes(uint8_t * p, const uint8_t * src, uint32_t size) { // does not check target length
    memcpy(p, src, size);
}
inline void write_bool(uint8_t * p, bool value) {
    *p = value;
}

#endif //URPC_DEVXINET_COMMON_HPP
