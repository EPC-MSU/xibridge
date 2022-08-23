#ifndef _XIBRIDGE_H
#define  _XIBRIDGE_H

#include <stdint.h>

//#include "../config_xi.h"

#define BUILD_SHARED_LIBS_XI

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
    * \english
    * Structure for xibridge device identifier 
    * \english
    * \russian
    * Структура для идентификатора устройства xibridge
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
typedef struct _xibridge_device_t xibridge_device_t;

/**
    * \english
    * Structure for version records 
    * \english
    * \russian
    * Структура для  версии
    * \endrussian
*/
PACK(
struct _xibridge_version_t
{
    uint8_t major;
    uint8_t minor;
    uint8_t bagfix;
});
typedef struct _xibridge_version_t xibridge_version_t;

#define xibridge_version_invalid  {0, 0, 0}

/**
    * \english
    * Structure for xibridge connection data (id + protocol version) 
    * \english
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
typedef struct _xibridge_conn_t xibridge_conn_t;

#define xibridge_conn_invalid { 0, { 0, 0, 0 } }

#define XI_URI_HOST_LEN 64
#define XI_URI_SCHEMA_LEN 16

/**
    * \english
    * Structure for parced device uri 
    * \english
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
typedef struct _xibridge_parsed_uri xibridge_parsed_uri;

/** 
    * \english
    * Base timeout constant defining time (in milliseconds) while server request results is being waited  
    * \english
    * \russian
    * Константа определяет (в милисекундах) значение базового таймаута при операциях взаимодействия с сервером 
    * \endrussian
*/
#define TIMEOUT 3000

/** 
    * \english
    * Function to get library version 
    * @return xibridge library version
    * \endenglish
    * \russian
    * Функция получения версии библиотеки 
    * @return версия библиотеки xibridge
    * \endrussian
*/
XI_EXPORT xibridge_version_t  
xibridge_get_library_version();

/** 
   * \english
   * Function to get last protocol version
   * @return last supported protocol version
   * \endenglish
   * Функция определения последней версии протокола, поддерживаемого библиотекой
   * @return последняя поддерживаемая версия протокола
   * \endrussian
*/
XI_EXPORT xibridge_version_t 
xibridge_get_last_protocol_version();

/**
   * \english
   * Debug function to set protocol version to dial with the server
   * @return error code - faulted, 0 - success
   * \endenglish
   * \russian
   * Отладочная функция установки версии протокола для взаимодействия с сервером
   * @param ver[in] версия протокола для взаимодействия с сервером (1, 2, 3) 
   * @return код ошибки, если установка завершилась неудачно, 0 - если удачно
   * \endrussian
*/
XI_EXPORT uint32_t 
xibridge_set_base_protocol_version(xibridge_version_t ver);

/**
   * \english
   * Function to get the current protocol version to deal with the server via the specified connection
   * @param[in] pconn connection identifier pointer
   * @return current protocol version to deal with the server
   * \endenglish
   * \russian
   * Функция запроса версии протокола для взаимодействия с сервером по данному подключению
   * @param[in] pconn указатель на указатель на идентификатор подключения
   * @return версия протокола для взаимодействия с сервером 
   * \endrussian
*/
XI_EXPORT xibridge_version_t  
xibridge_get_connection_protocol_version(const xibridge_conn_t *pconn);

/**
 * \english
   * Function opens a connection to a device via the server
   * @param[in] xi_net_uri char string containing uri-address of the device to connect to, for example:"xi-net://192.168.0.16/9" или
   * "xi-net://server.com/15a97f550017"
   * @param[out] pconn connection data pointer to place the connection data if success 
   * @return error code if faulted, 0 - success
   * \endenglish
   * \russian
   * Функция открытия подключения по сети к устройству через сервер (xibridge, ximc, urpc) 
   * @param[in] xi_net_uri строка с uri-адресом открываемого устройства, например: "xi-net://192.168.0.16/9" или
   * "xi-net://server.com/15a97f550017"
   * @param[out] pconn указатель переменной, куда будут записаны данные подключения
   * @return код ошибки в случае неудачной оперции открытия, 0 - в случае успеха  
   * \endrussian
*/
XI_EXPORT uint32_t 
xibridge_open_device_connection(const char *xi_net_uri, 
                                xibridge_conn_t *pconn);

