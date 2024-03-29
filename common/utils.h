#ifndef _UTILS_H
#define  _UTILS_H
#include "defs.h"
#include "protocols.h"
#include "../xibridge.h"

#ifdef _MSC_VER

#include <io.h>
#include <fcntl.h>
#define portable_strdup _strdup
#define portable_snprintf _snprintf
#define portable_strncasecmp _strnicmp
#define portable_strcasecmp _stricmp

typedef SSIZE_T ssize_t;

#else

#define portable_strdup strdup
#define portable_snprintf snprintf
#define portable_strncasecmp strncasecmp
#define portable_strcasecmp strcasecmp

#endif

class AHex
{
public :
    AHex():_lend(false), _tsize(0) {}
    virtual bool littleEndian() const 
    {
        return _lend;
    }

    void put_stream1_4(
        uint8_t **ptr, 
        uint32_t val
    );
protected:
    uint32_t 
    _get_stream1_4(uint8_t **ptr);
   
    bool _lend;
    size_t _tsize;
};

/* 
   * все числа более байта в протоколах заданы в обратном порядке, поэтому
   * эти числа интерпретируются тоже в обратном порядке
   * последующие 4 класса просто вспомогательные, облегчающие прочтение нескольких байт от 1-го до 4-х
*/
class Hex32 : public AHex
{
public:
    Hex32(
         uint32_t v = 0, 
         bool lit_end = false
    ):
         value(v)
    { 
        _lend = lit_end; 
        _tsize = sizeof(uint32_t); 
    }

    Hex32(char *psymbol_name_decimal);
    
    operator uint32_t () const 
    { return value; }
    
    Hex32& operator= (uint32_t v) 
    { value = v; return *this; }
    
    void _get_stream(uint8_t **ptr);
private:
    uint32_t value;
};

class Hex8 : public AHex
{
public:
    Hex8(uint8_t v = 0):
         value(v)
    { 
        _tsize = sizeof(uint8_t); 
    }
    
    void _get_stream(uint8_t **ptr);
    operator uint8_t () const 
    { return value; }
    
    Hex8&  operator += (uint8_t a) 
    { 
        value += a; 
        return *this; 
    }
    Hex8& operator = (uint8_t v)
    {
        value = v; 
        return *this; 
    }

private:
    uint8_t value;
};

class Hex16 : public AHex
{
public:
    Hex16(
         uint16_t v = 0, 
         bool lit_end = false
    ):
         value(v)
    { 
        _lend = lit_end;  
        _tsize = sizeof(uint16_t); 
    }
    void _get_stream(uint8_t **ptr);
    
    operator uint16_t () const 
    { return value; }
    
    Hex16& operator = (uint16_t v)
    { 
        value = v; 
        return *this; 
    }
private:
    uint16_t value;
};

class Hex24 : public AHex
{
public:
    Hex24(
          uint32_t v = 0, 
          bool lit_end = false
          ):
          value(v)
    { 
        _lend = lit_end; 
        _tsize = 3; 
    }
    
    void _get_stream(uint8_t **ptr);

    operator uint32_t ()
    { return value; }

    Hex24& operator = (uint32_t v)
    { 
        value = v; 
        return *this; 
    }

private:
    uint32_t value;
};

/*
* Класс для хранения 12-байтных идентификаторов устройств
*/

class MBuf;
class DevId;
class HexIDev3 : public AHex
{
public:
    HexIDev3(
        const DevId *pdevid = nullptr,
        bool lit_end = false
        );
        
    void get_stream(MBuf& mbuf);
    
    void put_stream(MBuf& mbuf) const ;
    
    virtual bool littleEndian() const 
    { return _lend; }

    DevId toDevId() const; 
    
private:
    Hex32 _id_value, _reserve;
    Hex16 _id_pid, _id_vid;
 };

/*
    * Простой класс для хранения данных буфера и контроля длины и текущего указателя;
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

    MBuf(
        const uint8_t *readyd, 
        size_t size, 
        bool readonly = true
    );

    MBuf(
        int size, 
        bool readonly = false
    );
    
    ~MBuf() 
    { delete[] origin_data; }
    
    // преобразование типа
    operator const uint8_t *() const 
    { return origin_data; }

    // просто уловка
    operator uint8_t *() const 
    { return origin_data; }
    
    uint8_t *cur_data() const 
    { return pdata; }
    
    size_t bufferLen() const 
    { return dlen; }
    
    void setRdOnly()
    { _rdon = true; }

    MBuf& operator << (Hex32 v);
    MBuf& operator >> (Hex32 &v);
    MBuf& operator << (Hex24 v);
    MBuf& operator >> (Hex24 &v);
    MBuf& operator << (Hex16 v);
    MBuf& operator >> (Hex16 &v);
    MBuf& operator << (Hex8 v);
    MBuf& operator >> (Hex8 &v);
    MBuf& operator << (const HexIDev3 &v);
    MBuf& operator >> (HexIDev3 &v);
    MBuf& operator << (const MBuf &src);

    // дозапись данных в буфер этого объекта и перемещкение указателя на длину записанных данных
    size_t memwrite(
        const uint8_t *data, 
        size_t len
    );
    // копирование данных из буфера (когда объект для чтения) в буфер-приемник и перемещкение указателя на длину скопированных данных
    // dest - Приемник
    // dlen - длина приемника
    // len - длина копируемых данных
    // возвр. дут, еслив  все нормально
    size_t memread(
        uint8_t *dest, 
        size_t dlen, 
        size_t len
    );

    // рерраспределяет внутренний буфер, втавляя кусок 
    // в начало
    bool meminsert_start(
        const uint8_t *what, 
        size_t len
    );
    // вставляет два байта в начало и пересапр. внутр. буфер
    bool meminsert_start(
        uint8_t num1, 
        uint8_t num2
    );

    // перемещение тек. указателя
    bool mseek(int offest);
    
    // установка указателя на позицию от начала
    bool tot_seek(int offset);

    // проверка, были ли нарушения
    bool wasBroken() const 
    { return ovrflow != 0; }
    
    size_t realSize() const 
    { return (int)(pdata - origin_data); }

    // вычисление отсавшегося размера, начиная с позиции from_pos
    size_t restOfSize(int from_pos) const;
        
    uint32_t CRC32(int from_pos);
    
    bvector to_vector(bool rest = false) const;

protected:
    size_t dlen;     // размер распределенной памяти - за это число нельзя выйти никогда
    uint8_t *origin_data;  // указатель на данные
    uint8_t *pdata;   // текущее положение указателя
    uint8_t ovrflow; // признак переполнения
    bool _rdon;
};

extern bvector &add_uint32_2_bvector(
           bvector & bv, 
           uint32_t val
       );

extern bool xi_net_dev_uris(
           MBuf& result, 
           const char *server, 
           const bvector& data_devid, 
           int count
       );

extern bvector &add_dev_id_bvector_net_order(
           bvector & bv, 
           const xibridge_device_t &devid
       );

#endif // 
