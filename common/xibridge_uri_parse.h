/**
    *\russian
    * Разбирает uri устройства xibridge  
    * примеры uriЖ "xi-net://server.org/09" или "xi-net://192.168.06.08/00000000BBBBAAAA00000009"  
    * @param[in] uri устройства в виде строки
    * @param[out] parsed-uri структура для разобранного uri
    * @return 0 - успех, 1 - не удалось разобрать
    *\endrussian
*/
#include "defs.h"
uint32_t xibridge_parse_uri_dev12(const char *uri, xibridge_parsed_uri *parsed_uri)
{
#define XI_NET_SCHEMA "xi-net" 
    if (parsed_uri == NULL) return 1;
    memset(parsed_uri->uri_schema, 0, XI_URI_SCHEMA_LEN);
    memset(parsed_uri->uri_server_host, 0, XI_URI_HOST_LEN);
    parsed_uri->uri_device_id.reserve = 0;
    parsed_uri->uri_device_id.id = 0;
    parsed_uri->uri_device_id.PID = 0;
    parsed_uri->uri_device_id.VID = 0;
    const char *p = uri;
    p += strspn(p, " \t");
    const char *end_s = strstr(p, "://");
    int l = 0;
    size_t len;
    if (end_s != 0 && (l = ((int)(end_s - p))) < XI_URI_SCHEMA_LEN)
    {
        memcpy(parsed_uri->uri_schema, p, l);
        if (strcmp(parsed_uri->uri_schema, XI_NET_SCHEMA) == 0)
        {
            p = end_s + 3; // length of separators;
            end_s = strchr(p, '/');
            l = 0;
            if (end_s != 0 && (l = ((int)(end_s - p))) < XI_URI_HOST_LEN)
            {
                memcpy(parsed_uri->uri_server_host, p, l);
                p = end_s + 1;  // length of separator;
                end_s = strchr(p, 0);
                if (end_s == 0) return 1;
                char sdev[24 + 1];  // hex-symbols;
                memset(sdev, '0', 24);
                if ((len = strlen(p)) <= 24) // aligning shorter record left to find out vid, pid, id
                {
                    strcpy(sdev + (24 - (int)len), p);
                    {
                        unsigned int vid, pid, id, reserved;
                        if (sscanf(sdev, "%8x%4x%4x%8x", &reserved, &vid, &pid, &id)
                            == 4)
                        {
                            parsed_uri->uri_device_id.reserve = (uint16_t)reserved;
                            parsed_uri->uri_device_id.VID = (uint16_t)vid;
                            parsed_uri->uri_device_id.PID = (uint16_t)pid;
                            parsed_uri->uri_device_id.id = (uint32_t)id;
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 1;
}
