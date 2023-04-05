#include <cstring>
#include <cstdlib>
#include <cassert>
#include <ctype.h>
#include <zf_log.h>
#include "../client/xibridge_client.h" // ERROR CODES
#include "../inc/client/version.h"

#include "xibridge_server_lib.h"

#ifdef XIBRIDGE_SERVER_ENABLE_SUPERVISOR
#include "supervisor.hpp"
#endif

void print_help(char *argv[])
{
#if ZF_LOG_LEVEL <= ZF_LOG_DEBUG
    std::cout <<
        "Usage: " << argv[0] << " [keyfile] [debug] [bvvu|by_com_addr|by_serial|by_serialpidvid]"
        << std::endl
        << "Examples: " << std::endl
        << argv[0] << std::endl
        << argv[0] << " ximc" << std::endl
        << argv[0] << " debug bvvu" << std::endl
        << argv[0] << " ~/keyfile.sqlite by_serial" << std::endl
        << argv[0] << " ~/keyfile.sqlite debug by_com_addr" << std::endl
        << "Debug logging will be disabled by default, bvvu-style usb port matching configuration selected by default" << std::endl;
#else
    std::cout << "Usage: " << argv[0] << " keyfile [bvvu|by_com_addr|by_serial|by_serialpidvid]" 
        << std::endl
        << "Examples: " << std::endl
        << argv[0] << std::endl
        << argv[0] << " ~/keyfile.sqlite" << std::endl
        << argv[0] << " ~/keyfile.sqlite by_serial" << std::endl;
#endif

    std::cout << "Press a key to exit" << std::endl;

}

//the next function id not C standard, not supported in non win, the next is manual definition  
static char *strlwr_portable(char *str)
{
    unsigned char *p = (unsigned char *)str;

    while (*p) {
        *p = tolower((unsigned char)*p);
        p++;
    }

    return str;
}

// main function of the console app
int main(int argc, char *argv[])
{

    xibridge_version_t ver = XIBRIDGE_VERSION;
    std::cout << "=== Xibridge Server "
        << (int)ver.major << "."
        << (int)ver.minor << "."
        << (int)ver.bagfix << " "
        << "===" << std::endl;

    std::cout << "=== xi-net protocols v.2 and v.3 supported ===" << std::endl;

    bool exit = false;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            //if any param is something like "help" 
            char *s = argv[i];
            strlwr_portable(s);
            if (strcmp(s, "-help") == 0 || strcmp(s, "help") == 0
                || strcmp(s, "--help") == 0 || strcmp(s, "-h") == 0
                || strcmp(s, "--h") == 0)
                exit = true;
        }
    }
    if (exit)
    {
        print_help(argv);
        std::cin.get(); // To avoid console closing
        return 0;
    }

    const char *debug = nullptr;
    const char *mode = "bvvu";
    const char *keyfile = nullptr;
    const char *svisor = nullptr;
    int slimit = -1;

    if (argc > 1)
    {
        const char * s = argv[argc - 1];
        bool urpc = false;
        bool ximc = false;
        bool ximc_ext = false;
        if ((urpc = strcmp(s, "urpc") == 0) || (ximc = strcmp(s, "ximc") == 0) || (ximc_ext = strcmp(s, "ximc_ext") == 0) || // for compatibility
            strcmp(s, "by_com_addr") == 0 || strcmp(s, "by_serial") == 0 || strcmp(s, "by_serialpidvid") == 0                // new option vals
            || strcmp(s, "bvvu") == 0)
        {
            argc--;

            if (urpc) s = "by_com_addr";
            if (ximc) s = "by_serial";
            if (ximc_ext) s = "by_serialpidvid";
            mode = s;
        }
    }

    std::cout << "=== The " << mode << " device identification is selected  ===" << std::endl;

    bool is_keyfile_supplied = false;
    if (argc > 1)
    {
        strlwr_portable(argv[1]);
        if (!(is_keyfile_supplied = strcmp(argv[1], "debug") != 0) || (argc > 2 && strcmp(argv[2], "debug") == 0))
        {
            debug = "debug";
        }
    }

    int ret = server_main(keyfile, debug, mode, true, 0);

    switch (ret)
    {
    case sm_err_allstarted:

        std::cout << "Error! Another process (xxx_xinet_server) already running. Exitting...\nPress a key to exit!" << std::endl;
        std::cin.get(); //
        break;
    case sm_err_initfailed:
        std::cout << "Server initialization failed. Press a key to exit!" << std::endl;
        std::cin.get(); //
        break;
    }
    return ret;
}
