#include <iostream>
#include "../common/defs.h"
#include <../common/protocols.h>
#include "../vendor/acutest/include/acutest.h"
#include <bindy/bindy-static.h>

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


static void test_request_proto1()
{
    TEST_MSG("?test_request_proto1?");

    uint32_t err;
    Protocol1 proto(&err, false);
    TEST_CHECK (proto.create_version_request(0).size() == 0);
    bvector  reqw = proto.create_open_request(1, 1000);

    const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_1_OPEN, proto.get_cmd_shema());
    TEST_CHECK(cm1.is_match(reqw.data(), (int)reqw.size(), 1, 1));

    reqw = proto.create_close_request(2, 2000);

    const cmd_schema & cm2 = cmd_schema::get_schema(PROTO_1_CLOSE, proto.get_cmd_shema());
    TEST_CHECK(cm2.is_match(reqw.data(), (int)reqw.size(), 1, 2));
   
    bvector data = { 'h', 'a', 'h', 'a', 'h', 'a', '1' };

    reqw = proto.create_cmd_request(3, 3000, &data);
    const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_1_RAW, proto.get_cmd_shema());
    TEST_CHECK(cm3.is_match(reqw.data(), (int)reqw.size(), 1, 3));

    reqw = proto.create_enum_request(4000);
    const cmd_schema & cm4 = cmd_schema::get_schema(PROTO_1_ENUM, proto.get_cmd_shema());
    TEST_CHECK(cm4.is_match(reqw.data(), (int)reqw.size(), 1, 0));
}

static void test_request_proto2()
{
    TEST_MSG("?test_request_proto2?");
    uint32_t err;
    Protocol2 proto(&err, false);
    TEST_CHECK(proto.create_version_request(0).size() == 0);
  
    bvector reqw = proto.create_open_request(1, 1000);

    const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_2_OPEN, proto.get_cmd_shema());
    TEST_CHECK(cm1.is_match(reqw.data(), (int)reqw.size(), 2, 1));
    reqw = proto.create_close_request(2, 2000);

    const cmd_schema & cm2 = cmd_schema::get_schema(PROTO_2_CLOSE, proto.get_cmd_shema());
    TEST_CHECK(cm2.is_match(reqw.data(), (int)reqw.size(), 2, 2));
    
    bvector data = { 'h', 'a', 'h', 'a', 'h', 'a', '2' };
    
    reqw = proto.create_cmd_request(3, 3000, &data);
    const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_2_CMD, proto.get_cmd_shema());
    TEST_CHECK(cm3.is_match(reqw.data(), (int)reqw.size(), 2, 3));
        
    TEST_CHECK(proto.create_enum_request(4000).size() == 0);
}

static void test_request_proto3()
{
    TEST_MSG("?test_request_proto3?");
    uint32_t err;
    Protocol3 proto(&err, false);
    bvector reqw = proto.create_version_request(0);
    const cmd_schema & cm0 = cmd_schema::get_schema(PROTO_3_VER, proto.get_cmd_shema());
    TEST_CHECK(cm0.is_match(reqw.data(), (int)reqw.size(), 3, 0));
    
    reqw = proto.create_open_request(DevId(1), 1000);
    const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_3_OPEN, proto.get_cmd_shema());
    TEST_CHECK(cm1.is_match(reqw.data(), (int)reqw.size(), 3, 1));
    
    reqw = proto.create_close_request(DevId(2), 2000);
    const cmd_schema & cm2 = cmd_schema::get_schema(PROTO_3_CLOSE, proto.get_cmd_shema());
    TEST_CHECK(cm2.is_match(reqw.data(), (int)reqw.size(), 3, 2));
  
    bvector data = { 'h', 'a', 'h', 'a', 'h', 'a', '3' };

    reqw = proto.create_cmd_request(DevId(3), 3000, &data);
    const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_3_CMD, proto.get_cmd_shema());
    TEST_CHECK(cm3.is_match(reqw.data(), (int)reqw.size(), 3, 3));
   
    reqw = proto.create_enum_request(4000);
    const cmd_schema & cm4 = cmd_schema::get_schema(PROTO_3_ENUM, proto.get_cmd_shema());
    TEST_CHECK(cm4.is_match(reqw.data(), (int)reqw.size(), 3, 0));
 }

void  test_protocols()
{
    acutest_verbose_level_ = 3;
    TEST_MSG("test_protocols...");
    test_request_proto1();
    test_request_proto2();
    test_request_proto3();
}

extern uint32_t xibridge_parse_uri_dev12(const char *uri, 
                                         xibridge_parsed_uri *parsed_uri);

void test_xibridge_uri_parse()
{
    xibridge_parsed_uri parsed;
    TEST_MSG("Starting test_xibridge_uri_parse...");
    TEST_CHECK(xibridge_parse_uri_dev12("xi-net://abcd/1", &parsed) == 0);
    TEST_CHECK(parsed.uri_device_id.id == 1);
    TEST_CHECK(parsed.uri_device_id.VID == 0 && parsed.uri_device_id.PID == 0 && parsed.uri_device_id.reserve == 0);
    TEST_CHECK(xibridge_parse_uri_dev12("xi-net://0.0.0.0/000000df00DF00dFAAAAAAAA", &parsed) == 0);
    TEST_CHECK(parsed.uri_device_id.id == 0xAAAAAAAA);
    TEST_CHECK(parsed.uri_device_id.VID == 0xDF);
    TEST_CHECK(parsed.uri_device_id.PID == 0xDF);
    TEST_CHECK(parsed.uri_device_id.reserve == 0xDF);
}

/*
 * server simulator must be started to do this test
*/

void test_server_3()
{

    char *pdevs;
    uint32_t count;

    uint32_t result = xibridge_enumerate_adapter_devices("127.0.0.1",
        "",
        &pdevs,
        &count);
}

void test_main()
{
    //test_server_3();

    TEST_MSG("Starting test_main...");
    test_protocols();
    test_xibridge_uri_parse();
}

TEST_LIST = {
    { "test", test_main },
    
    { NULL, NULL }     /* zeroed record marking the end of the list */
};
