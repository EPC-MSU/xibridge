#include "../common/defs.h"
#include <zf_log.h>
#include <../common/protocols.h>


/* 
* The next definition is replaced to xibridge
*/
// // ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

/*
* extern functions to be used in test
*/
extern bool test_protocols(); 
extern bool test_connect_1();
extern bool test_connect_2();
extern void test_connect_2_threads();
extern void test_connect_1_threads();

extern uint32_t xibridge_parse_uri_dev12(const char *uri, xibridge_parsed_uri *parsed_uri);


/*
Temporary wrapper as a solution for finding key in libximc using c++ functions.
*/
bool find_key(const char* hints, const char* key, char* buf, unsigned int length)
{
    if (hints == NULL) return FALSE;
    char *s, *ptr, *ptoc;
    int key_count, i, len;
    char delim, eq;
    bool ret;
    s = (char *)malloc(strlen(hints) + 1);
    memcpy(s, hints, strlen(hints));
    s[strlen(hints)] = 0;

    delim = ',';
    eq = '=';
    key_count = 0;
    ret = true;
    do 
    { // exit when no new item is found in strrchr() function
        ptr = strrchr(s, eq);
        if (ptr == NULL) break;
        key_count++;
        while (ptr != s && *ptr != delim) // find the nearest left delimiter
            ptr--;
        if (ptr == s) break;
        *ptr-- = 0;
    } while (1);
    ptr = s;
    for (i = 0; i < key_count; i++)
    {
        len = strlen(ptr);
        ptoc = ptr;
        while (*ptoc == ' ') ptoc++;
        if (portable_strncasecmp(ptoc, key, strlen(key)) == 0 && strchr (ptoc, eq) != NULL)
        {
            ptoc += strlen(key);
            while (*ptoc == ' ') ptoc++;
            if (*ptoc++ == eq)
            {
                while (*ptoc == ' ') ptoc++;
                if (length < strlen(ptoc) + 1) ret = false;
                else  memcpy(buf, ptoc, strlen(ptoc) + 1);
                free(s);
                return ret;
            }
        }
        ptr += (len + 1);
    }
    free(s);
    return false;
}

bool test_find_key()
{
    ZF_LOGD("Starting test_find_key...");
    char * hints = "addr= abb, c,dd, xi-net=  888, 999, ";
    char *hints_empty = "addr=";
    char *bad_hints = " addr = 8 = 9";
    char result[128];
    if (!find_key(hints, "addr", result, 128))
        return false;
    if (!find_key(hints, "xi-net", result, 128))
        return false;
    if (!find_key(hints_empty, "addr", result, 128))
        return false;
    if (!find_key(bad_hints, "addr", result, 128))
        return false;
    return true;
}


bool test_xibridge_uri_parse()
{
	xibridge_parsed_uri parsed;
	ZF_LOGD("Starting test_xibridge_uri_parse...");
	bool ret = true;

    if (!test_find_key())
        return false;

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

	return ret;
}


int main(int argc, char *argv[])
{
	zf_log_set_output_level(ZF_LOG_DEBUG);

	ZF_LOGD("Starting test_main...");
	bool ret = test_protocols();

	if (!test_xibridge_uri_parse())
		ret = false;
	
	//if (!test_connect_2())
	//	ret = false;
	
	// if it is something wrong at protocol matching check stage or 
	// server interaction stage - 
	// there is nothing more to do - exit with error
	if (!ret) return 1;
	//test_connect_2_threads();

    if (!test_connect_1())
    {
        ret = false;
    }
    if (!ret) return 1;
	test_connect_1_threads();
	return 0;
}