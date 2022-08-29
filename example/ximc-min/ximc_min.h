/*
* Test support data structures for ximc-xinet
*/

#ifndef XIMC_MIN_H
#define XIMC_MIN_H

// from ximc.h
typedef struct
{
    float Speed;    /**< \english Target speed. \endenglish \russian �������� ��������. \endrussian */
    float Accel;    /**< \english Motor shaft acceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian ���������, �������� � ����� � �������^2(��) ��� � �������� � ������ �� �������(DC). \endrussian */
    float Decel;    /**< \english Motor shaft deceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian ����������, �������� � ����� � �������^2(��) ��� � �������� � ������ �� �������(DC). \endrussian */
    float AntiplaySpeed;    /**< \english Speed in antiplay mode. \endenglish \russian �������� � ������ ���������. \endrussian */
    unsigned int MoveFlags; /**< \english \ref flagset_moveflags "Flags of the motion parameters". \endenglish \russian \ref flagset_moveflags "����� ���������� ��������". \endrussian */
} move_settings_calb_t;


typedef struct
{
    unsigned int MoveSts;   /**< \english \ref flagset_movestate "Flags of move state". \endenglish \russian \ref flagset_movestate "����� ��������� ��������". \endrussian */
    unsigned int MvCmdSts;  /**< \english \ref flagset_mvcmdstatus "Move command state". \endenglish \russian \ref flagset_mvcmdstatus "��������� ������� ��������". \endrussian */
    unsigned int PWRSts;    /**< \english \ref flagset_powerstate "Flags of power state of stepper motor". \endenglish \russian \ref flagset_powerstate "����� ��������� ������� �������� ������". \endrussian */
    unsigned int EncSts;    /**< \english \ref flagset_encodestatus "Encoder state". \endenglish \russian \ref flagset_encodestatus "��������� ��������". \endrussian */
    unsigned int WindSts;   /**< \english \ref flagset_windstatus "Winding state". \endenglish \russian \ref flagset_windstatus "��������� �������". \endrussian */
    int CurPosition;    /**< \english Current position. \endenglish \russian ��������� ����, � ������� �������� ������� �������, ��� �� �� ���� �������� �������� �����. � ������ ������ � DC-������� � ���� ���� ��������� ������� ������� �� ������ � ��������, � ������ ������ � ��-������� � ������, ����� ���������� �������� ��������, ���������� �� �����, � ���� ���� ���������� ����� �������� ����� ������� �������. \endrussian */
    int uCurPosition;   /**< \english Step motor shaft position in microsteps. Microstep size and the range of valid values for this field depend on selected step division mode (see MicrostepMode field in engine_settings). Used only with stepper motor. \endenglish \russian ������� ����� ������� ������� � ����������. �������� ��������� � �������� ���������� �������� ��� ������� ���� ������� �� ���������� ������ ������� ���� (��. ���� MicrostepMode � engine_settings). ������������ ������ � ������� ����������. \endrussian */
    long EncPosition;   /**< \english Current encoder position. \endenglish \russian ������� ������� �� ������ � �������� � ��������� ��������, ������������ ������ ���� ������� ����������, ������������� � �� �������� �������� �������� ���������, �������� ��� ������������� �������� ��������� � ������� ���������� ��� �������� ���������������. \endrussian */
    int CurSpeed;   /**< \english Motor shaft speed in steps/s or rpm. \endenglish \russian ������� ��������. \endrussian */
    int uCurSpeed;  /**< \english Part of motor shaft speed in microsteps. Microstep size and the range of valid values for this field depend on selected step division mode (see MicrostepMode field in engine_settings). Used only with stepper motor. \endenglish \russian ������� ����� ������� �������� � ����������. �������� ��������� � �������� ���������� �������� ��� ������� ���� ������� �� ���������� ������ ������� ���� (��. ���� MicrostepMode � engine_settings). ������������ ������ � ������� ����������. \endrussian */
    int Ipwr;   /**< \english Engine current, mA. \endenglish \russian ��� ����������� ������� �����, ��. \endrussian */
    int Upwr;   /**< \english Power supply voltage, tens of mV. \endenglish \russian ���������� �� ������� �����, ������� ��. \endrussian */
    int Iusb;   /**< \english USB current, mA. \endenglish \russian ��� ����������� �� USB, ��. \endrussian */
    int Uusb;   /**< \english USB voltage, tens of mV. \endenglish \russian ���������� �� USB, ������� ��. \endrussian */
    int CurT;   /**< \english Temperature in tenths of degrees C. \endenglish \russian ����������� ���������� � ������� ����� �������� �������. \endrussian */
    unsigned int Flags; /**< \english \ref flagset_stateflags "Status flags". \endenglish \russian \ref flagset_stateflags "����� ���������". \endrussian */
    unsigned int GPIOFlags; /**< \english \ref flagset_gpioflags "Status flags of the GPIO outputs". \endenglish \russian \ref flagset_gpioflags "����� ��������� GPIO ������". \endrussian */
    unsigned int CmdBufFreeSpace;   /**< \english This field is a service field. It shows the amount of free cells buffer synchronization chain. \endenglish \russian ������ ���� ���������. ��� ���������� ���������� ��������� ����� ������ ������� �������������. \endrussian */
} status_t;

