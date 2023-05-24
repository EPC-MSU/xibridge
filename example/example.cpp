#include <iostream>
#include "../common/defs.h"
#include "../xibridge.h"
#include <zf_log.h>

ZF_LOG_DEFINE_GLOBAL_OUTPUT_LEVEL;

int main(int argc, char * argv[])
{
    
    zf_log_set_output_level(ZF_LOG_NONE);
    char *ip = argv[1];
    uint32_t count;
    sscanf(argv[2], "%d", &count);
    
    char *pdata;
    
    uint32_t err = xibridge_enumerate_adapter_devices(ip, "", &pdata, & (uint32_t)count);

    if (err)
    {
        return 1;
    }
    
    xibridge_free_enumerate_devices(pdata);
    return 0;
}