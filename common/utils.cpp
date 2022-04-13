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

// ������� ������������ �������� ��������� �������� �� ������������������ ���� �������
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

// ����������� �������� �� ����������� ���������� ASCII- c�����
// ������ ������ ������. �����
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

// ����������� �������� ������ MBuf
// pdata ����� ������ �����
// ���  ����������� ��� ������ ������
MBuf::MBuf(const uint8_t *readyd, int size, bool readonly) :
ovrflow(0),
_rdon(readonly)
{
	origin_data = pdata = new uint8_t[dlen = size];
	memcpy(pdata, readyd, size);
}

// ��� ����������� ��� ������ ������
MBuf::MBuf(int size, bool readonly) :
ovrflow(0),
_rdon(readonly)
{
	origin_data = pdata = new uint8_t[dlen = size];
	memset(pdata, 0, dlen);
}

// ��������� �� ������� 4-����-�� �������� 
MBuf& MBuf::operator >> (Hex32 &v)
{
	if (dlen - (pdata - origin_data)  < 4) ovrflow++;
	// ����� �������� ����� ��� ���������
	else
	{
		// �������
		v._get_stream(&pdata);
	}
	return *this;
}

// ������ � ������ 4-�������� �������� 
MBuf& MBuf::operator << (Hex32 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 4) ovrflow++;
		// ����� � ������ �� �������
		else
		{
			// ������� �����, ���������� � �������� ���������� ���� � ������
            /*
			if (v.littleEndian())
			{
				// ������� ����� little endian
				*pdata++ = (uint8_t)(v & 0xFF);
				*pdata++ = (uint8_t)((v >> 8) & 0xFF);
				*pdata++ = (uint8_t)((v >> 16) & 0xFF);
				*pdata++ = (uint8_t)((v >> 24) & 0xFF);
			}
			else
			{
				// ������� ����� big endian
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

// ��������� �� ������� 3-����-�� �������� 
MBuf& MBuf::operator >> (Hex24 &v)
{
	if (dlen - (pdata - origin_data)  < 3) ovrflow++;
	// ����� �������� ����� ��� ���������
	else
	{
		// �������
		v._get_stream(&pdata);
	}
	return *this;
}

// ������ � ������ 3-�������� �������� 
MBuf& MBuf::operator << (Hex24 v)
{
	if (!_rdon)
	{


		if (dlen - (pdata - origin_data)  < 3) ovrflow++;
		// ����� � ������ �� �������
		else
		{
            /*
            // �������
			// ������� ����� 
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

// ��������� �� ������� 2-����-�� �������� 
MBuf& MBuf::operator >> (Hex16 &v)
{
	if (dlen - (pdata - origin_data)  < 2) ovrflow++;
	// ����� �������� ����� ��� ���������
	else
	{
		// �������
		v._get_stream(&pdata);
	}
	return *this;
}

// ������ � ������ 2-�������� �������� 
MBuf& MBuf::operator << (Hex16 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 2) ovrflow++;
		// ����� � ������ �� �������
		else
		{
            /*
			// �������
			// ������� ����� 
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

// ��������� �� ������� 1-����-�� �������� 
MBuf& MBuf::operator >> (Hex8 &v)
{
	if (dlen - (pdata - origin_data)  < 1) ovrflow++;
	// ����� �������� ����� ��� ���������
	else
	{
		// �������
		v._get_stream(&pdata);
	}
	return *this;
}

// ������ � ������ 1-�������� �������� 
MBuf& MBuf::operator << (Hex8 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 1) ovrflow++;
		// ����� � ������ �� �������
		else
		{
			// �������
			// ������� ����� 
			*pdata++ = (uint8_t)(v & 0xFF);
		}
	}
	return *this;
}

// ������ � ����� ����������� ������� ������  
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

// ������ � ����� ����������� HexIDev3  
MBuf& MBuf::operator << (const HexIDev3 &hidev)
{
    hidev.put_stream(*this);
    return *this;
}

// ������ �� ������ ����������� HexIDev3  
MBuf& MBuf::operator >> (HexIDev3 &hidev)
{
    hidev.get_stream(*this);
    return *this;
}

// ������ ������ � ����� �������
int MBuf::memwrite(const uint8_t *data, int len)
{
	// ��������� �� �����������
	if (_rdon || len < 0) return 0;

	// ����� � ������ ���� �� �������
	if (dlen - (pdata - origin_data)  < len) { ovrflow++; return 0; }

	// ����� ����, ����������
	memcpy(pdata, data, len);
	pdata += len;
	return len;
}

// �������������� ���������� �����, ������� ����� 
// � ������ ������� ����������� ������
bool MBuf::meminsert_start(const uint8_t *what, int len)
{
	// ��������� �� �����������
	if (_rdon || len < 0) return false;
	// ����� ��� �������� �� ����� 
	const uint8_t * temp_origin = origin_data;
	// ������� �� �������� 
	int cur_pos = (int)(pdata - origin_data);
	// ����� ���������� ������
	dlen += len;
	origin_data = pdata = new uint8_t[dlen];

	memwrite(what, len);
	memwrite(temp_origin, dlen - len);
	// ���� �����, ���� � �����
	pdata = origin_data + cur_pos + len;

	delete[] temp_origin;

	return true;
}


// ��������� ��� ����� � ������ � ��������. �����. �����
bool MBuf::meminsert_start(uint8_t num1, uint8_t num2)
{
	// ��������� �� �����������
	if (_rdon) return false;
	// ����� ��� �������� �� ����� 
	const uint8_t * temp_origin = origin_data;
	// ������� �� �������� 
	int cur_pos = (int)(pdata - origin_data);
	// ����� ���������� ������
	dlen += 2;
	origin_data = pdata = new uint8_t[dlen];
	*this << Hex8(num1) << Hex8(num2);
	memwrite(temp_origin, dlen - 2);
	// ���� �����, ���� � �����
	pdata = origin_data + cur_pos + 2;
	delete[] temp_origin;
	return true;
}


// ������  ������ �� ������ �������
int MBuf::memread(uint8_t *dest, int dlen, int len)
{
	// ���������, ����� �� �����������
	if (len < 0 || len > dlen || restOfSize(-1) < len) return -1;
	// ����� ����, ��������
	memcpy(dest, pdata, len);
	pdata += len;
	return len;
}

// ����������� ������� �������
bool MBuf::mseek(int offset)
{
	if (dlen <= pdata + offset - origin_data || pdata + offset < origin_data) ovrflow++;
	else pdata += offset;
	return ovrflow != 0;
}

// ����������� ������� ������� ������������ ��� ������
bool MBuf::tot_seek(int offset)
{
	if (offset >= dlen || offset < 0) ovrflow++;
	else pdata = origin_data + offset;
	return ovrflow == 0;
}

// ����� ���� � ������ ������ �� ������� ������� �� �� ������ ���� ������
// ������ �����. -1
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