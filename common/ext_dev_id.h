#ifndef _EXT_DEV_ID_H
#define _EXT_DEV_ID_H
/*
* Структура для работы с расширеным идентификаторм устройства (используется в протоколе обмена версии 3 и выше)
*/
typedef struct
{
	unsigned int reserve;
	unsigned short int VID;
	unsigned short int PID;
	unsigned int id;
} ExtDevId;

#endif