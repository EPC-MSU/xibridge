#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "utils.h"



uint32_t AHex::_get_stream1_4(uint8_t **ptr)
{ 
    uint32_t val = 0;
    uint8_t *p = * ptr;
	if (littleEndian())
	{
		p += _tsize;
		for (int i = 0; i < _tsize; i++)
		{
			val <<= 8;
			val += *(--p);
		}
	}
	else
	{
		
		for (int i = 0; i < _tsize; i++)
		{
			val <<= 8;
			val += *(p++);
		}
	}
	*ptr += _tsize; 
    return val;
}

void AHex::put_stream1_4(uint8_t **ptr, uint32_t val)
{
    uint8_t *p = *ptr;
    if (littleEndian())
    {
        for (int i = 0; i < _tsize; i++)
        {
           *p++ = (uint8_t)((val >> (i * 8)) & 0xFF);
        }
    }
    else
    {
		for (int i = 0; i < _tsize; i++)
		{
			uint8_t v = (uint8_t)((val >> ((_tsize - 1 - 3) * 8)));
			*p++ = (uint8_t)((val >> ((_tsize-1-i) * 8)) & 0xFF);
		}
    }
    *ptr += _tsize;
 }

// функции формирования значений различных пределов из последовательности байт массива
void Hex8::_get_stream(uint8_t **ptr)
{
	value = **ptr;
	*ptr += 1;
}

void Hex32::_get_stream(uint8_t **ptr)
{
	/*
	uint8_t * p = *ptr;
	if (littleEndian())
		value = *(p)+*(p + 1) * 0x100 + *(p + 2) * 0x10000 + *(p + 3) * 0x1000000;
	else
		value = *(p + 3) + *(p + 2) * 0x100 + *(p + 1) * 0x10000 + *(p)* 0x1000000;
	*ptr += 4;
	*/
	value = _get_stream1_4(ptr);
}

// консруирует значение из симворльной десятичной ASCII- cтроки
// строка должна заканч. нулем
Hex32::Hex32(char *psymbol_name_decimal) :
AHex()
{
	_lend = false;
	value = atoi(psymbol_name_decimal);
}

void Hex16::_get_stream(uint8_t ** ptr)
{
	/*
	uint8_t * p = *ptr;
	if (littleEndian())
		value = *(p)+*(p + 1) * 0x100;
	else
		value = *(p + 1) + *p * 0x100;
	*ptr += 2;
	*/
	value = _get_stream1_4(ptr);
}

void Hex24::_get_stream(uint8_t ** ptr)
{
	/*
	uint8_t * p = *ptr;
	if (littleEndian())
		value = *(p)+*(p + 1) * 0x100 + *(p + 2) * 0x10000;
	else
		value = *(p + 2) + *(p + 1) * 0x100 + *(p)* 0x10000;
	*ptr += 3;
	*/
	_get_stream1_4(ptr);
}


void HexIDev3::get_stream(MBuf & stream)
{
    if (littleEndian())
    {
		stream >> _id_value >> _id_pid >> _id_vid >> _reserve;
    }
	else
	{
		stream >> _reserve >> _id_vid >> _id_pid >> _id_value;
	}
}

void HexIDev3::put_stream(MBuf & stream) const
{
    if (littleEndian())
    {
        stream << _id_value << _id_pid << _id_vid << _reserve;
    }
    else
    {
        stream << _reserve << _id_vid << _id_pid << _id_value;
    }
}

xibridge_device_t HexIDev3::toExtDevId() const
{
	xibridge_device_t exdevid;
	exdevid.reserve = _reserve;
	exdevid.id = (uint32_t)_id_value;
	exdevid.PID = (unsigned short)_id_pid;
	exdevid.VID = (unsigned short)_id_vid;
	return exdevid;
}

// конструктор простого буфера MBuf
// pdata может только расти
// это  конструктор для буфера чтения
MBuf::MBuf(const uint8_t *readyd, int size, bool readonly) :
ovrflow(0),
_rdon(readonly)
{
	origin_data = pdata = new uint8_t[dlen = size];
	memcpy(pdata, readyd, size);
}

// это конструстор для буфера записи
MBuf::MBuf(int size, bool readonly) :
ovrflow(0),
_rdon(readonly)
{
	origin_data = pdata = new uint8_t[dlen = size];
	memset(pdata, 0, dlen);
}

