#include <zf_log.h>
#include <../common/protocols.h>
#include "../common/defs.h"


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


static bool test_request_proto1()
{
	ZF_LOGD("?test_request_proto1?");

	bool ret = true;
	Protocol1 proto(false);
	bvector reqw = proto.create_version_request(0);
	if (reqw.size() != 0) 
	{
		ZF_LOGE("!Failed create_version_request(0)");
		ret = false;
	}

	reqw = proto.create_open_request(1, 1000);
	
	const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_1_OPEN, proto.get_cmd_shema());
	if (!cm1.is_match(reqw.data(), reqw.size(), 1, 1))
	{
		ZF_LOGE("!Failed create_open_request(1, 1000)");
		ret = false;
	}

	reqw = proto.create_close_request(2, 2000);

	const cmd_schema & cm2= cmd_schema::get_schema(PROTO_1_CLOSE, proto.get_cmd_shema());
	if (!cm2.is_match(reqw.data(), reqw.size(), 1, 2))
	{
		ZF_LOGE("!Failed create_close_request(2, 2000)");
		ret = false;
	}

	bvector data = { 'h', 'a', 'h', 'a', 'h', 'a' , '1'};
	
	reqw = proto.create_cmd_request(3, 3000, &data);
	const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_1_RAW, proto.get_cmd_shema());
	if (!cm3.is_match(reqw.data(), reqw.size(), 1, 3))
	{
		ZF_LOGE("!Failed create_cmd_request(3, 3000)");
		ret = false;
	}

	reqw = proto.create_enum_request(4000);
	const cmd_schema & cm4 = cmd_schema::get_schema(PROTO_1_ENUM, proto.get_cmd_shema());
	if (!cm4.is_match(reqw.data(), reqw.size(), 1, 0))
	{
		ZF_LOGE("!Failed create_enum_request(4000)");
		ret = false;
	}

	if (ret == true)
		ZF_LOGD("test_request_proto1 is OK");
	return ret;
}

static bool test_request_proto2()
{
	ZF_LOGD("?test_request_proto2?");

	bool ret = true;
	Protocol2 proto(false);
	bvector reqw = proto.create_version_request(0);
	if (reqw.size() != 0)
	{
		ZF_LOGE("!Failed create_version_request(0)");
		ret = false;
	}

	reqw = proto.create_open_request(1, 1000);

	const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_2_OPEN, proto.get_cmd_shema());
	if (!cm1.is_match(reqw.data(), reqw.size(), 2, 1))
	{
		ZF_LOGE("!Failed create_open_request(1, 1000)");
		ret = false;
	}

	reqw = proto.create_close_request(2, 2000);

	const cmd_schema & cm2 = cmd_schema::get_schema(PROTO_2_CLOSE, proto.get_cmd_shema());
	if (!cm2.is_match(reqw.data(), reqw.size(), 2, 2))
	{
		ZF_LOGE("!Failed create_close_request(2, 2000)");
		ret = false;
	}

	bvector data = { 'h', 'a', 'h', 'a', 'h', 'a' , '2' };

	reqw = proto.create_cmd_request(3, 3000, &data);
	const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_2_CMD, proto.get_cmd_shema());
	if (!cm3.is_match(reqw.data(), reqw.size(), 2, 3))
	{
		ZF_LOGE("!Failed create_cmd_request(3, 3000)");
		ret = false;
	}

	reqw = proto.create_enum_request(4000);
	if (reqw.size() != 0)
	{
		ZF_LOGE("!Failed create_enum_request(4000)");
		ret = false;
	}

	if (ret == true)
		ZF_LOGD("test_request_proto2 is OK");
	return ret;
}

static bool test_request_proto3()
{
	ZF_LOGD("?test_request_proto3?");

	bool ret = true;
	Protocol3 proto(false);
	bvector reqw = proto.create_version_request(0);
	const cmd_schema & cm0 = cmd_schema::get_schema(PROTO_3_VER, proto.get_cmd_shema());
	if (!cm0.is_match(reqw.data(), reqw.size(), 3, 0))
	{
		ZF_LOGE("!Failed create_version_request(0)");
		ret = false;
	}

	reqw = proto.create_open_request(1, 1000);

	const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_3_OPEN, proto.get_cmd_shema());
	if (!cm1.is_match(reqw.data(), reqw.size(), 3, 1))
	{
		ZF_LOGE("!Failed create_open_request(1, 1000)");
		ret = false;
	}

	reqw = proto.create_close_request(2, 2000);

	const cmd_schema & cm2 = cmd_schema::get_schema(PROTO_3_CLOSE, proto.get_cmd_shema());
	if (!cm2.is_match(reqw.data(), reqw.size(), 3, 2))
	{
		ZF_LOGE("!Failed create_close_request(2, 2000)");
		ret = false;
	}

	bvector data = { 'h', 'a', 'h', 'a', 'h', 'a', '3' };

	reqw = proto.create_cmd_request(3, 3000, &data);
	const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_3_CMD, proto.get_cmd_shema());
	if (!cm3.is_match(reqw.data(), reqw.size(), 3, 3))
	{
		ZF_LOGE("!Failed create_cmd_request(3, 3000)");
		ret = false;
	}

	reqw = proto.create_enum_request(4000);
	const cmd_schema & cm4 = cmd_schema::get_schema(PROTO_3_ENUM, proto.get_cmd_shema());
	if (!cm4.is_match(reqw.data(), reqw.size(), 3, 0))
	{
		ZF_LOGE("!Failed create_enum_request(4000)");
		ret = false;
	}

	if (ret == true)
		ZF_LOGD("test_request_proto3 is OK");
	return ret;
}



ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

extern  bool test_connect_2();

int main(int argc, char *argv[])
{
	zf_log_set_output_level(ZF_LOG_DEBUG);

	ZF_LOGD("This is first proto_test!");
	bool ret = test_request_proto1();
	if (!test_request_proto2())
		ret = false;
	if (!test_request_proto3())
		ret = false;

	test_connect_2();

	return ret == true ? 0 : 1;
}