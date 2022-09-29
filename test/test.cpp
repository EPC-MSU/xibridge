#include <iostream>
#include "../common/defs.h"
#include <../common/protocols.h>
#include "../vendor/acutest/include/acutest.h"
#include <bindy/bindy-static.h>

/*
* Commnads ids - needed for testing
*/
#define PROTO_1_OPEN  1
#define PROTO_1_CLOSE 2 
#define PROTO_1_RAW   0 
#define PROTO_1_ENUM  3

#define PROTO_2_OPEN  1
#define PROTO_2_CLOSE 2
#define PROTO_2_CMD   3

#define PROTO_3_OPEN  1
#define PROTO_3_CLOSE 2
#define PROTO_3_CMD   3
#define PROTO_3_VER   5
#define PROTO_3_ENUM  4

// from ximc.h
typedef struct
{
    unsigned int NomVoltage;    /**< \english Rated voltage in tens of mV. Controller will keep the voltage drop on motor below this value if ENGINE_LIMIT_VOLT flag is set (used with DC only). \endenglish \russian Номинальное напряжение мотора в десятках мВ. Контроллер будет сохранять напряжение на моторе не выше номинального, если установлен флаг ENGINE_LIMIT_VOLT (используется только с DC двигателем). \endrussian */
    unsigned int NomCurrent;    /**< \english Rated current (in mA). Controller will keep current consumed by motor below this value if ENGINE_LIMIT_CURR flag is set. Range: 15..8000 \endenglish \russian Номинальный ток через мотор (в мА). Ток стабилизируется для шаговых и может быть ограничен для DC(если установлен флаг ENGINE_LIMIT_CURR). Диапазон: 15..8000 \endrussian */
    unsigned int NomSpeed;  /**< \english Nominal (maximum) speed (in whole steps/s or rpm for DC and stepper motor as a master encoder). Controller will keep motor shaft RPM below this value if ENGINE_LIMIT_RPM flag is set. Range: 1..100000. \endenglish \russian Номинальная (максимальная) скорость (в целых шагах/с или rpm для DC и шагового двигателя в режиме ведущего энкодера). Контроллер будет сохранять скорость мотора не выше номинальной, если установлен флаг ENGINE_LIMIT_RPM. Диапазон: 1..100000. \endrussian */
    unsigned int uNomSpeed; /**< \english The fractional part of a nominal speed in microsteps (is only used with stepper motor). Microstep size and the range of valid values for this field depend on selected step division mode (see MicrostepMode field in engine_settings). \endenglish \russian Микрошаговая часть номинальной скорости мотора (используется только с шаговым двигателем). Величина микрошага и диапазон допустимых значений для данного поля зависят от выбранного режима деления шага (см. поле MicrostepMode в engine_settings). \endrussian */
    unsigned int EngineFlags;   /**< \english \ref flagset_engineflags "Flags of engine settings". \endenglish \russian \ref flagset_engineflags "Флаги параметров мотора". \endrussian */
    int Antiplay;   /**< \english Number of pulses or steps for backlash (play) compensation procedure. Used if ENGINE_ANTIPLAY flag is set. \endenglish \russian Количество шагов двигателя или импульсов энкодера, на которое позиционер будет отъезжать от заданной позиции для подхода к ней с одной и той же стороны. Используется, если установлен флаг ENGINE_ANTIPLAY. \endrussian */
    unsigned int MicrostepMode; /**< \english \ref flagset_microstepmode "Flags of microstep mode". \endenglish \russian \ref flagset_microstepmode "Флаги параметров микрошагового режима". \endrussian */
    unsigned int StepsPerRev;   /**< \english Number of full steps per revolution(Used with stepper motor only). Range: 1..65535. \endenglish \russian Количество полных шагов на оборот(используется только с шаговым двигателем). Диапазон: 1..65535. \endrussian */
} engine_settings_t;

