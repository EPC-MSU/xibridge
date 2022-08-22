/*
* Test support data structures for ximc-xinet
*/

#ifndef XIMC_MIN_H
#define XIMC_MIN_H

// from ximc.h
typedef struct
{
    float Speed;    /**< \english Target speed. \endenglish \russian Заданная скорость. \endrussian */
    float Accel;    /**< \english Motor shaft acceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian Ускорение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). \endrussian */
    float Decel;    /**< \english Motor shaft deceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian Торможение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). \endrussian */
    float AntiplaySpeed;    /**< \english Speed in antiplay mode. \endenglish \russian Скорость в режиме антилюфта. \endrussian */
    unsigned int MoveFlags; /**< \english \ref flagset_moveflags "Flags of the motion parameters". \endenglish \russian \ref flagset_moveflags "Флаги параметров движения". \endrussian */
} move_settings_calb_t;


typedef struct
{
    unsigned int MoveSts;   /**< \english \ref flagset_movestate "Flags of move state". \endenglish \russian \ref flagset_movestate "Флаги состояния движения". \endrussian */
    unsigned int MvCmdSts;  /**< \english \ref flagset_mvcmdstatus "Move command state". \endenglish \russian \ref flagset_mvcmdstatus "Состояние команды движения". \endrussian */
    unsigned int PWRSts;    /**< \english \ref flagset_powerstate "Flags of power state of stepper motor". \endenglish \russian \ref flagset_powerstate "Флаги состояния питания шагового мотора". \endrussian */
    unsigned int EncSts;    /**< \english \ref flagset_encodestatus "Encoder state". \endenglish \russian \ref flagset_encodestatus "Состояние энкодера". \endrussian */
    unsigned int WindSts;   /**< \english \ref flagset_windstatus "Winding state". \endenglish \russian \ref flagset_windstatus "Состояние обмоток". \endrussian */
    int CurPosition;    /**< \english Current position. \endenglish \russian Первичное поле, в котором хранится текущая позиция, как бы ни была устроена обратная связь. В случае работы с DC-мотором в этом поле находится текущая позиция по данным с энкодера, в случае работы с ШД-мотором в режиме, когда первичными являются импульсы, подаваемые на мотор, в этом поле содержится целое значение шагов текущей позиции. \endrussian */
    int uCurPosition;   /**< \english Step motor shaft position in microsteps. Microstep size and the range of valid values for this field depend on selected step division mode (see MicrostepMode field in engine_settings). Used only with stepper motor. \endenglish \russian Дробная часть текущей позиции в микрошагах. Величина микрошага и диапазон допустимых значений для данного поля зависят от выбранного режима деления шага (см. поле MicrostepMode в engine_settings). Используется только с шаговым двигателем. \endrussian */
    long EncPosition;   /**< \english Current encoder position. \endenglish \russian Текущая позиция по данным с энкодера в импульсах энкодера, используется только если энкодер установлен, активизирован и не является основным датчиком положения, например при использовании энкодера совместно с шаговым двигателем для контроля проскальзования. \endrussian */
    int CurSpeed;   /**< \english Motor shaft speed in steps/s or rpm. \endenglish \russian Текущая скорость. \endrussian */
    int uCurSpeed;  /**< \english Part of motor shaft speed in microsteps. Microstep size and the range of valid values for this field depend on selected step division mode (see MicrostepMode field in engine_settings). Used only with stepper motor. \endenglish \russian Дробная часть текущей скорости в микрошагах. Величина микрошага и диапазон допустимых значений для данного поля зависят от выбранного режима деления шага (см. поле MicrostepMode в engine_settings). Используется только с шаговым двигателем. \endrussian */
    int Ipwr;   /**< \english Engine current, mA. \endenglish \russian Ток потребления силовой части, мА. \endrussian */
    int Upwr;   /**< \english Power supply voltage, tens of mV. \endenglish \russian Напряжение на силовой части, десятки мВ. \endrussian */
    int Iusb;   /**< \english USB current, mA. \endenglish \russian Ток потребления по USB, мА. \endrussian */
    int Uusb;   /**< \english USB voltage, tens of mV. \endenglish \russian Напряжение на USB, десятки мВ. \endrussian */
    int CurT;   /**< \english Temperature in tenths of degrees C. \endenglish \russian Температура процессора в десятых долях градусов цельсия. \endrussian */
    unsigned int Flags; /**< \english \ref flagset_stateflags "Status flags". \endenglish \russian \ref flagset_stateflags "Флаги состояния". \endrussian */
    unsigned int GPIOFlags; /**< \english \ref flagset_gpioflags "Status flags of the GPIO outputs". \endenglish \russian \ref flagset_gpioflags "Флаги состояния GPIO входов". \endrussian */
    unsigned int CmdBufFreeSpace;   /**< \english This field is a service field. It shows the amount of free cells buffer synchronization chain. \endenglish \russian Данное поле служебное. Оно показывает количество свободных ячеек буфера цепочки синхронизации. \endrussian */
} status_t;

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
#endif