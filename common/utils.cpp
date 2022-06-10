#include "defs.h"
#include "utils.h"

uint32_t AHex::_get_stream1_4(uint8_t **ptr)
{ 
    uint32_t val = 0;
    uint8_t *p = * ptr;
	if (littleEndian())
	{
		p += _tsize;
		for (int i = 0; i < (int)_tsize; i++)
		{
			val <<= 8;
			val += *(--p);
		}
	}
	else
	{
		
		for (int i = 0; i < (int)_tsize; i++)
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
        for (int i = 0; i < (int)_tsize; i++)
        {
           *p++ = (uint8_t)((val >> (i * 8)) & 0xFF);
        }
    }
    else
    {
		for (int i = 0; i < (int)_tsize; i++)
		{
			uint8_t v = (uint8_t)((val >> ((_tsize - 1 - 3) * 8)));
			*p++ = (uint8_t)((val >> ((_tsize-1-i) * 8)) & 0xFF);
		}
    }
    *ptr += _tsize;
 }


void Hex8::_get_stream(uint8_t **ptr)
{
	value = **ptr;
	*ptr += 1;
}

void Hex32::_get_stream(uint8_t **ptr)
{
	value = _get_stream1_4(ptr);
}

Hex32::Hex32(char *psymbol_name_decimal) :
AHex()
{
	_lend = false;
	value = atoi(psymbol_name_decimal);
}

void Hex16::_get_stream(uint8_t ** ptr)
{
	value = _get_stream1_4(ptr);
}

void Hex24::_get_stream(uint8_t ** ptr)
{
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

MBuf::MBuf(const uint8_t *readyd, size_t size, bool readonly) :
ovrflow(0),
_rdon(readonly)
{
	origin_data = pdata = new uint8_t[dlen = size];
	memcpy(pdata, readyd, size);
}

MBuf::MBuf(int size, bool readonly) :
ovrflow(0),
_rdon(readonly)
{
	origin_data = pdata = new uint8_t[dlen = size];
	memset(pdata, 0, dlen);
}

MBuf& MBuf::operator >> (Hex32 &v)
{
	if (dlen - (pdata - origin_data)  < 4) ovrflow++;
	else
	{
		v._get_stream(&pdata);
	}
	return *this;
}

MBuf& MBuf::operator << (Hex32 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 4) ovrflow++;
		else
		{
			v.put_stream1_4(&pdata, v);
		}
	}
	return *this;
}

MBuf& MBuf::operator >> (Hex24 &v)
{
	if (dlen - (pdata - origin_data)  < 3) ovrflow++;
	else
	{
		v._get_stream(&pdata);
	}
	return *this;
}

MBuf& MBuf::operator << (Hex24 v)
{
	if (!_rdon)
	{


		if (dlen - (pdata - origin_data)  < 3) ovrflow++;
		else
		{
            v.put_stream1_4(&pdata, v);
		}
	}
	return *this;
}

MBuf& MBuf::operator >> (Hex16 &v)
{
	if (dlen - (pdata - origin_data)  < 2) ovrflow++;
	else
	{
		v._get_stream(&pdata);
	}
	return *this;
}

MBuf& MBuf::operator << (Hex16 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 2) ovrflow++;
		else
		{
            v.put_stream1_4(&pdata, v);
		}
	}
	return *this;
}

MBuf& MBuf::operator >> (Hex8 &v)
{
	if (dlen - (pdata - origin_data)  < 1) ovrflow++;
	else
	{
		v._get_stream(&pdata);
	}
	return *this;
}

MBuf& MBuf::operator << (Hex8 v)
{
	if (!_rdon)
	{
		if (dlen - (pdata - origin_data)  < 1) ovrflow++;
		else
		{
			*pdata++ = (uint8_t)(v & 0xFF);
		}
	}
	return *this;
}

 
MBuf& MBuf::operator << (const MBuf & src)
{
	size_t _len = src.realSize();
	size_t _dlen = restOfSize(-1);
	if (_dlen < _len) ovrflow++;
	else
	{
		memcpy(pdata, (uint8_t *)src, _len);
		pdata += _len;
	}
	return *this;
}

  
MBuf& MBuf::operator << (const HexIDev3 &hidev)
{
    hidev.put_stream(*this);
    return *this;
}