// получение из массива 4-байт-го значения 
MBuf& MBuf::operator >> (Hex32 &v)
{
	if (dlen - (pdata - origin_data)  < 4) ovrflow++;
	// буфер кончился ранее чем ожидалось
	else
	{
		// хватило
		v._get_stream(&pdata);
	}
	return *this;
}

// запись в массив 4-байтного значения 
MBuf& MBuf::operator << (Hex32 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 4) ovrflow++;
		// места в буфере не хватило
		else
		{
			// хватило Места, разберемся с порядком следования байт в памяти
            /*
			if (v.littleEndian())
			{
				// порядок здесь little endian
				*pdata++ = (uint8_t)(v & 0xFF);
				*pdata++ = (uint8_t)((v >> 8) & 0xFF);
				*pdata++ = (uint8_t)((v >> 16) & 0xFF);
				*pdata++ = (uint8_t)((v >> 24) & 0xFF);
			}
			else
			{
				// порядок здесь big endian
				*pdata++ = (uint8_t)((v >> 24) & 0xFF);
				*pdata++ = (uint8_t)((v >> 16) & 0xFF);
				*pdata++ = (uint8_t)((v >> 8) & 0xFF);
				*pdata++ = (uint8_t)(v & 0xFF);
			}
            */
            v.put_stream1_4(&pdata, v);
		}
	}
	return *this;
}

// получение из массива 3-байт-го значения 
MBuf& MBuf::operator >> (Hex24 &v)
{
	if (dlen - (pdata - origin_data)  < 3) ovrflow++;
	// буфер кончился ранее чем ожидалось
	else
	{
		// хватило
		v._get_stream(&pdata);
	}
	return *this;
}

// запись в массив 3-байтного значения 
MBuf& MBuf::operator << (Hex24 v)
{
	if (!_rdon)
	{


		if (dlen - (pdata - origin_data)  < 3) ovrflow++;
		// места в буфере не хватило
		else
		{
            /*
            // хватило
			// порядок здесь 
			if (v.littleEndian())
			{
				*pdata++ = (uint8_t)(v & 0xFF);
				*pdata++ = (uint8_t)((v >> 8) & 0xFF);
				*pdata++ = (uint8_t)((v >> 16) & 0xFF);

			}
			else
			{
				*pdata++ = (uint8_t)((v >> 16) & 0xFF);
				*pdata++ = (uint8_t)((v >> 8) & 0xFF);
				*pdata++ = (uint8_t)(v & 0xFF);
			}
            */
            v.put_stream1_4(&pdata, v);
		}
	}
	return *this;
}

// получение из массива 2-байт-го значения 
MBuf& MBuf::operator >> (Hex16 &v)
{
	if (dlen - (pdata - origin_data)  < 2) ovrflow++;
	// буфер кончился ранее чем ожидалось
	else
	{
		// хватило
		v._get_stream(&pdata);
	}
	return *this;
}

// запись в массив 2-байтного значения 
MBuf& MBuf::operator << (Hex16 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 2) ovrflow++;
		// места в буфере не хватило
		else
		{
            /*
			// хватило
			// порядок здесь 
			if (v.littleEndian())
			{
				*pdata++ = (uint8_t)(v & 0xFF);
				*pdata++ = (uint8_t)((v >> 8) & 0xFF);

			}
			else
			{
				*pdata++ = (uint8_t)((v >> 8) & 0xFF);
				*pdata++ = (uint8_t)(v & 0xFF);
			}
            */
            v.put_stream1_4(&pdata, v);
		}
	}
	return *this;
}

// получение из массива 1-байт-го значения 
MBuf& MBuf::operator >> (Hex8 &v)
{
	if (dlen - (pdata - origin_data)  < 1) ovrflow++;
	// буфер кончился ранее чем ожидалось
	else
	{
		// хватило
		v._get_stream(&pdata);
	}
	return *this;
}

// запись в массив 1-байтного значения 
MBuf& MBuf::operator << (Hex8 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 1) ovrflow++;
		// места в буфере не хватило
		else
		{
			// хватило
			// порядок здесь 
			*pdata++ = (uint8_t)(v & 0xFF);
		}
	}
	return *this;
}

