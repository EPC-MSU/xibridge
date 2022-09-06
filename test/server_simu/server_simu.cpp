#include <iostream>
#include "../common/defs.h"
#include <../common/protocols.h>
#include <bindy/bindy-static.h>
#include <zf_log.h>

/*
* Commnads ids - needed for testing
*/
#define PROTO_1_OPEN  1
#define PROTO_1_CLOSE 2 
#define PROTO_1_RAW   0 
#define PROTO_1_ENUM  3

#define PROTO_2_OPEN  1
#define PROTO_2_CLOSE 2
#define PROTO_2_CMD   3

#define PROTO_3_OPEN  1
#define PROTO_3_CLOSE 2
#define PROTO_3_CMD   3
#define PROTO_3_VER   5
#define PROTO_3_ENUM  4



// ========================================================
void callback_data(conn_id_t conn_id, std::vector<uint8_t> data) {
    ZF_LOGD("From %u received packet of length: %lu.", conn_id, data.size());

    if (data.size() < 16) { // We need at least the protocol version and command code... and serial too
        ZF_LOGE("From %u received incorrect data packet: Size: %lu, expected at least 16.", conn_id, data.size());
        throw std::runtime_error("Incorrect data packet");
    }

}

/*
server simulation object

*/


class server_simu
{
public:
    server_simu(bindy::Bindy *pb) :
        _pbindy(pb)
    {
        _version = { 3, 0, 0 };
    }
private:
    bindy::Bindy *_pbindy;
    xibridge_version_t _version;
};

void start_server()
{
    try{
        bindy::Bindy bindy("", true, false);
        bindy.connect();
        server_simu smu(&bindy);
        bindy.set_handler(&callback_data);

    }
    catch (std::exception &e)
    {
        ZF_LOGE("Exception while server simulator initializing! Details: %s", e.what());
    }
    
}

ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

void main()
{
    zf_log_set_output_level(ZF_LOG_DEBUG);
    start_server();
   
}
