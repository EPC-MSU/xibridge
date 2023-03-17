#include <iostream>
#include "../common/defs.h"
#include "../xibridge.h"

extern bool xinet_ximc_usage_example(const char *ip, uint32_t dev_num);
extern bool xinet_urpc_usage_example(const char *ip, uint32_t dev_num);
extern void xinet_ximc_threads();
extern void xinet_urpc_threads();
extern bool xinet_xibridge_usage_example_urpc(const char *ip, uint32_t dev_num);
extern void xinet_xibridge_threads();
extern bool xinet_xibridge_usage_example_ximc(const char *ip, uint32_t dev_num);
extern bool xinet_xibridge_usage_example_ximc_ext(const char *ip, uint32_t dev_num);
extern bool xinet_xibridge_usage_example_urpc(const char *ip, uint32_t dev_num);
extern void xinet_xibridge_threads_urpc();

int main(int /*argc*/, char ** /*argv[]*/)
{
    bool ret = true;
    std::cout << "Starting..." << std::endl;

    std::cout << "Enter ip-address for ximc xinet-server if you intend to test xibridge with it (or type 'N' if not):\n";

    std::string ip_s;
    std::string controller_type;
    uint32_t dev_num;
    dev_num = 0;
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
        //xinet_ximc_threads();
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
        // xinet_urpc_threads();  
    }

    std::cout << "Enter ip-address for xibridge xinet-server if you intend to test xibridge with it (or type 'N' if not):\n";
    std::cin >> ip_s;
    if (!ip_s.empty() && ip_s[0] != 'N' && ip_s[0] != 'n')
    {
        std::cout << "Enter device identification type matching xibridge-server option selected (bvvu, by_serial, by_com_addr or by_serialpidvid is supported) is to be used with xibridge-server:\n";
        std::cin >> controller_type;
        if (controller_type != "ximc_ext" && controller_type != "by_serialpidvid")
        {
            std::cout << "Enter device identifier to be used with xibridge-server according to the selected device identification type (decimal unsigned number):\n";
            std::cin >> dev_num;
        }

        if (controller_type == "ximc" || controller_type == "by_serial")
        {
            std::cout << "XIMC-controller commnads will be used in the example!\n";
            ret = xinet_xibridge_usage_example_ximc(ip_s.data(), dev_num);
        }

        else if (controller_type == "urpc" || controller_type == "by_com_addr" || controller_type == "bvvu")
        {
            std::cout << "URMC-controller commands (with CRC16 added) will be used in the example!\n";
            ret = xinet_xibridge_usage_example_urpc(ip_s.data(), dev_num);
            //if (ret) xinet_xibridge_threads_urpc();
        }
        else if (controller_type == "ximc_ext" || controller_type == "by_serialpidvid")
        {
            std::cout << "XIMC-controller commnads will be used in the example!\n";
            ret = xinet_xibridge_usage_example_ximc_ext(ip_s.data(), dev_num);
            //if (ret) xinet_xibridge_threads_urpc();
        }
        else
        {
            std::cout << "Controller type is unknown!:\n";
            ret = false;
        }

        if (!ret) return 1;
       
    }
    
    std::cout << "All is done OK. Press some char key, <-| to exit\n";
    char key;
    std::cin >> key;
    return 0;
}