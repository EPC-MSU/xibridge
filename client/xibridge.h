#ifndef _XIBRIDGE_H
#define  _XIBRIDGE_H


#if defined (WIN32) || defined(WIN64)
#if defined(BUILD_SHARED_LIBS_XI)
    #define XI_EXPORT __declspec(dllexport)
#else
#define XI_EXPORT 
#endif
#else
   #define XI_EXPORT
#endif

/**
    * \russian
    * Структура для работы с расширеным идентификаторм устройства (используется в протоколе обмена версии 3 и выше)
*/
typedef struct
{
	uint32_t reserve;
	uint16_t VID;
	uint16_t PID;
	uint32_t id;
} xibridge_device_t;

/**
    * \russian
    * Структура для хранения версии библиотеки, протокола
*/
typedef struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t bagfix;
} xibridge_version_t;

/**
* \russian
* Структура для хранения данных подключения (id подключения +  версия протокола)
*/
typedef struct
{
    uint32_t conn_id;  // уникальный идентифакатор сетевого подключения
    xibridge_version_t proto_ver; // версия протокола
} xibridge_conn_t;


/**
    * \russian
    * Определение идентификатора несуществующего подключения
*/
#define conn_id_invalid 0

#if defined(__cplusplus)
extern "C" {
#endif

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
uint32_t  XI_EXPORT xibridge_set_connection_protocol_verion(xibridge_conn_t conn, xibridge_version_t ver);

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
   * Функция записи данных ("как есть", без формирования по протоколу) в устройство с помощью данного подключения
   * @param[in] conn_id идентификатор подключения
   * @param[in] buf данные для отправки в устройство 
   * @param[in] size длина данных 
   * @return 0 - если операция завершилась неудачно
   * \endrussian
*/
int  XI_EXPORT xibridge_write_connection(unsigned int conn_id, const unsigned char *buf, int size);

/**
   * \russian
   * Функция чтения данных ("как есть", без парсинга по протоколу) из устройства с помощью данного подключения
   * @param[in] conn_id идентификатор подключения
   * @param[in] buf буфер-приемник для получения данных 
   * @param[in] size длина буфера-приемника 
   * @return 0 - если операция завершилась неудачно
   * \endrussian
*/
int  XI_EXPORT xibridge_read_connection_buffer(unsigned int conn_id, unsigned char *buf, int size);

/**
   * \russian
   * Функция открытия подключения по сети к устройству через сервер (xibridge) при использовании расширенного идентификатора устройств
   
   * @param[in] recv_timeout таймаут ответа сервера
   * @param[out] pconn указатель переменной, куда будут  записаны данные подключения
   * @return код ошибки в случае неудачной оперции открытия, 0 - в случае успеха ненулевой идентификатор подключения в случае успешного подключения к устройству, 0 - в случае неудачи 
   * \endrussian
*/
uint32_t  XI_EXPORT xibridge_open_device_connection(const char *xi_net_uri, unsigned int recv_timeout, xibridge_conn_t *pconn);


uint32_t  XI_EXPORT xibridge_close_device_connection(xibridge_conn_t conn);

/**
   * \russian
   * Функция автоопределения максимальной версии протокола, поддерживаемого на сервере (urpc-, ximc-, xibridge-) 
   * @param[in] addr ip-адрес сервера
   * @param[in] send_timeout таймаут отправки запроса на сервер
   * @param[in] recv_timeout таймаут получения ответа сервера 
   * @return возвращает номер максимальной версии протокола (1, 2 или 3) в случае успешного операции, 0 - если определить версия не удалось 
   * \endrussian
*/
unsigned int  XI_EXPORT xibridge_detect_protocol_version(const char *addr, unsigned int send_timeout, unsigned int recv_timeout);

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
int  XI_EXPORT xibridge_device_request_response(unsigned int conn_id,
	                                         const unsigned char *req, 
											 int req_len, unsigned char *resp, 
                                             int resp_len, unsigned int *res_err
											);

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
int  XI_EXPORT xibridge_enumerate_adapter_devices(const char *addr, const char *adapter,
	unsigned char **result,
	unsigned int *pcount, unsigned int timeout,
	unsigned int* last_errno);


/**
   * \russian
   * Функция определения устройств с расширенными идентификаторами, доступных для работы на сервере (xibridge-)
   * Функция распределяет и заполняет массив расширенных идентификаторов по количеству определенных устройств
   * @param[in] addr ip-адрес сервера
   * @param[in] addr ip-адрес адаптера(?)
   * @param[out] result указатель на указатель, по которому будет распределен массив структур с описанием устройтств
   * @param[out] pcount указатель на переменную, куда будет помещено количесиво найденных устройств
   * @param[in] timeout таймаут ответа сервера
   * @param[out] last_errno указатель на переменную, куда будет помещен код ошибки в случае неудачной операции
   * @return 0 в случае неудачи
* \endrussian
*/void  /*XI_EXPORT*/ xibridge_enumerate_devices_ext(const char *addr,
	const char *adapter,
	ExtDevId **result,
	unsigned int *pcount, unsigned int timeout,
	unsigned int* last_errno);

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