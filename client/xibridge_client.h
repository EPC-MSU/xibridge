#ifndef _XIBRIDGE_CLIENT_H
#define  _XIBRIDGE_CLIENT_H

#include "../Common/defs.h"

#include "../Common/Protocols.h"

#include "Bindy_helper.h"

#include "xibridge.h"

#include <vector>


#define MAXHOST 64  

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
#define ERR_RECV_TIMEOUT 6


/*
When no answerback data len known this time value is used to control answer arrival
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
	* Функция определения версии xibridge-компонента
	* @return версия xibridge-компонента
	* \endrussian
*/
	static int xibridge_major_version()
	{
		return 1;
	};

/**
	* \russian
	* Функция определения максимальной версии протокола xibridge-компонента
	* @return версия максимальной версии протокола xibridge-компонента (1,2 или 3)
	* \endrussian
*/
	static int xibridge_protocol_version()
	{
		return 3;
	}
	
/**
   * \russian
   * Функция установки версии протокола для взаимодействия с сервером по данному подключению
   * @param conn_id[in] идентификатор подключения
   * @param ver[in] номер версии протокола для взаимодействия с сервером (1,2 или 3)
   * \endrussian
*/
	static void xibridge_set_server_protocol(unsigned int conn_id, unsigned int proto);

/**
   * \russian
   * Функция запроса версии протокола для взаимодействия с сервером по данному подключению
   * @param conn_id[in] идентификатор подключения
   * @return номер версии протокола для взаимодействия с сервером (1,2 или 3), 0 - если версия не определена
   * \endrussian
*/
	static unsigned int xibridge_get_server_protocol(unsigned int conn_id);

/**
   * \russian
   * Функция закрытия данного подключения
   * @param[in] conn_id идентфикатор закрываемого подклбчения
   * \endrussian
*/
	static void xibridge_close_connection_device(unsigned int conn_id);

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
   * @param[in] addr ip-адрес сервера
   * @param[in] serial номер com-порта, слота или идентификатор устройств определенного типа, однозначно соответствующий имени устройства для открытия на сервере
   * @param[in] proto_ver номер версии протокола сервера  (1,2 или 3), к которому производится подключение
   * @param[in] send_timeout таймаут отправки запроса на подключение к серверу
   * @param[in] recv_timeout таймаут ответа сервера
   * \endrussian
*/
	Xibridge_client(const char *addr, 
		            unsigned int serial,  
		            unsigned int proto_ver, 
				    unsigned int send_tmout, 
					unsigned int recv_tmout);

/**
	* \russian
	* Функция установки версии протокола для взаимодействия с сервером по данному подключению
	* @param conn_id[in] идентификатор подключения
	* @param ver[in] номер версии протокола для взаимодействия с сервером (1,2 или 3)
	* \endrussian
*/
	void set_server_protocol_version(uint32 v) { _server_protocol_version = v; };

/**
	* \russian
	* Функция автоопределения максимальной версии протокола, поддерживаемого на сервере (urpc-, ximc-, xibridge-)
	* @param[in] addr ip-адрес сервера
	* @param[in] send_timeout таймаут отправки запроса на сервер
	* @param[in] recv_timeout таймаут получения ответа сервера
	* @return возвращает номер максимальной версии протокола (1, 2 или 3) в случае успешного операции, 0 - если определить версия не удалось
	* \endrussian
*/
	static uint32  xibridge_detect_protocol_version(const char *addr, uint32 send_timeout, uint32 resv_timeout);

/**
	* \russian
	* Функция инициализации xibridge-компонента, должна вызываться перед использованием любых других функций xibridge
	* @param[in] key_file_path имя существующего файла, содержащего ключевую информацию шифрования
	* @return 0, если инициализация завершилась неудачно
	* \endrussian
*/
	static bool xibridge_init(const char *key_file_path);
	
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
   * @param[out] буфер-приемник текста ошибки
   * @param[in] len длина буфера-приемника
   * @param[in] is_russian флаг - выдать текст на русском языке, должен быть не 0, если нужен русский 
   * @param[in] err_no код ошибки
   * \endrussian
*/
	static void xibridge_get_err_expl(char * s, int len, bool is_russian, unsigned int err_no);

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

	bvector  send_data_and_receive(bvector data, uint32 resp_length, uint32& res_err);
	
	uint32 get_server_protocol_version() const {return _server_protocol_version;}

	static uint32 get_self_protocol_version() { return xibridge_protocol_version(); }

	uint32 get_last_error() const { return _last_error; };

	void clr_errors() { _last_error = 0; }

	void get_error_expl(char * s, int len, bool is_russian = false) const;

	uint32 get_dev_num() const {
		return _dev_num;
	}

	uint32 get_resv_tmout() const { return _recv_tmout; }

	uint32 get_proto_version_of_the_recv_message() const { return AProtocol::get_version_of_cmd(_recv_message); }

private:
	/*
	 * Возвращает клиента, считаем, что обращение с отдельно взятым клиентом - в одном потоке
	*/
	static Xibridge_client * _get_client_as_free(unsigned int conn_id);

	bool _send_and_receive(bvector &req);
	void _set_last_error(uint32 err) { _last_error = err; }

	uint32 _dev_num;    // device number in xibridge server enumeration
    uint32 _server_protocol_version; // commincation protocol version
	uint32 _send_tmout;
	uint32 _recv_tmout;

	conn_id_t _conn_id; // id of bindy connection
	
	std::condition_variable _is_recv; //
	bool _is_really_recv;
	std::mutex _mutex_recv;
	bvector _recv_message; // for bindy-callback

	uint32 _last_error;  // last error 
	// uint32 _last_client_error; //  last client side error
	char _host[MAXHOST+1];
};

#endif