// from urmc.h
typedef struct
{
    uint8_t Manufacturer[16]; /**< \~english Manufacturer name. The name is set by the manufacturer. \~russian Имя производителя. Устанавливается производителем. */
    uint8_t ProductName[16]; /**< \~english Product name. The name is set by the manufacturer. \~russian Название продукта. Устанавливается производителем. */
    uint8_t ControllerName[16]; /**< \~english User controller name. This name can be set by user via additional command. \~russian Пользовательское имя контроллера. Может быть установлено пользователем с помощью отдельной команды. */
    uint8_t HardwareMajor; /**< \~english The major number of the hardware version. \~russian Основной номер версии железа. */
    uint8_t HardwareMinor; /**< \~english Minor number of the hardware version. \~russian Второстепенный номер версии железа. */
    uint16_t HardwareBugfix; /**< \~english Number of edits for this release of hardware. \~russian Номер правок этой версии железа. */
    uint8_t BootloaderMajor; /**< \~english Bootloader major version number. \~russian Мажорный номер версии загрузчика. */
    uint8_t BootloaderMinor; /**< \~english Bootloader minor version number. \~russian Минорный номер версии загрузчика. */
    uint16_t BootloaderBugfix; /**< \~english Bootloader release version number. \~russian Номер релиза версии загрузчика. */
    uint8_t FirmwareMajor; /**< \~english Firmware major version number. \~russian Мажорный номер версии прошивки. */
    uint8_t FirmwareMinor; /**< \~english Firmware minor version number. \~russian Минорный номер версии прошивки. */
    uint16_t FirmwareBugfix; /**< \~english Firmware release version number. \~russian Номер релиза версии прошивки. */
    uint32_t SerialNumber; /**< \~english Device serial number. \~russian Серийный номер изделия. */
    uint8_t reserved[8]; /**< \~english  \~russian  */

} urmc_get_identity_information_t;

PACK(
struct _geng_re
{
    char gets[4];
    engine_settings_t settings;
});

typedef struct _geng_re re_geng;

static void test_request_proto1()
{
    TEST_MSG("?test_request_proto1?");

    uint32_t err;
    Protocol1 proto(&err, false);
    TEST_CHECK (proto.create_version_request(0).size() == 0);
    bvector  reqw = proto.create_open_request(1, 1000);

    const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_1_OPEN, proto.get_cmd_shema());
    TEST_CHECK(cm1.is_match(reqw.data(), (int)reqw.size(), 1, 1));

    reqw = proto.create_close_request(2, 2000);

    const cmd_schema & cm2 = cmd_schema::get_schema(PROTO_1_CLOSE, proto.get_cmd_shema());
    TEST_CHECK(cm2.is_match(reqw.data(), (int)reqw.size(), 1, 2));
   
    bvector data = { 'h', 'a', 'h', 'a', 'h', 'a', '1' };

    reqw = proto.create_cmd_request(3, 3000, &data);
    const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_1_RAW, proto.get_cmd_shema());
    TEST_CHECK(cm3.is_match(reqw.data(), (int)reqw.size(), 1, 3));

    reqw = proto.create_enum_request(4000);
    const cmd_schema & cm4 = cmd_schema::get_schema(PROTO_1_ENUM, proto.get_cmd_shema());
    TEST_CHECK(cm4.is_match(reqw.data(), (int)reqw.size(), 1, 0));
}

static void test_request_proto2()
{
    TEST_MSG("?test_request_proto2?");
    uint32_t err;
    Protocol2 proto(&err, false);
    TEST_CHECK(proto.create_version_request(0).size() == 0);
  
    bvector reqw = proto.create_open_request(1, 1000);

    const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_2_OPEN, proto.get_cmd_shema());
    TEST_CHECK(cm1.is_match(reqw.data(), (int)reqw.size(), 2, 1));
    reqw = proto.create_close_request(2, 2000);

    const cmd_schema & cm2 = cmd_schema::get_schema(PROTO_2_CLOSE, proto.get_cmd_shema());
    TEST_CHECK(cm2.is_match(reqw.data(), (int)reqw.size(), 2, 2));
    
    bvector data = { 'h', 'a', 'h', 'a', 'h', 'a', '2' };
    
    reqw = proto.create_cmd_request(3, 3000, &data);
    const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_2_CMD, proto.get_cmd_shema());
    TEST_CHECK(cm3.is_match(reqw.data(), (int)reqw.size(), 2, 3));
        
    TEST_CHECK(proto.create_enum_request(4000).size() == 0);
}

