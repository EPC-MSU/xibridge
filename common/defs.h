#ifndef _DEFS_H
#define  _DEFS_H

#include <bindy/bindy-static.h>
#include <vector>
#include <string>
#include <string.h>
#include <stdint.h>

#ifndef conn_id_t
  #define conn_id_t bindy::conn_id_t
#endif

#ifndef conn_id_invalid
    #define conn_id_invalid  bindy::conn_id_invalid
#endif

typedef  std::vector<uint8_t> bvector;

#define bmap std::map<conn_id_t, Xibridge_client *>

#endif