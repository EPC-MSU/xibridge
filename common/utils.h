#ifndef _UTILS_H
#define  _UTILS_H
#include <vector>
#include "defs.h"

/* * ��� ����� ����� ����� � ���������� ������ � �������� �������, �������
   * ��� ����� ���������������� ���� � �������� �������
   * ����������� 4 ������ ������ ���������������, ����������� ��������� ���������� ���� �� 1-�� �� 4-�
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

/* * ������� ����� ��� �������� ������ ������ � �������� ����� � �������� ���������;
* �������� ��� ������ ������ � ��������� ������
* ���� ����� �� ������� ������ ��������������� � ������, ������ �� ����������,
* ����� ������ �� ���������;
* �� ��������� �������� �������� ���������� ���������  ��������� ����� ������������
* �������� Mbuf, �� ������ == 0 !

* ����� ����� ������ ����� ��������� ������������� � �������� � ���,
* ����� � ������ ������-���� ������ �� ������� ������� �� ��������� ������, 
* � ������ ���������� ������ ������
*/
class MBuf
{
public:

	MBuf(const uint8 *readyd, int size, bool readonly = TRUE);
	MBuf(int size, bool readonly = FALSE);
	~MBuf() { delete[] origin_data; }
	// �������������� ����
	operator const uint8 *() const { return origin_data; }
	// ������ ������
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
	// ������ � ����� ����������� ������� ������ 
	MBuf& operator << (const MBuf &src);

	// �������� ������ � ����� ����� ������� � ������������ ��������� �� ����� ���������� ������
	int memwrite(const uint8 *data, int len);
	// ����������� ������ �� ������ (����� ������ ��� ������) � �����-�������� � ������������ ��������� �� ����� ������������� ������
	// dest - ��������
	// dlen - ����� ���������
	// len - ����� ���������� ������
	// �����. ���, �����  ��� ���������
	int memread(uint8 *dest, int dlen, int len);

	// ��������������� ���������� �����, ������� ����� 
	// � ������
	bool meminsert_start(const uint8 *what, int len);
	// ��������� ��� ����� � ������ � ��������. �����. �����
	bool meminsert_start(uint8 num1, uint8 num2);

	// ����������� ���. ���������
	bool mseek(int offest);
	// ��������� ��������� �� ������� �� ������
	bool tot_seek(int offset);

	// ��������, ���� �� ���������
	bool wasBroken() const { return ovrflow != 0; }
	int realSize() const { return (pdata - origin_data); }
	// ���������� ����������� �������, ������� � ������� from_pos
	int restOfSize(int from_pos) const;
		
	uint32 CRC32(int from_pos);
	bvector to_vector(bool rest = false) const;

protected:
	int dlen;     // ������ �������������� ������ - �� ��� ����� ������ ����� �������
	uint8 *origin_data;  // ��������� �� ������
	uint8 *pdata;   // ������� ��������� ���������
	uint8 ovrflow; // ������� ������������
	bool _rdon;
};

#endif // 