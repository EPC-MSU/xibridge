/*
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "../client/xibridge.h"
*/

#define XI_URI_SCHEMA_LEN 16

/**
    *\russain
    * Структура для хранения частей uri устройства
	*\endrussain
*/
PACK(struct _xibridge_parsed_uri{
	char uri_schema[XI_URI_SCHEMA_LEN];
	char uri_server_host[XI_URI_HOST_LEN];
	xibridge_device_t uri_device_id;
}); 
typedef _xibridge_parsed_uri xibridge_parsed_uri;

/**
    *\russian
    * преобразует строку с uri в структуру из составляющих его частей
	* @param[in] uri строка с uri
	* @param[out] parsed-uri указатель на структуру для разобранного uri
	* @return 0 - успех, 1 - ошибка 
	*\endrussian
*/
uint32_t xibridge_parse_uri(const char *uri, xibridge_parsed_uri *parsed_uri)
{
#define XI_NET_SCHEMA "xi-net" 
	if (parsed_uri == NULL) return 1;
	memset(parsed_uri -> uri_schema, 0, XI_URI_SCHEMA_LEN);
	memset(parsed_uri -> uri_server_host, 0, XI_URI_HOST_LEN) ;
	parsed_uri->uri_device_id.reserve = 0;
	parsed_uri->uri_device_id.id = 0;
	parsed_uri->uri_device_id.PID = 0;
	parsed_uri->uri_device_id.VID = 0;

	const char *p = uri;
	while (*p == ' ' || *p == '\t') p++;
	const char *end_s = strstr(p, "://");

	int l = 0;
	if (end_s != 0 && (l = ((int)(end_s - p))) < XI_URI_SCHEMA_LEN)
	{
		memcpy(parsed_uri -> uri_schema, p, l);
		if (stricmp(p, XI_NET_SCHEMA) == 0)
		{
			p = end_s + 3; // length of separators;
			end_s = strchr(p, '/');
			l = 0;
			if (end_s != 0 || (l = ((int)(end_s - p))) < XI_URI_HOST_LEN)
			{
			   memcpy(parsed_uri -> uri_server_host, p, l);
			   p = end_s + 1;  // length of separator;
			   char sdev[24];  // hex-symbols;
			   //char format_str[16]; sprintf(format_str, "%%0%ds", sizeof(xibridge_device_t)* 2);
			   if (sscanf(p, "%0+24s", sdev) != 0)
			   {
				   unsigned int vid, pid, id, reserved;
				   if (sscanf(p, "%8x%4x%4x%8x", parsed_uri->uri_device_id.reserve, parsed_uri->uri_device_id.VID, parsed_uri->uri_device_id.PID, parsed_uri->uri_device_id.id)
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
	return 1;
}