/**
   * \english
   * Function closes a device connection
   * @param[out] pconn pointer to the connection data
   * @return error code if fault, 0 if success  
   * \endenglish
   * \russian
   * Функция закрытия подключения к устройству 
   * @param[out] pconn указатель на данные подключения
   * @return код ошибки в случае неудачной оперции закрытия, 0 - в случае успеха  
   * \endrussian
*/
XI_EXPORT uint32_t  
xibridge_close_device_connection(const xibridge_conn_t *pconn);

/**
   * \english
   * Function executes request-response operation, accounts a protocol kind applied at this device connection
   * @param[in] pconn pointer to the connection data 
   * @param[in] req request data
   * @param[in] req_len request data length
   * @param[out] resp buffer to accept response data
   * @param[in] resp_len the length of data to be recorded
   * @return error code if fault, 0 if success
   * \endenglish
   * \russian
   * Функция выполнения операции запрос-ответ с учетом протокола, применяемого в данном подключении
   * @param[in] pconn указатель на данные подключения
   * @param[in] req данные запроса 
   * @param[in] req_len длина данных запроса
   * @param[out] resp буфер-приемник данных
   * @param[in] resp_len длина записанных данных ответа
   * @return код ошибки в случае неудачной операции, 0 - если операция завершилась неудачно
   * \endrussian
*/
XI_EXPORT uint32_t 
xibridge_device_request_response(const xibridge_conn_t *pconn,
                                 const uint8_t *req,
                                 uint32_t req_len, 
                                 uint8_t *resp,
                                 uint32_t resp_len);

/**
   * \english
   * Function retunts error explanation text 
   * @param[in] err_no error code
   * @return char string containing error explanation text or nullptr in case of unknown error code
   * \endenglish
   * \russian
   * Функция возвращает текст ошибки по ее коду
   * @param[in] err_no код ошибки
   * @return строка с ошибкой или NULL, если код ошибки неизвестен
   * \endrussian
*/
XI_EXPORT const char * 
xibridge_get_err_expl(uint32_t err_no);

/**
   * \english
   * Function discovers devices which are availbale to operate with via the server. It allocates and fill the char 
   * array sequence containing available ddevice uries
   * @param[in] addr ip-address of a server (or its domain name), for example: "192.168.0.16" or "server.com"
   * @param[in] addr ip-address of the net adapter, nullptr may be passed if used no adapter 
   * @param[out] ppresult pointer to memory address which will point to the allocated char array sequence with device 
   * uries, separated by zeroes, example: "xi-net://192.168.0.16/9<0>xi-net://192.168.0.16/15a97f550017<0><0>"
   * @return error code if fault, 0 if success
   * \endenglish
   * \russian
   * Функция определения списка устройств, доступных для работы на сервере (ximc-) 
   * Функция распределяет и заполняет последовательность строк по количеству определенных устройств
   * @param[in] addr ip-адрес сервера (либо доменное имя), например: "192.168.0.16" или "server.com"
   * @param[in] addr ip-адрес сетевого адаптера, можно передать nullptr, если нет адаптера  
   * @param[out] ppresult указатель на указатель, по которому будут распределены и размещены строки, разделенные 
   * нулями, с uri-адресами устройств, пример:
   * "xi-net://192.168.0.16/9<0>xi-net://192.168.0.16/15a97f550017<0><0>"
   * @param[out] pcount указатель на переменную, куда будет помещено количество найденных устройств
   * @return код ошибки в случае неудачного определения списка устройства или 0 в случае успеха
   * \endrussian
*/
XI_EXPORT uint32_t  
xibridge_enumerate_adapter_devices(const char *addr, 
                                   const char *adapter,
                                   char **ppresult,
                                   uint32_t *pcount);

/**
   * \english
   * Function releases memory allocated by xibridge_enumerate_adapter_devices function call,
   * must be called after ibridge_enumerate_adapter_devices calling
   * \endenglish
   * \russian
   * Функция освобождения ресурсов, распределенных в результате вызова xibridge_enumerate_adapter_devices, должна 
   * вызываться после xibridge_enumerate_adapter_devices
   * @param[in] presult указатель на распределенные данные, полученные в результате вызова* xibridge_enumerate_adapter_devices 
   * \endrussian
*/
XI_EXPORT void 
xibridge_free_enumerate_devices(char *presult);

#if defined(__cplusplus)
};
#endif
#endif
