#ifndef _XIBRIDGE_SERVER_LIB_H
#define _XIBRIDGE_SERVER_LIB_H



#if defined (WIN32) || defined(WIN64)
#define XI_EXPORT __declspec(dllexport)
#else
#define XI_EXPORT
#endif

#define sm_err_allstarted 1
#define sm_err_initfailed 2

//#if defined(__cplusplus)
//extern "C" {
//#endif
    
    XI_EXPORT int server_main(
        const char *keyfile,
        const char *debug,
        const char * supervisor,
        int sp_limit,
        const char *dev2usb_mode,
        bool is_console_app);

    class DevId;
    XI_EXPORT std::vector<DevId> enumerate_devs_opened();

    XI_EXPORT std::vector<DevId> enumerate_devs();

    XI_EXPORT void start_server_thread_spv();

    XI_EXPORT void stop_server_thread();


//#if defined(__cplusplus)
//};
//#endif
#endif