static void test_request_proto3()
{
    TEST_MSG("?test_request_proto3?");
    uint32_t err;
    Protocol3 proto(&err, false);
    bvector reqw = proto.create_version_request(0);
    const cmd_schema & cm0 = cmd_schema::get_schema(PROTO_3_VER, proto.get_cmd_shema());
    TEST_CHECK(cm0.is_match(reqw.data(), (int)reqw.size(), 3, 0));
    
    reqw = proto.create_open_request(DevId(1), 1000);
    const cmd_schema & cm1 = cmd_schema::get_schema(PROTO_3_OPEN, proto.get_cmd_shema());
    TEST_CHECK(cm1.is_match(reqw.data(), (int)reqw.size(), 3, 1));
    
    reqw = proto.create_close_request(DevId(2), 2000);
    const cmd_schema & cm2 = cmd_schema::get_schema(PROTO_3_CLOSE, proto.get_cmd_shema());
    TEST_CHECK(cm2.is_match(reqw.data(), (int)reqw.size(), 3, 2));
  
    bvector data = { 'h', 'a', 'h', 'a', 'h', 'a', '3' };

    reqw = proto.create_cmd_request(DevId(3), 3000, &data);
    const cmd_schema & cm3 = cmd_schema::get_schema(PROTO_3_CMD, proto.get_cmd_shema());
    TEST_CHECK(cm3.is_match(reqw.data(), (int)reqw.size(), 3, 3));
   
    reqw = proto.create_enum_request(4000);
    const cmd_schema & cm4 = cmd_schema::get_schema(PROTO_3_ENUM, proto.get_cmd_shema());
    TEST_CHECK(cm4.is_match(reqw.data(), (int)reqw.size(), 3, 0));
 }

void  test_protocols()
{
    acutest_verbose_level_ = 3;
    TEST_MSG("test_protocols...");
    test_request_proto1();
    test_request_proto2();
    test_request_proto3();
}

extern uint32_t xibridge_parse_uri_dev12(const char *uri, 
                                         xibridge_parsed_uri *parsed_uri);

void test_xibridge_uri_parse()
{
    xibridge_parsed_uri parsed;
    TEST_MSG("Starting test_xibridge_uri_parse...");
    TEST_CHECK(xibridge_parse_uri_dev12("xi-net://abcd/1", &parsed) == 0);
    TEST_CHECK(parsed.uri_device_id.id == 1);
    TEST_CHECK(parsed.uri_device_id.VID == 0 && parsed.uri_device_id.PID == 0 && parsed.uri_device_id.reserve == 0);
    TEST_CHECK(xibridge_parse_uri_dev12("xi-net://0.0.0.0/000000df00DF00dFAAAAAAAA", &parsed) == 0);
    TEST_CHECK(parsed.uri_device_id.id == 0xAAAAAAAA);
    TEST_CHECK(parsed.uri_device_id.VID == 0xDF);
    TEST_CHECK(parsed.uri_device_id.PID == 0xDF);
    TEST_CHECK(parsed.uri_device_id.reserve == 0xDF);
}

/*
 * server simulator must be started to do this test
*/

