#ifndef _XIBRIDGE_SERVER_LIB_H
#define _XIBRIDGE_SERVER_LIB_H



#if defined (WIN32) || defined(WIN64)
#define XI_EXPORT __declspec(dllexport)
#else
#define XI_EXPORT
#endif

#define sm_err_allstarted 1
#define sm_err_initfailed 2
    
    XI_EXPORT int server_main(
        const char *keyfile,
        const char *debug,
        const char * supervisor,
        int sp_limit,
        const char *dev2usb_mode,
        bool is_console_app,
        void(*cb_devsrescanned_val)());

    XI_EXPORT std::vector<std::string> enumerate_devs_opened();

    XI_EXPORT std::vector<std::string> enumerate_devs();

    XI_EXPORT int start_server_thread_spv(void(*cb_devsrescanned_val)());

    XI_EXPORT void stop_server_thread();

#endif
