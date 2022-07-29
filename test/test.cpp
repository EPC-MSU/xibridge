﻿#include <iostream>
#include "../common/defs.h"
#include <zf_log.h>
#include <../common/protocols.h>

/*
* extern functions to be used in test
*/
extern bool test_protocols(); 

extern uint32_t xibridge_parse_uri_dev12(const char *uri, xibridge_parsed_uri *parsed_uri);

bool test_xibridge_uri_parse()
{
	xibridge_parsed_uri parsed;
	ZF_LOGD("Starting test_xibridge_uri_parse...");
	bool ret = true;

    //if (!test_find_key())
    //    return false;

	// test invalid uri
	if (xibridge_parse_uri_dev12("xi-net://abcd/1", &parsed) != 0)
	{
		ZF_LOGE("URI base parse (xi-net://abcd/1) failed...");
		return false;
		// nothing to do
	}
	
	if (parsed.uri_device_id.id != 1)
	{
		ZF_LOGE("xi-net://abcd/1: invalid device id %u", parsed.uri_device_id.id);
		ret = false;
	}

	if (parsed.uri_device_id.VID != 0 || parsed.uri_device_id.PID != 0 || parsed.uri_device_id.reserve != 0)
	{
		ZF_LOGE("xi-net://abcd/1: non zero device_id fields.");
		ret = false;
	}

	if (xibridge_parse_uri_dev12("xi-net://0.0.0.0/000000df00DF00dFAAAAAAAA", &parsed) != 0)
	{
		ZF_LOGE("URI base parse (xi-net://0.0.0.0/000000df00DF00dFAAAAAAAA) failed...");
		return false;
		// nothing to do
	}

	if (parsed.uri_device_id.id != 0xAAAAAAAA)
	{
		ZF_LOGE("xi-net://abcd/1: invalid device id %u", parsed.uri_device_id.id);
		ret = false;
	}

	if (parsed.uri_device_id.VID != 0xDF)
	{
		ZF_LOGE("xi-net://abcd/1: invalid device VID %u", parsed.uri_device_id.VID);
		ret = false;
	}

	if (parsed.uri_device_id.PID != 0xDF)
	{
		ZF_LOGE("xi-net://abcd/1: invalid device PID %u", parsed.uri_device_id.PID);
		ret = false;
	}

	if (parsed.uri_device_id.reserve!= 0xDF)
	{
		ZF_LOGE("xi-net://abcd/1: invalid device reserve %u", parsed.uri_device_id.reserve);
		ret = false;
	}
    if (ret)
    {
        ZF_LOGD("test_xibridge_uri_parse OK");
    }
    return ret;
}


int main(int /*argc*/, char ** /*argv[]*/)
{
	zf_log_set_output_level(ZF_LOG_DEBUG);

	ZF_LOGD("Starting test_main...");
	bool ret = test_protocols();

	if (!test_xibridge_uri_parse())
		ret = false;

    if (!ret) return 1;

    return 0;
}