#ifndef _XIBRIDGE_H
#define  _XIBRIDGE_H

#include "../common/ext_dev_id.h" 

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
* если резултат 0 - неудача
*/
extern int xibridge_init(const char *key_file_path);

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
  * Функция создания подключения к серверу (xi-net, xibridge)
  *  Реально работает на любом сервере и proto == 1 || proto == 2
  * To do timeout ? ? ?
*/
extern unsigned int xibridge_open_device_connection(const char *addr,
	unsigned int serial, unsigned int proto, unsigned int* last_errno = nullptr);

/*
* Функция создания подключения к серверу  xibridge с указанием расширенного идентификатора устройства
* To do timeout ? ? ?
*  Реально работает c сервером xibridge и proto >=3
*/
extern unsigned int xibridge_open_device_connection(const char *addr,
 const ExtDevId *pe_id, unsigned int proto, unsigned int* last_errno = nullptr);
/*

/*
 * Функция закрытия данного подключения
*/
extern void xibridge_close_device_connection(unsigned int conn_id);

/*
Функция создания подключения к серверу (xi-net, xibridge)
*/
extern unsigned int xibridge_detect_protocol_version(const char *addr);

/*
* Функция выполнения запроса
* To do timeout ? ? ?
* результат 0 - неудача
*/
extern int xibridge_device_request_response(unsigned int conn_id, 
	                                         const unsigned char *req, 
											 int req_len, unsigned char *resp, 
                                             int resp_len, unsigned int *res_err
											);

/*
* Функция формирования текста ошибки по ее номеру
*/
extern void xibridge_get_err_expl(char * s, int len, int is_russian, unsigned int err_no);

/*
* Функция получения списка номеров доступных устройств 
* Реально работает c сервером ximc и сервером xibridge
*/
extern void xibridge_enumerate_devices(const char *addr,
    unsigned int proto,
    unsigned int *result,
    unsigned int *pcount,
    unsigned int* last_errno = nullptr);


/*
* Функция получения списка раширенных идентификаторов  доступных устройств
* Реально работает с сервером xibridge
*/
extern void xibridge_enumerate_devices_ext(const char *addr,
	unsigned int proto,
	ExtDevId *result,
	unsigned int *pcount,
	unsigned int* last_errno = nullptr);

/*
* Функция получения номера последней ощибки у данного подключения
*/
extern unsigned int xibridge_get_last_err_no(unsigned int conn_id);
#endif