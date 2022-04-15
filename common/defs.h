#ifndef _DEFS_H
#define  _DEFS_H

#include <bindy/bindy-static.h>

#define conn_id_t bindy::conn_id_t

#define conn_id_invalid  bindy::conn_id_invalid

typedef  std::vector<uint8_t> bvector;

#define bmap std::map<conn_id_t, Xibridge_client *>



#endif