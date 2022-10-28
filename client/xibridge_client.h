#ifndef _XIBRIDGE_CLIENT_H
#define  _XIBRIDGE_CLIENT_H
#include "../common/defs.h"
#include "../xibridge.h"
#include "../common/protocols.h"
#include "bindy_helper.h"
#include "../inc/client/version.h"

/*
* Defines client errors (errors to take place at the client side)
*/
#define ERR_NO_PROTOCOL 1
#define ERR_NO_CONNECTION 2
#define ERR_SEND_TIMEOUT  3
#define ERR_NO_BINDY    4
#define ERR_SEND_DATA 5

/*
* Defines protocol and server errors
*/
#define ERR_RECV_TIMEOUT 7
#define ERR_ANOTHER_PROTOCOL 8

/*
    *\english
    * Errors of using  
    * \endenglish
    *\russian
    * Ошибки использования
    * \endrussian
*/
#define ERR_NULLPTR_PARAM  9
#define ERR_SET_CONNECTION 10

/*
    *\english
    * Error(s) from server 
    * \endenglish
    *\russian
    * Ошибки из сервера
    * \endrussian
*/
#define ERR_DEVICE_OPEN 0xffbc
#define ERR_DEVICE_LOST 0xfffb

#define ERR_PCKT_FMT 12
#define ERR_PCKT_INV 13

/*
    *\english
    * Defines default version number of the protocol
    * \endenglish
    *\russian
    * Определяет номер версии протокола по умолчанию  
    * \endrussian
*/
#define DEFAULT_PROTO_VERSION 3

/* 
    * \english
    * Class to communicate as a client with ximc, urpc-, xibridge-server
    * \endenglish
    * \russian
    * Класс для взаимодействия в качестве клиента с ximc, urpc-, xibridge-сервером
    * \endrussian
*/
class Xibridge_client
{
    friend class Bindy_helper;
public:
/*
    * \english
    * Function to get library version 
    * @return xibridge library version
    * \endenglish
    * \russian
    * Функция получения версии библиотеки 
    * @return версия библиотеки xibridge
    * \endrussian
*/
    static xibridge_version_t xbc_get_version()
    {
        return XIBRIDGE_VERSION;
    };

/*
    * \english
    * Function to get last protocol version
    * @return last supported protocol version
    * \endenglish
    * Функция определения последней версии протокола, поддерживаемого библиотекой
    * @return последняя поддерживаемая версия протокола
    * \endrussian
*/
    static xibridge_version_t xbc_get_last_protocol_version()
    {
        return{ DEFAULT_PROTO_VERSION, 0, 0 };
    }
     
/*
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
    static uint32_t xbc_set_base_protocol_version(xibridge_version_t ver);

/*
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
    static xibridge_version_t xbc_get_connection_protocol_version(const xibridge_conn_t *pconn);

/*
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
    static uint32_t xbc_close_connection_device(const xibridge_conn_t *pconn);
        
/*
    * \russian
    * Функция чтения данных ("как есть", без парсинга по протоколу) из устройства с помощью данного подключения
    * @param[in] pconn указатель на данные подключения
    * @param[in] buf буфер-приемник для получения данных 
    * @param[in] size длина буфера-приемника 
    * @param[out] preal_read указатель на переменную, куда будет записана длина в байтах реально полученных данных 
    * @return код ошибки - если операция завершилась неудачно< 0 - если успех
    * \endrussian
*/
    static uint32_t xbc_read_connection_buffer(
        const xibridge_conn_t *pconn, 
        uint8_t *buf, 
        uint32_t size,
        uint32_t* preal_read = nullptr
    );

/*
    * \russian
    * Функция записи данных ("как есть", без формирования по протоколу) в устройство с помощью данного подключения
    * @param[in] pconn указатель на данные подключения
    * @param[in] buf данные для отправки в устройство
    * @param[in] size длина данных
    * @return код ошибки - если операция завершилась неудачно< 0 - если успех
    * \endrussian
*/
    static uint32_t xbc_write_connection(
        const xibridge_conn_t *pconn, 
        const uint8_t*buf, 
        uint32_t size
    );
/*
    * \russian
    * Конструктор класса: создает  подключение по сети к устройству через сервер (urpc-xinet, ximc-xinet, xibridge)
    * @param[in] xi_net_uri строка с uri устройства, начинающаяся "xi-net://..." или строка с адресом (даже пустым) - только для сетевого для подключения 
    * @param[in] send_timeout таймаут отправки запроса на подключение к серверу
    * @param[in] recv_timeout таймаут ответа сервера
    * \endrussian
*/
    Xibridge_client(const char *xi_net_uri, const char *adapter = NULL);

/*
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
    static uint32_t xbc_request_response(
        const xibridge_conn_t *pconn, 
        const uint8_t *req, 
        uint32_t req_len, 
        uint8_t *resp, 
        uint32_t resp_len
    );

/*
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
    static const char *xbc_get_err_expl(unsigned int err_no);

/*
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
    static void xbc_free_enumerate_devices(char *presult)
    {
        if (presult != nullptr)
        {
            free (presult);
        }
    }
    
    bool open_device();

    bool exec_enumerate( char **result, uint32_t *pcount);

    bool open_connection();

    bool is_connected();

    void disconnect();
    
    bool close_connection_device();

    bool decrement_server_protocol_version();

    conn_id_t conn_id() const { return _conn_id; }

    bvector  send_data_and_receive(bvector data, uint32_t resp_length);
    
    uint32_t get_last_error() const { return _last_error; };

    void clr_errors() 
    { 
        _last_error = 0; 
        _complex_error  = ""; 
    }

    uint32_t get_resv_tmout() const { return _recv_tmout; }

    uint32_t get_proto_version_of_the_recv_message() const 
    { 
        return AProtocol::get_version_of_cmd(_recv_message); 
    }
    
    xibridge_conn_t to_xibridge_conn_t() const 
    {
        return{ (uint32_t)_conn_id, { (uint8_t)_server_protocol_version, 0, 0 } };
    }
private:

/*
    * \english
    * Returns pointer to a client by connection id
    * \endenglish
    * \russian
    * Возвращает клиента, считаем, что обращение с отдельно взятым клиентом - в одном потоке
    * \endrussian
*/
    static Xibridge_client * _get_client_as_free(conn_id_t conn_id);
    static uint32_t _server_base_protocol_version;

