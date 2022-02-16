#ifndef _DEFS_H
#define  _DEFS_H

#include <bindy/bindy-static.h>

#define uint8 uint8_t
#define uint32 uint32_t
#define uint16 uint16_t
#define int32 int32_t

#define conn_id_t bindy::conn_id_t
#define conn_id_invalid  bindy::conn_id_invalid

#define bvector std::vector<uint8>

#define bmap std::map<conn_id_t, Xibridge_client *>

#endif