﻿#include <iostream>
#include "../common/defs.h"
#include "../include/xibridge.h"
#include <zf_log.h>

ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

extern bool xinet_ximc_usage_example(const char *ip, uint32_t dev_num);
extern bool xinet_urpc_usage_example(const char *ip, uint32_t dev_num);
extern void xinet_ximc_threads();
extern void xinet_urpc_threads();

int main(int /*argc*/, char ** /*argv[]*/)
{
    zf_log_set_output_level(ZF_LOG_WARN);
    bool ret = true;
    std::cout << "Starting..." << std::endl;

    std::cout << "Enter ip-address for ximc xinet-server if you intend to test xibridge with it (or type 'N' if not):\n";

    std::string ip_s;
    uint32_t dev_num;
    std::cin >> ip_s;
    if (!ip_s.empty() && ip_s[0] != 'N' && ip_s[0] != 'n' )
    {
        std::cout << "Enter device serial number to be used with ximc-server (decimal unsigned number):\n";
        std::cin >> dev_num;
        if (!xinet_ximc_usage_example(ip_s.data(), dev_num))
        {
            ret = false;
        }    
        if (!ret) return 1;
        //xinet_1_threads();
    }

    std::cout << "Enter ip-address for urpc xinet-server if you intend to test xibridge with it (or type 'N' if not):\n";
    std::cin >> ip_s;
    if (!ip_s.empty() && ip_s[0] != 'N' && ip_s[0] != 'n' )
    {
        std::cout << "Enter device serial number to be used with urpc-server  (decimal unsigned number):\n";
        std::cin >> dev_num;

        if (!xinet_urpc_usage_example(ip_s.data(), dev_num))
        {
            ret = false;
        }    
        if (!ret) return 1;
        //xinet_2_threads();  
    }
    
    std::cout << "All is done OK. Press some char key, <-| to exit\n";
    char key;
    std::cin >> key;
    return 0;
}