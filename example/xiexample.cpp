#include <iostream>
#include "../common/defs.h"
#include "../client/xibridge.h"
#include <zf_log.h>

// to make log level controlled
#if defined(BUILD_SHARED_LIBS_XI)
   ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;
#endif
/*
* extern functions to be used in test
*/
extern bool test_connect_1(const char *ip, uint32_t dev_num);
extern bool test_connect_2(const char *ip, uint32_t dev_num);
extern void test_connect_2_threads();
extern void test_connect_1_threads();

int main(int argc, char *argv[])
{
	zf_log_set_output_level(ZF_LOG_DEBUG);
	bool ret = true;
	ZF_LOGD("Starting test_main...");

    std::cout << "Enter ip-address for ximc xinet-server if you intend to test xibridge with it (or type 'N' if not):\n";

    std::string ip_s;
    uint32_t dev_num;
    std::cin >> ip_s;
    if (!ip_s.empty() && ip_s[0] != 'N' && ip_s[0] != 'n' )
    {
        std::cout << "Enter device serial number to be used with ximc-server:\n";
        std::cin >> dev_num;
        if (!test_connect_1(ip_s.data(), dev_num))
        {
			ret = false;
        }    
		if (!ret) return 1;
        test_connect_1_threads();
    }

    std::cout << "Enter ip-address for urpc xinet-server if you intend to test xibridge with it (or type 'N' if not):\n";
    std::cin >> ip_s;
	if (!ip_s.empty() && ip_s[0] != 'N' && ip_s[0] != 'n' )
    {
        std::cout << "Enter device serial number to be used with urpc-server:\n";
        std::cin >> dev_num;

        if (!test_connect_2(ip_s.data(), dev_num))
        {
            ret = false;
        }    
		if (!ret) return 1;
        test_connect_2_threads();
    }

   return 0;
}