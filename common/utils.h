#ifndef _UTILS_H
#define  _UTILS_H
#include <vector>
#include "defs.h"

/* * все числа более байта в протоколах заданы в обратном порядке, поэтому
   * эти числа интерпретируются тоже в обратном порядке
   * последующие 4 класса просто вспомогательные, облегчающие прочтение нескольких байт от 1-го до 4-х
*/
class Hex32
{
public:
	//Hex32(uint8 **ptr):_lend(FALSE){_get_stream(ptr);}
	Hex32(uint32 v = 0, bool lit_end = FALSE) :value(v), _lend(lit_end){}

	Hex32(char *psymbol_name_decimal);
	operator uint32 () const { return value; }
	Hex32& operator= (uint32 v) { value = v; return *this; }
	void _get_stream(uint8 **ptr);
	bool littleEndian() const { return _lend; }
private:
	uint32 value;
	bool _lend;
};

class Hex8
{
public:
	Hex8(uint8 v = 0) :value(v){}
	//Hex8(uint8 ** ptr){_get_stream(ptr);}
	void _get_stream(uint8 **ptr);
	operator uint8 () const { return value; }
	Hex8&  operator+=(uint8 a) { value += a; return *this; }
	Hex8& operator=(uint8 v){ value = v; return *this; }

private:
	uint8 value;
};

class Hex16
{
public:
	Hex16(uint16 v = 0, bool lit_end = FALSE) :value(v), _lend(lit_end){}
	//Hex16(uint8 ** ptr):_lend(FALSE){_get_stream(ptr);};
	void _get_stream(uint8 **ptr);
	operator uint16 () const { return value; }
	Hex16& operator=(uint16 v){ value = v; return *this; }

	bool littleEndian() const { return _lend; }
private:
	uint16 value;
	bool _lend;
};

class Hex24
{
public:
	Hex24(uint32 v = 0, bool lit_end = FALSE) :value(v), _lend(lit_end){}
	//Hex24(uint8 ** ptr):_lend(FALSE) {_get_stream(ptr);};
	void _get_stream(uint8 **ptr);
	operator uint32 (){ return value; }
	Hex24& operator = (uint32 v){ value = v; return *this; }
	bool littleEndian() const { return _lend; }
private:
	uint32 value;
	bool _lend;
};

/* * Простой класс для хранения данных буфера и контроля длины и текущего указателя;
* подходит для чтения данных и занесения данных
* если длины не хватает ошибка устанавливается в неноль, запись не происходит,
* порчи памяти не случается;
* по окончании желаемых операций достаточно проверить  состояние флага переполнения
* объектов Mbuf, он должен == 0 !

* смысл этого класса кроме краткости использования и удобства в том,
* чтобы в случае какого-либо выхода за границы массива не испортить память, 
* а просто установить флажок ошибки
*/
class MBuf
{
public:

	MBuf(const uint8 *readyd, int size, bool readonly = TRUE);
	MBuf(int size, bool readonly = FALSE);
	~MBuf() { delete[] origin_data; }
	// преобразование типа
	operator const uint8 *() const { return origin_data; }
	// просто уловка
	operator uint8 *() const { return origin_data; }
	uint8 * cur_data() const { return pdata; }
	int bufferLen() const { return dlen; }
	void setRdOnly(){ _rdon = TRUE; }

	MBuf& operator << (Hex32 v);
	MBuf& operator >> (Hex32 &v);
	MBuf& operator << (Hex24 v);
	MBuf& operator >> (Hex24 &v);
	MBuf& operator << (Hex16 v);
	MBuf& operator >> (Hex16 &v);
	MBuf& operator << (Hex8 v);
	MBuf& operator >> (Hex8 &v);
	// запись в буфер содержимого другого буфера 
	MBuf& operator << (const MBuf &src);

	// дозапись данных в буфер этого объекта и перемещкение указателя на длину записанных данных
	int memwrite(const uint8 *data, int len);
	// копирование данных из буфера (когда объект для чтения) в буфер-приемник и перемещкение указателя на длину скопированных данных
	// dest - Приемник
	// dlen - длина приемника
	// len - длина копируемых данных
	// возвр. дут, еслив  все нормально
	int memread(uint8 *dest, int dlen, int len);

	// рерраспределяет внутренний буфер, втавляя кусок 
	// в начало
	bool meminsert_start(const uint8 *what, int len);
	// вставляет два байта в начало и пересапр. внутр. буфер
	bool meminsert_start(uint8 num1, uint8 num2);

	// перемещение тек. указателя
	bool mseek(int offest);
	// установка указателя на позицию от начала
	bool tot_seek(int offset);

	// проверка, были ли нарушения
	bool wasBroken() const { return ovrflow != 0; }
	int realSize() const { return (pdata - origin_data); }
	// вычисление отсавшегося размера, начиная с позиции from_pos
	int restOfSize(int from_pos) const;
		
	uint32 CRC32(int from_pos);
	bvector to_vector(bool rest = false) const;

protected:
	int dlen;     // размер распределенной памяти - за это число нельзя выйти никогда
	uint8 *origin_data;  // указатель на данные
	uint8 *pdata;   // текущее положение указателя
	uint8 ovrflow; // признак переполнения
	bool _rdon;
};

#endif // 