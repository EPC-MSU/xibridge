/**
    *\russian
    * преобразует строку с uri xibridge в структуру из составляющих его частей
    * uri типа "xi-net://server.org/09" или "xi-net://192.168.06.08/00000000BBBBAAAA00000009" (идентификатор устройства - до 24 цифр в Hex-кодировке)  
	* @param[in] uri строка с uri, которая  должна заканчиваться 0, в ней не должно быть промежуточных и завершающих пробелов
	* @param[out] parsed-uri указатель на структуру для разобранного uri
	* @return 0 - успех, 1 - ошибка 
	*\endrussian
*/
uint32_t xibridge_parse_uri_dev12(const char *uri, xibridge_parsed_uri *parsed_uri)
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
        if (stricmp(parsed_uri->uri_schema, XI_NET_SCHEMA) == 0)
		{
			p = end_s + 3; // length of separators;
			end_s = strchr(p, '/');
			l = 0;
			if (end_s != 0 && (l = ((int)(end_s - p))) < XI_URI_HOST_LEN)
			{
			   memcpy(parsed_uri -> uri_server_host, p, l);
			   p = end_s + 1;  // length of separator;
               end_s = strchr(p, 0);
               if (end_s == 0) return 1;
               char sdev[24 + 1];  // hex-symbols;
			   if (sprintf(sdev, "%024s", p) > 0)
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
	return 1;
}
