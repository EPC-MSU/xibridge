#ifndef _XIBRIDGE_H
#define  _XIBRIDGE_H

#include <stdint.h>

#if defined (WIN32) || defined(WIN64)
#if defined(BUILD_SHARED_LIBS_XI)
    #define XI_EXPORT __declspec(dllexport)
#else
#define XI_EXPORT 
#endif
#else
   #define XI_EXPORT
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

/**
    * \russian
    * Структура для работы с расширеным идентификаторм устройства (используется в протоколе обмена версии 3 и выше)
	* \endrussian
*/
PACK(
struct _xibridge_device_t
{
	uint32_t reserve;
	uint16_t VID;
	uint16_t PID;
	uint32_t id;
});
typedef _xibridge_device_t xibridge_device_t;

/**
    * \russian
    * Структура для хранения версии библиотеки, протокола
	* \endrussian
*/
PACK(
struct _xibridge_version_t
{
    uint8_t major;
    uint8_t minor;
    uint8_t bagfix;
});
typedef _xibridge_version_t xibridge_version_t;

/**
* \russian
* Структура для хранения данных подключения (id подключения +  версия протокола)
* \endrussian
*/
PACK(
struct _xibridge_conn_t
{
    uint32_t conn_id;  // уникальный идентифакатор сетевого подключения
    xibridge_version_t proto_ver; // версия протокола
});
typedef _xibridge_conn_t xibridge_conn_t;


/**
* \russian
* Константы длин чаcтей uri
* \endrussian
*/
#define XI_URI_HOST_LEN 64
#define XI_URI_SCHEMA_LEN 16

/**
*\russain
* Структура для хранения частей uri устройства
*\endrussain
*/
PACK(
struct _xibridge_parsed_uri{
    char uri_schema[XI_URI_SCHEMA_LEN];
    char uri_server_host[XI_URI_HOST_LEN];
    xibridge_device_t uri_device_id;
});
typedef _xibridge_parsed_uri xibridge_parsed_uri;

/** 
    * \russian
	* Константа определяет значение базового таймаута при операциях взаимодействия с сервером 
    * \endrussian
*/
#define TIMEOUT 3000

/** 
    * \russian
	* Функция определения версии xibridge-компонента 
    * @return версия библиотеки xibridge
    * \endrussian
*/
xibridge_version_t  XI_EXPORT xibridge_get_library_version();

/** 
   * \russian
   * Функция определения максимальной версии протокола xibridge-компонента
   * @return версия максимальной версии протокола xibridge-компонента (1,2 или 3)
   * \endrussian
*/
xibridge_version_t  XI_EXPORT xibridge_get_max_protocol_version();

/**
   * \russian
   * Функция инициализации xibridge-библиотеки, должна вызываться перед использованием любых других функций xibridge
   * @param[in] key_file_path имя существующего файла, содержащего ключевую информацию шифрования
   * @return код ошибки, если инициализация завершилась неудачно или key_file_path отличается от уже установленного,, 0 - если удачно
   * \endrussian
*/
uint32_t  XI_EXPORT xibridge_init(const char *key_file_path);

/**
   * \russian
   * Функция установки версии протокола для взаимодействия с сервером по данному подключению
   * @param conn_id[in] данные подключения
   * @param ver[in] устанавливаемая версия протокола для взаимодействия с сервером (1,2 или 3)
   * @return код ошибки, если установка версии завершилась неудачно,  0 если удачно
   * \endrussian
*/
uint32_t  XI_EXPORT xibridge_set_connection_protocol_version(xibridge_conn_t conn, xibridge_version_t ver);

/**
   * \russian
   * Функция запроса версии протокола для взаимодействия с сервером по данному подключению
   * @param conn_id[in] данные подключения
   * @return версия протокола для взаимодействия с сервером 
   * \endrussian
*/
xibridge_version_t  XI_EXPORT xibridge_get_connection_protocol_version(xibridge_conn_t conn);

/**
   * \russian
   * Функция открытия подключения по сети к устройству через сервер (xibridge) 
     * @param[in] recv_timeout таймаут ответа сервера
   * @param[out] pconn указатель переменной, куда будут  записаны данные подключения
   * @return код ошибки в случае неудачной оперции открытия, 0 - в случае успеха  
   * \endrussian
*/
uint32_t  XI_EXPORT xibridge_open_device_connection(const char *xi_net_uri, unsigned int recv_timeout, xibridge_conn_t *pconn);

/**
   * \russian
   * Функция закрытия подключения по сети к устройству через сервер (xibridge) 
   * @param[out] pconn указатель переменной, куда будут  записаны данные подключения
   * @return код ошибки в случае неудачной оперции открытия, 0 - в случае успеха  
   * \endrussian
*/
uint32_t  XI_EXPORT xibridge_close_device_connection(xibridge_conn_t conn);

/**
   * \russian
   * Функция выполнения операции запрос-ответ с учетом протокола, применяемого в данном подключении
   * @param[in] conn данные подключения
   * @param[in] req данные запроса (код команды+параметры)
   * @param[in] req_len длина данных запроса
   * @param[out] resp буфер-приемник данных
   * @param[in] resp_len точная длина ожидаемых данных
   * @return код ошибки в случае неудачной операции, 0 - если операция завершилась неудачно
   * \endrussian
*/
uint32_t  XI_EXPORT xibridge_device_request_response(xibridge_conn_t conn,
    const unsigned char *req,
    int req_len, unsigned char *resp,
    int resp_len);

/**
   * \russian
   * Функция возвращает текст ошибки по ее коду
   * @param[out] буфер-приемник текста ошибки
   * @param[in] err_no код ошибки
   * @return строка с ошибкой, NULL, если код ошибки неизвестен
   * \endrussian
*/
const char *  XI_EXPORT xibridge_get_err_expl(uint32_t err_no);

/**
   * \russian
   * Функция определения устройств, доступных для работы на сервере (ximc-) 
   * Функция распределяет и заполняет последовательность строк по количеству определенных устройств
   * @param[in] addr ip-адрес сервера
   * @param[in] addr ip-адрес адаптера(?)
   * @param[out] result указатель на указатель, по которому будут распределены и размещены строки(через 0) с uri-адресами устройств
   * @param[out] pcount указатель на переменную, куда будет помещено количество найденных устройств
   * @param[in] timeout таймаут ответа сервера
   * @return код ошибки в случае или 0
   * \endrussian
*/
uint32_t  XI_EXPORT xibridge_enumerate_adapter_devices(const char *addr, const char *adapter,
	char **result,
	unsigned int *pcount, unsigned int timeout);

/**
   * \russian
   * Функция возвращает код последней ошибки, связанной с данным подключением
   * @param[in] conn данные подключения
   * @return код последней ошибки
   * \endrussian
*/
uint32_t  XI_EXPORT xibridge_get_last_err_no(xibridge_conn_t conn);

#if defined(__cplusplus)
};
#endif
#endif