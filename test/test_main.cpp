#include <zf_log.h>
#include <../common/protocols.h>
#include "../common/defs.h"

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

int main(int argc, char *argv[])
{
	zf_log_set_output_level(ZF_LOG_DEBUG);

	ZF_LOGD("Starting test_main...");
	bool ret = test_protocols();
	
	//if (!test_connect_2())
	//	ret = false;
	
	// if it is something wrong at protocol matching check stage or 
	// server interaction stage - 
	// there is nothing more to do - exit with error
	if (!ret) return 1;

	test_connect_2_threads();

	//test_connect_2();
	return 0;
}