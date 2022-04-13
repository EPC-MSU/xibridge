#ifndef _XIBRIDGE_CLIENT_H
#define  _XIBRIDGE_CLIENT_H

#include "../Common/defs.h"

#include "../Common/Protocols.h"

#include "Bindy_helper.h"

#include "xibridge.h"

#include <vector>


#define MAXURI 256  

/*
* Defines client errors (errors to take place at the client side)
*/
#define ERR_NO_PROTOCOL 1
#define ERR_NO_CONNECTION 2
#define ERR_SEND_TIMEOUT  3
#define ERR_NO_BINDY    4
#define ERR_SEND_DATA 5
#define ERR_KEYFILE_NOT_REPLACED 6
/*
* Defines protocol and server errors
*/
#define ERR_RECV_TIMEOUT 7


/**
   * Defines default version number of the protocol
*/
#define DEFAULT_PROTO_VERSION 3


/**
   * When no answerback data len known this time value is used to control answer arrival
*/
//#define TIMEOUT_WAIT_STANDARD 100 

/*
When no answerback data len known this buffer value is used to control answer data size
*/
//#define MAX_ANSWER_LEN 512

/** 
    * \english
    * Class to communicate as a client with ximc, urpc- xibridge-server
    * \russian
    * Класс для взаимодействия в качестве клиента с ximc, urpc- xibridge-сервером
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
	static xibridge_version_t xibridge_get_version()
	{
        return {1, 0, 0};
	};

/**
	* \russian
	* Функция определения максимальной версии протокола библиотеки xibridge
	* @return версия максимальной версии протокола xibridge
	* \endrussian
*/
    static xibridge_version_t xibridge_get_max_protocol_version()
	{
        return{ DEFAULT_PROTO_VERSION, 0, 0 };
	}
	
/**
   * \russian
   * Функция установки версии протокола для взаимодействия с сервером по данному подключению
   * @param conn[in] данные подключения
   * @param ver[in] версия протокола для взаимодействия с сервером (1, 2, 3) 
   * @return код ошибки, если установка завершилась неудачно, 0 - если удачно
   * \endrussian
*/
    static uint32_t xibridge_set_connection_protocol_version(xibridge_conn_t conn, xibridge_version_t ver);

/**
   * \russian
   * Функция запроса версии протокола для взаимодействия с сервером по данному подключению
   * @param conn_id[in] идентификатор подключения
   * @return версия протокола для взаимодействия с сервером (1,2 или 3), если подключение не существует - возвращается версия протокола 3
   * \endrussian
*/
    static xibridge_version_t xibridge_get_connection_protocol_version(xibridge_conn_t conn);

/**
   * \russian
   * Функция закрытия данного подключения
   * @param[in] conn данные закрываемого подключения
   * @return код ошибки в случае ее возникновения при закрытии подключения, 0 - если успех  
   * \endrussian
*/
    static uint32_t xibridge_close_connection_device(xibridge_conn_t conn);

/**
   * \russian
   * Функция определения устройств, доступных для работы на сервере (ximc-) 
   * Функция распределяет и заполняет массив структур по количеству определенных устройств
   * @param[in] addr ip-адрес сервера
   * @param[in] addr ip-адрес адаптера(?)
   * @param[out] result указатель на указатель, по которому будет распределен массив структур с описанием устройств
   * @param[out] pcount указатель на переменную, куда будет помещено количество найденных устройств
   * @param[in] timeout таймаут ответа сервера
   * @param[out] last_errno указатель на переменную, куда будет помещен код ошибки в случае неудачной операции
   * @return 0 в случае неудачи
   * \endrussian
*/
	static bool xibridge_enumerate_adapter_devices(const char *addr, const char *adapter, 
		                                   unsigned char **result, 
										   unsigned int *pcount, unsigned int timeout,
										   unsigned int* last_errno = nullptr);

/**
   * \russian
   * Функция чтения данных ("как есть", без парсинга по протоколу) из устройства с помощью данного подключения
   * @param[in] conn_id идентификатор подключения
   * @param[in] buf буфер-приемник для получения данных 
   * @param[in] size длина буфера-приемника 
   * @return 0 - если операция завершилась неудачно
   * \endrussian
*/
	static int xibridge_read_connection_buffer(unsigned int conn_id, unsigned char *buf, int size);