// запись в буфер содержимого другого буфера  
MBuf& MBuf::operator << (const MBuf & src)
{
	int _len = src.realSize();
	int _dlen = restOfSize(-1);
	if (_dlen < _len) ovrflow++;
	else
	{
		memcpy(pdata, (uint8_t *)src, _len);
		pdata += _len;
	}
	return *this;
}

// запись в буфер содержимого HexIDev3  
MBuf& MBuf::operator << (const HexIDev3 &hidev)
{
    hidev.put_stream(*this);
    return *this;
}

// чтение из буфера содержимого HexIDev3  
MBuf& MBuf::operator >> (HexIDev3 &hidev)
{
    hidev.get_stream(*this);
    return *this;
}

// запись данных в буфер объекта
int MBuf::memwrite(const uint8_t *data, int len)
{
	// проверяем на перполнение
	if (_rdon || len < 0) return 0;

	// места в буфере явно не хватает
	if (dlen - (pdata - origin_data)  < len) { ovrflow++; return 0; }

	// место есть, записываем
	memcpy(pdata, data, len);
	pdata += len;
	return len;
}

// рераспределяет внутренний буфер, втавляя кусок 
// в начало старого внутреннего буфера
bool MBuf::meminsert_start(const uint8_t *what, int len)
{
	// проверяем на перполнение
	if (_rdon || len < 0) return false;
	// нужно для удаления на потом 
	const uint8_t * temp_origin = origin_data;
	// смотрим на смещение 
	int cur_pos = (int)(pdata - origin_data);
	// новый абсолютный размер
	dlen += len;
	origin_data = pdata = new uint8_t[dlen];

	memwrite(what, len);
	memwrite(temp_origin, dlen - len);
	// куда казал, туда и кажет
	pdata = origin_data + cur_pos + len;

	delete[] temp_origin;

	return true;
}


// вставляет два байта в начало и пересапр. внутр. буфер
bool MBuf::meminsert_start(uint8_t num1, uint8_t num2)
{
	// проверяем на перполнение
	if (_rdon) return false;
	// нужно для удаления на потом 
	const uint8_t * temp_origin = origin_data;
	// смотрим на смещение 
	int cur_pos = (int)(pdata - origin_data);
	// новый абсолютный размер
	dlen += 2;
	origin_data = pdata = new uint8_t[dlen];
	*this << Hex8(num1) << Hex8(num2);
	memwrite(temp_origin, dlen - 2);
	// куда казал, туда и кажет
	pdata = origin_data + cur_pos + 2;
	delete[] temp_origin;
	return true;
}


// чтение  данных из буфера объекта
int MBuf::memread(uint8_t *dest, int dlen, int len)
{
	// проверяем, можно ли скопировать
	if (len < 0 || len > dlen || restOfSize(-1) < len) return -1;
	// место есть, копируем
	memcpy(dest, pdata, len);
	pdata += len;
	return len;
}

// перемещение курсора массива
bool MBuf::mseek(int offset)
{
	if (dlen <= pdata + offset - origin_data || pdata + offset < origin_data) ovrflow++;
	else pdata += offset;
	return ovrflow != 0;
}

// перемещение курсора массива относительно его начала
bool MBuf::tot_seek(int offset)
{
	if (offset >= dlen || offset < 0) ovrflow++;
	else pdata = origin_data + offset;
	return ovrflow == 0;
}

// здесь даже в случае выхода за границу массива мы не ставим флаг ошибки
// просто возвр. -1
int MBuf::restOfSize(int from_pos) const
{
	if (from_pos == -1) from_pos = (int)(pdata - origin_data);
	if (from_pos < 0 || from_pos >= dlen) return -1;
	return (dlen - from_pos);
}

bvector MBuf::to_vector(bool rest) const
{
	bvector ret;
	if (rest)
	{
		if (dlen != 0)
		  ret.assign(pdata, pdata + restOfSize(-1));
	}
    else if (_rdon && pdata == origin_data)// non-read buffer all to vector
    {
        ret.assign(origin_data, origin_data + dlen);
    }
    else
	{
		if (pdata > origin_data)
		   ret.assign(origin_data, pdata);
	}
	return ret;
}


bvector &add_uint32_2_bvector(bvector & bv, uint32_t val)
{
	for (int i = 0; i < sizeof(uint32_t); i++)
	{
		bv.push_back((uint8_t)(val >> (24 - 8 * i)));
	}
	return bv;
}