typedef struct
{
    unsigned int NomVoltage;    /**< \english Rated voltage in tens of mV. Controller will keep the voltage drop on motor below this value if ENGINE_LIMIT_VOLT flag is set (used with DC only). \endenglish \russian ����������� ���������� ������ � �������� ��. ���������� ����� ��������� ���������� �� ������ �� ���� ������������, ���� ���������� ���� ENGINE_LIMIT_VOLT (������������ ������ � DC ����������). \endrussian */
    unsigned int NomCurrent;    /**< \english Rated current (in mA). Controller will keep current consumed by motor below this value if ENGINE_LIMIT_CURR flag is set. Range: 15..8000 \endenglish \russian ����������� ��� ����� ����� (� ��). ��� ��������������� ��� ������� � ����� ���� ��������� ��� DC(���� ���������� ���� ENGINE_LIMIT_CURR). ��������: 15..8000 \endrussian */
    unsigned int NomSpeed;  /**< \english Nominal (maximum) speed (in whole steps/s or rpm for DC and stepper motor as a master encoder). Controller will keep motor shaft RPM below this value if ENGINE_LIMIT_RPM flag is set. Range: 1..100000. \endenglish \russian ����������� (������������) �������� (� ����� �����/� ��� rpm ��� DC � �������� ��������� � ������ �������� ��������). ���������� ����� ��������� �������� ������ �� ���� �����������, ���� ���������� ���� ENGINE_LIMIT_RPM. ��������: 1..100000. \endrussian */
    unsigned int uNomSpeed; /**< \english The fractional part of a nominal speed in microsteps (is only used with stepper motor). Microstep size and the range of valid values for this field depend on selected step division mode (see MicrostepMode field in engine_settings). \endenglish \russian ������������ ����� ����������� �������� ������ (������������ ������ � ������� ����������). �������� ��������� � �������� ���������� �������� ��� ������� ���� ������� �� ���������� ������ ������� ���� (��. ���� MicrostepMode � engine_settings). \endrussian */
    unsigned int EngineFlags;   /**< \english \ref flagset_engineflags "Flags of engine settings". \endenglish \russian \ref flagset_engineflags "����� ���������� ������". \endrussian */
    int Antiplay;   /**< \english Number of pulses or steps for backlash (play) compensation procedure. Used if ENGINE_ANTIPLAY flag is set. \endenglish \russian ���������� ����� ��������� ��� ��������� ��������, �� ������� ���������� ����� ��������� �� �������� ������� ��� ������� � ��� � ����� � ��� �� �������. ������������, ���� ���������� ���� ENGINE_ANTIPLAY. \endrussian */
    unsigned int MicrostepMode; /**< \english \ref flagset_microstepmode "Flags of microstep mode". \endenglish \russian \ref flagset_microstepmode "����� ���������� ������������� ������". \endrussian */
    unsigned int StepsPerRev;   /**< \english Number of full steps per revolution(Used with stepper motor only). Range: 1..65535. \endenglish \russian ���������� ������ ����� �� ������(������������ ������ � ������� ����������). ��������: 1..65535. \endrussian */
} engine_settings_t;
#endif