/**
	* \russian
	* Функция записи данных ("как есть", без формирования по протоколу) в устройство с помощью данного подключения
	* @param[in] conn_id идентификатор подключения
	* @param[in] buf данные для отправки в устройство
	* @param[in] size длина данных
	* @return 0 - если операция завершилась неудачно
	* \endrussian
*/
	static int xibridge_write_connection(unsigned int conn_id, const unsigned char *buf, int size);

	/*

/**
   * \russian
   * Конструктор класса: создает  подключение по сети к устройству через сервер (urpc-xinet, ximc-xinet, xibridge)
   * @param[in] xi_net_uri строка с uri устройства, начинающаяся "xi-net://..."
   * @param[in] send_timeout таймаут отправки запроса на подключение к серверу
   * @param[in] recv_timeout таймаут ответа сервера
   * \endrussian
*/
    Xibridge_client(const char *xi_net_uri, unsigned int send_timeout, unsigned int recv_timeout);

/**
	* \russian
	* Функция автоопределения максимальной версии протокола, поддерживаемого на сервере (urpc-, ximc-, xibridge-)
	* @param[in] addr ip-адрес сервера
	* @param[in] send_timeout таймаут отправки запроса на сервер
	* @param[in] recv_timeout таймаут получения ответа сервера
	* @return возвращает номер максимальной версии протокола (1, 2 или 3) в случае успешного операции, 0 - если определить версия не удалось
	* \endrussian
*/
	static uint32_t  xibridge_detect_protocol_version(const char *addr, uint32_t send_timeout, uint32_t resv_timeout);

/**
	* \russian
	* Функция инициализации xibridge-системы, должна вызываться перед использованием любых других функций xibridge
	* @param[in] key_file_path имя существующего файла, содержащего ключевую информацию шифрования
	* @return код ошибки, если инициализация завершилась неудачно или key_file_path отличается от уже установленного, 0 если удачно
	* \endrussian
*/
	static uint32_t xibridge_init(const char *key_file_path);
	
/**
   * \russian
   * Функция выполнения операции запрос-ответ с учетом протокола, применяемого в данном подключении
   * @param[in] conn_id идентификатор подключения
   * @param[in] req данные запроса (код команды+параметры)
   * @param[in] req_len длина данных запроса
   * @param[out] resp буфер-приемник данных
   * @param[in] resp_len точная длина ожидаемых данных
   * @param[out] res_err указатель переменной для записи результата операции или ошибки в случае неудачи 
   * @return 0 - если операция завершилась неудачно
   * \endrussian
*/
	static bool xibridge_request_response(unsigned int conn_id, 
		                                  const unsigned char *req, 
										  int req_len, 
										  unsigned char *resp, 
										  int resp_len, unsigned int *res_err);

/**
   * \russian
   * Функция возвращает текст ошибки по ее коду
   * @param[in] err_no код ошибки
   * @return строка с ошибкой или нулевой указатель в случае неизвестного кода ошибки
   * \endrussian
*/
	static const char * xibridge_get_err_expl(unsigned int err_no);

	/*
	* This static member function  returns last err number of the connection conn_id
	*/
/**
   * \english
   * This static member function returns last error number of the  conn_id connection
   * @param[in] conn_id connection identifier
   * @return lst error code
   * \russian
   * Функция возвращает код последней ошибки, связанной с данным подключением
   * @param[in] conn_id идентификатор подключения
   * @return код последней ошибки
   * \endrussian
*/
	static unsigned int xibridge_get_last_err_no(unsigned int conn_id);
	
	bool open_connection_device();

	bool is_connected();

	void disconnect();
	
	bool close_connection_device();

	conn_id_t conn_id() const { return _conn_id; }

	bvector  send_data_and_receive(bvector data, uint32_t resp_length, uint32_t& res_err);
	
	uint32_t get_last_error() const { return _last_error; };

	void clr_errors() { _last_error = 0; }

	uint32_t get_dev_num() const {
		return _dev_num;
	}

	uint32_t get_resv_tmout() const { return _recv_tmout; }

	uint32_t get_proto_version_of_the_recv_message() const { return AProtocol::get_version_of_cmd(_recv_message); }
    xibridge_conn_t to_xibridge_conn_t() const { return{ (uint32_t)_conn_id, { _server_protocol_version, 0, 0 } } };
private:
/**
	* Возвращает клиента, считаем, что обращение с отдельно взятым клиентом - в одном потоке
*/
	static Xibridge_client * _get_client_as_free(conn_id_t conn_id);

	bool _send_and_receive(bvector &req);
	void _set_last_error(uint32_t err) { _last_error = err; }
/**
	* Ключевые атрибуты сущности
*/
    uint32_t _server_protocol_version; // default server_protocol_version number to put to the message
    conn_id_t _conn_id; //bindy id of this connection (bindy id + server protocol version)
    xibridge_device_t _dev_id;        // common device id 
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
	
/*
    *  Храним uri подключения для возможных логов  в процессе работы  
*/
	char _uri[MAXURI+1];
};

#endif