#ifndef _XIBRIDGE_CLIENT_H
#define  _XIBRIDGE_CLIENT_H
#include "../common/defs.h"
#include "xibridge.h"
#include "../common/protocols.h"
#include "bindy_helper.h"

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
* User's fault to pass a pointer to param
*/
#define ERR_NULLPTR_PARAM  9

#define ERR_SET_CONNECTION 10

/*
* Server errors
*/

#define ERR_DEVICE_LOST -5

/*
Packet-Fmt errors
*/

#define ERR_PCKT_FMT 12
#define ERR_PCKT_INV 13

/**
   * Defines default version number of the protocol
*/
#define DEFAULT_PROTO_VERSION 3

/** 
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

/**
	* \russian
	* Функция определения версии библиотеки xibridge
	* @return версия xibridge
	* \endrussian
*/
	static xibridge_version_t xi_get_version()
	{
        return {1, 0, 0};
	};

/**
	* \russian
	* Функция определения максимальной версии протокола библиотеки xibridge
	* @return версия максимальной версии протокола xibridge
	* \endrussian
*/
    static xibridge_version_t xi_get_max_protocol_version()
	{
        return{ DEFAULT_PROTO_VERSION, 0, 0 };
	}
	
    /**
    * \russian
    * Функция инициализации статических данных xibridge
    * @return код ошибки, если инициализация завершилась неудачно, 0 если удачно
    * \endrussian
    */
    static void xi_init();

/**
   * \russian
   * Отладочная функция установки версии протокола для взаимодействия с сервером
   * @param ver[in] версия протокола для взаимодействия с сервером (1, 2, 3) 
   * @return код ошибки, если установка завершилась неудачно, 0 - если удачно
   * \endrussian
*/
    static uint32_t xi_set_base_protocol_version(xibridge_version_t ver);

/**
   * \russian
   * Функция запроса версии протокола для взаимодействия с сервером по данному подключению
   * @param pconn указатель на данные подключения
   * @return версия протокола для взаимодействия с сервером (1,2 или 3), если подключение не существует - возвращается версия протокола 3
   * \endrussian
*/
    static xibridge_version_t xi_get_connection_protocol_version(const xibridge_conn_t *pconn);

/**
   * \russian
   * Функция закрытия данного подключения
   * @param[in] pconn указатель на данные закрываемого подключения
   * @return код ошибки в случае ее возникновения при закрытии подключения, 0 - если успех  
   * \endrussian
*/
    static uint32_t xi_close_connection_device(const xibridge_conn_t *pconn);
    	
/**
   * \russian
   * Функция чтения данных ("как есть", без парсинга по протоколу) из устройства с помощью данного подключения
   * @param[in] pconn указатель на данные подключения
   * @param[in] buf буфер-приемник для получения данных 
   * @param[in] size длина буфера-приемника 
   * @param[out] preal_read указатель на переменную, куда будет записана длина в байтах реально полученных данных 
   * @return код ошибки - если операция завершилась неудачно< 0 - если успех
   * \endrussian
*/
	static uint32_t xi_read_connection_buffer(
		                                         const xibridge_conn_t *pconn, 
											     uint8_t *buf, 
											     uint32_t size,
											     uint32_t* preal_read = nullptr
										     );

/**
	* \russian
	* Функция записи данных ("как есть", без формирования по протоколу) в устройство с помощью данного подключения
	* @param[in] pconn указатель на данные подключения
	* @param[in] buf данные для отправки в устройство
	* @param[in] size длина данных
	* @return код ошибки - если операция завершилась неудачно< 0 - если успех
	* \endrussian
*/
	static uint32_t xi_write_connection(
		                                   const xibridge_conn_t *pconn, 
									       const uint8_t*buf, 
									       uint32_t size
								       );
/**
   * \russian
   * Конструктор класса: создает  подключение по сети к устройству через сервер (urpc-xinet, ximc-xinet, xibridge)
   * @param[in] xi_net_uri строка с uri устройства, начинающаяся "xi-net://..." илт строка с адресом (даже пустым) - только для сетевого для подключения 
   * @param[in] send_timeout таймаут отправки запроса на подключение к серверу
   * @param[in] recv_timeout таймаут ответа сервера
   * \endrussian
*/
    Xibridge_client(const char *xi_net_uri, const char *adapter = NULL);

/**
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
	static uint32_t xi_request_response(
		                                   const xibridge_conn_t *pconn, 
		                                   const uint8_t *req, 
										   uint32_t req_len, 
										   uint8_t *resp, 
										   uint32_t resp_len
								       );

/**
   * \russian
   * Функция возвращает текст ошибки по ее коду
   * @param[in] err_no код ошибки
   * @return строка с ошибкой или нулевой указатель в случае неизвестного кода ошибки
   * \endrussian
*/
	static const char * xi_get_err_expl(unsigned int err_no);

	/**
	* \russian
	* Функция освобождения ресурсов, распределенных в результате вызова xibridge_enumerate_adapter_devices, должна
	* вызываться после xibridge_enumerate_adapter_devices
	* @param[in] presult указатель на распределенные данные, полученные в результате вызова
	* xibridge_enumerate_adapter_devices
	*/
	static void xi_free_enumerate_devices(char *presult)
	{
		if (presult != nullptr)
		{
			free (presult);
		}
	}
	
	bool open_device();

    bool exec_enumerate(
                           char **result,
                           uint32_t *pcount       
                       );

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

/**
   * \russian
   * Возвращает клиента, считаем, что обращение с отдельно взятым клиентом - в одном потоке
   * \endrussian
*/
	static Xibridge_client * _get_client_as_free(conn_id_t conn_id);
    static uint32_t _server_base_protocol_version;

   	bool _send_and_receive(bvector &req);
	void _set_last_error(uint32_t err, const char *add_text = nullptr) 
	{ 
		_last_error = err;
		const char *stand_text = xi_get_err_expl(err);
		if (stand_text == nullptr || add_text == nullptr) return;
		if (strstr(stand_text, "%s") != nullptr)
		{
			_complex_error = add_text;
		}
	}
/**
	* Ключевые атрибуты клиента
*/
    uint32_t _server_protocol_version; // server_protocol_version number to put to the message
    conn_id_t _conn_id;                // bindy id of this connection (bindy id + server protocol version)
    xibridge_device_t _dev_id;         // common device id 
	bool _is_proto_detected; 
/**
	* Таймауты чтения-записи
*/	
    uint32_t _send_tmout;
	uint32_t _recv_tmout;
 /**
	* Переменные для взаимодействия  с потоком данного клиента в bindy
*/  
	std::condition_variable _is_recv; 
	bool _is_really_recv;
	std::mutex _mutex_recv;
	bvector _recv_message; // for bindy-callback
/**
	* Есть необходимость хранить данное состояние ошибки, в частности из-за потока bindy
*/
	uint32_t _last_error;  // last error 
	std::string _complex_error; // last error if complex // not already used 
/*
    *  Храним uri подключения для возможных логов  в процессе работы  
*/
	char _host[XI_URI_HOST_LEN+1];
    char _adapter[XI_URI_HOST_LEN + 1];
};

#endif