MBuf& MBuf::operator >> (HexIDev3 &hidev)
{
    hidev.get_stream(*this);
    return *this;
}

size_t MBuf::memwrite(const uint8_t *data, size_t len)
{
	if (_rdon || len < 0) return 0;

	if (dlen - (pdata - origin_data)  < len) { ovrflow++; return 0; }

	memcpy(pdata, data, len);
	pdata += len;
	return len;
}

bool MBuf::meminsert_start(const uint8_t *what, size_t len)
{
	if (_rdon || len < 0) return false;
	const uint8_t * temp_origin = origin_data;
	int cur_pos = (int)(pdata - origin_data);
	dlen += len;
	origin_data = pdata = new uint8_t[dlen];

	memwrite(what, len);
	memwrite(temp_origin, dlen - len);
	pdata = origin_data + cur_pos + len;

	delete[] temp_origin;

	return true;
}


bool MBuf::meminsert_start(uint8_t num1, uint8_t num2)
{
	if (_rdon) return false;
	const uint8_t * temp_origin = origin_data;
	int cur_pos = (int)(pdata - origin_data);
	dlen += 2;
	origin_data = pdata = new uint8_t[dlen];
	*this << Hex8(num1) << Hex8(num2);
	memwrite(temp_origin, dlen - 2);
	pdata = origin_data + cur_pos + 2;
	delete[] temp_origin;
	return true;
}


size_t MBuf::memread(uint8_t *dest, size_t dlen, size_t len)
{
	if (len < 0 || len > dlen || restOfSize(-1) < len) return -1;
	memcpy(dest, pdata, len);
	pdata += len;
	return len;
}

bool MBuf::mseek(int offset)
{
    int len;
	if ((int)dlen < (len = (int)(pdata + offset - origin_data)) || (pdata + offset) < origin_data) ovrflow++;
	else pdata += offset;
	return ovrflow != 0;
}

bool MBuf::tot_seek(int offset)
{
	if (offset >= (int)dlen || offset < 0) ovrflow++;
	else pdata = origin_data + offset;
	return ovrflow == 0;
}

size_t MBuf::restOfSize(int from_pos) const
{
	if (from_pos == -1) from_pos = (int)(pdata - origin_data);
	if (from_pos < 0 || from_pos >= (int)dlen) return -1;
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


bvector &add_value_2_bvector_net_order(bvector & bv, uint32_t val, size_t size)
{
    for (int i = 0; i < (int)size; i++)
    {
        bv.push_back((uint8_t)(val >> (((int)size-1)*8 - 8 * i)));
    }
    return bv;
}

bvector &add_uint32_2_bvector(bvector & bv, uint32_t val)
{
    return add_value_2_bvector_net_order(bv, val, sizeof(uint32_t));
}

bvector &add_uint16_2_bvector(bvector & bv, uint16_t val)
{
    return add_value_2_bvector_net_order(bv,(uint32_t) val, sizeof(uint16_t));
}

bvector &add_dev_id_bvector_net_order(bvector & bv, const xibridge_device_t &devid)
{
    add_uint32_2_bvector(bv, devid.reserve);
    add_uint16_2_bvector(bv, devid.VID);
    add_uint16_2_bvector(bv, devid.PID);
    add_uint32_2_bvector(bv, devid.id);
    return bv;
}

static const char *_xinet_pre = "xi-net://";

bool xi_net_dev_uris(MBuf& result, const char *server, const bvector& data_devid, int count)
{
    uint8_t str_dev_id[sizeof(xibridge_device_t) * 2];
    MBuf read_buf(data_devid.data(),(int) data_devid.size());
    while (count--)
    {
        HexIDev3 devid;
        read_buf >> devid;
        xibridge_device_t dev = devid.toExtDevId();
		portable_snprintf((char *)str_dev_id, sizeof(xibridge_device_t)* 2, "%X%X%X%X", dev.reserve, dev.VID, dev.PID, dev.id);
        result.memwrite((const uint8_t *)_xinet_pre, (int)strlen(_xinet_pre));
        result.memwrite((const uint8_t *)server, (int)strlen(server));
        result << Hex8('/');
        result.memwrite(str_dev_id, (int)strlen((char *)str_dev_id));
        result << Hex8(0x0);
    }
    if (result.realSize() != 0) result << Hex8(0x0);

    return !read_buf.wasBroken() && !result.wasBroken();
}