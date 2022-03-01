#ifndef _XIBRIDGE_H
#define  _XIBRIDGE_H

/* *
* Функция запроса версии xibridge
*/
extern int xibridge_major_version();

/* *
* Функция запроса версии протокола, поддерживаемой данным xibridge
*/
extern int xibridge_protocol_version();
/*
* Функция инициализации системы xibridge 
*/
extern bool xibridge_init(const char *key_file_path);

/*
* Функция завершения работы с системой xibridge
*/
extern void xibridge_shutdown();

/*
* Функция установки версии протокола сервера xibridge для данного подключения
*
*/
extern void xibridge_set_server_protocol_verion(unsigned int conn_id, unsigned int ver);

/*
* Функция запроса версии протокола сервера данного подключения
*/
extern unsigned int xibridge_get_server_protocol_version(unsigned int conn_id);

/*
  Функция создания подключения к серверу (xi-net, xibridge)
*/
extern unsigned int xibridge_open_device_connection(const char *addr,
	unsigned int serial, unsigned int proto);

/*
  Функция закрытия данного подключения
*/
extern void xibridge_close_device_connection(unsigned int conn_id);

/*
Функция создания подключения к серверу (xi-net, xibridge)
*/
extern unsigned int xibridge_detect_protocol_version(const char *addr);

#endif