    bool _send_and_receive(bvector &req);
    void _set_last_error(uint32_t err, const char *add_text = nullptr);
    
/*
    * \english
    * Base client attributes
    * \endenglish
    * \russian
    * Ключевые атрибуты клиента
    * \endrussian
*/
    uint32_t _server_protocol_version; // номер версии протокола сервера 
    conn_id_t _conn_id;                // уникальный номер подключения (bindy)
    xibridge_device_t _dev_id;         // идентификатор устройства
    bool _is_proto_detected; 

/*
    * \english
    * Read and write timeout values
    * \endenglish
    * \russian
    * Таймауты чтения-записи
    * \endrussian
*/  
    uint32_t _send_tmout;
    uint32_t _recv_tmout;

 /*
    * \english
    * Variables to deal with the client thread in the bindy 
    * \endenglish
    * \russian
    * Переменные для взаимодействия с потоком данного клиента в bindy
    * \endrussian
*/  
    std::condition_variable _is_recv; 
    bool _is_really_recv;
    std::mutex _mutex_recv;
    bvector _recv_message; 

/*
    * \english
    * This to keep an error state partially becouse of a thread in bindy
    *\ endenglish
    * \russian
    * Есть необходимость хранить данное состояние ошибки, в частности из-за потока bindy
    * \endrussian
*/
    uint32_t _last_error;       // last error 
    std::string _complex_error; // last error if complex // not already used 

/*
    * \english
    * Keep connection addresses to have an opportunity to use them in some logging 
    * \endenglish
    * \russian
    *  Храним uri подключения для возможных логов  в процессе работы
    * \endrussian
*/
    char _host[XI_URI_HOST_LEN+1];
    char _adapter[XI_URI_HOST_LEN + 1];
    
};

#endif
