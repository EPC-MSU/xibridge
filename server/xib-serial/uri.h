#ifndef URPC_URI_H
#define URPC_URI_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#ifdef _MSC_VER
    // #define _CRT_SECURE_NO_WARNINGS
    #define portable_strcasecmp _stricmp
#else
    #define portable_strcasecmp strcasecmp
#endif

struct urpc_uri_t
{
    char scheme[1024];
    char host[1024];
    char path[1024];
    char paramname[1024];
    char paramvalue[1024];
    int port;
};

int
urpc_uri_parse(
    const char *uri,
    struct urpc_uri_t *result
);

#endif //URPC_URI_H