void test_server_ximc()
{
    xibridge_set_base_protocol_version({ 1, 0, 0 });
    char _DEV_IP[64];
    const char *s_ip = "127.0.0.1";
    const uint32_t dev_num = 3;

    sprintf(_DEV_IP, "xi-net://%s/%x", s_ip, dev_num);
    char  *pdata; uint32_t count;

    uint32_t err = xibridge_enumerate_adapter_devices(s_ip, "", &pdata, &count);
    TEST_CHECK(err == 0);
    TEST_MSG("Count of enumerated devices: %u", count);
    if (count)
    {
        const char *p = pdata;
        for (int i = 0; i < (int)count; i++)
        {
            TEST_MSG("Enumerated device #%d: URI: %s", i + 1, p);
            p = strchr(p, 0) + 1;
        }
    }
    xibridge_free_enumerate_devices(pdata);

    if (err) return;

    xibridge_conn_t conn;

    err = xibridge_open_device_connection(_DEV_IP, &conn);
    TEST_CHECK(err == 0);
    if (err)
    {
        xibridge_close_device_connection(&conn);
        return;
    }
   

    if (err) return;

    re_geng settings;
    err = xibridge_device_request_response(&conn, (const uint8_t *)"geng", 4, (uint8_t *)&settings, sizeof(re_geng));
    TEST_CHECK(err == 0);

    err = xibridge_close_device_connection(&conn);
    TEST_CHECK(err == 0);
}

void test_server_urpc()
{
    xibridge_set_base_protocol_version({ 2, 0, 0 });
    char _DEV_IP[64];
    const char *s_ip = "127.0.0.1";
    const uint32_t dev_num = 7;
    
    sprintf(_DEV_IP, "xi-net://%s/%x", s_ip, dev_num);
    xibridge_conn_t conn;
    uint32_t err = xibridge_open_device_connection(_DEV_IP, &conn);
    TEST_CHECK(err == 0);

    if (err) return;

    unsigned char resp[72 + 4];

    err = xibridge_device_request_response(&conn, (const unsigned char *)"ginf", 4, resp, 72 + 4);
    
    TEST_CHECK(err == 0);
    
    uint32_t urpc_res = (uint32_t)*resp;

    TEST_MSG("Urpc return code: %d\n", (int)(urpc_res >> 24));
      
    err = xibridge_device_request_response(&conn, (const unsigned char *)"gets", 4, resp, 48 + 4);
    
    TEST_CHECK(err == 0);

    err = xibridge_close_device_connection(&conn);

    TEST_CHECK(err == 0);
}

void test_server_xibridge()
{
    xibridge_set_base_protocol_version({ 3, 0, 0 });
    char _DEV_IP[64];
    const char *s_ip = "127.0.0.1";
    const uint32_t dev_num = 12;

    sprintf(_DEV_IP, "xi-net://%s/%x", s_ip, dev_num);
    char  *pdata; uint32_t count;

    uint32_t err = xibridge_enumerate_adapter_devices(s_ip, "", &pdata, &count);
    TEST_CHECK(err == 0);
    TEST_MSG("Count of enumerated devices: %u", count);
    if (count)
    {
        const char *p = pdata;
        for (int i = 0; i < (int)count; i++)
        {
            TEST_MSG("Enumerated device #%d: URI: %s", i + 1, p);
            p = strchr(p, 0) + 1;
        }
    }
    xibridge_free_enumerate_devices(pdata);

    if (err) return;

    xibridge_conn_t conn;

    err = xibridge_open_device_connection(_DEV_IP, &conn);
    TEST_CHECK(err == 0);
    if (err)
    {
        xibridge_close_device_connection(&conn);
        return;
    }


    if (err) return;
    /*
    re_geng settings;
    err = xibridge_device_request_response(&conn, (const uint8_t *)"geng", 4, (uint8_t *)&settings, sizeof(re_geng));
    TEST_CHECK(err == 0);
    */
    err = xibridge_close_device_connection(&conn);
    TEST_CHECK(err == 0);
}

void test_main()
{
    TEST_MSG("Starting test_main...");
    test_protocols();
    test_xibridge_uri_parse();
    TEST_MSG("Then, starting server-simulator...");
    // server_simu should be already started
    // test_server_ximc();
    // test_server_urpc();
    // test_server_xibridge();
}

TEST_LIST = {
    { "test", test_main },
    { NULL, NULL }     /* zeroed record marking the end of the list */
};
