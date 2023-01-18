/*
* Test support data structures for ximc-xinet
*/

#ifndef XIMC_MIN_H
#define XIMC_MIN_H

// from ximc.h
typedef struct
{
    unsigned int MoveSts;   // flagset_movestate "Flags of move state"
    unsigned int MvCmdSts;  // flagset_mvcmdstatus "Move command state"
    unsigned int PWRSts;    // flagset_powerstate "Flags of power state of stepper motor"
    unsigned int EncSts;    // flagset_encodestatus "Encoder state"
    unsigned int WindSts;   // flagset_windstatus "Winding state"
    int CurPosition;    // Current position
    int uCurPosition;   // Step motor shaft position in microsteps
    long EncPosition;   // Current encoder position
    int CurSpeed;   // Motor shaft speed in steps/s or rpm
    int uCurSpeed;  // Part of motor shaft speed in microsteps
    int Ipwr;   // Engine current, mA. 
    int Upwr;   // Power supply voltage, tens of mV.
    int Iusb;   // USB current, mA.
    int Uusb;   // USB voltage, tens of mV.
    int CurT;   // Temperature in tenths of degrees C. 
    unsigned int Flags; // flagset_stateflags "Status flags"
    unsigned int GPIOFlags; // flagset_gpioflags "Status flags of the GPIO outputs"
    unsigned int CmdBufFreeSpace;   // This field is a service field. It shows the amount of free cells buffer synchronization chain.
} status_t;

typedef struct
{
    unsigned int NomVoltage;    // Rated voltage in tens of mV. Controller will keep the voltage drop on motor below this value if ENGINE_LIMIT_VOLT flag is set (used with DC only). 
    unsigned int NomCurrent;    // Rated current (in mA). Controller will keep current consumed by motor below this value if ENGINE_LIMIT_CURR flag is set. Range: 15..8000.
    unsigned int NomSpeed;  // Nominal (maximum) speed (in whole steps/s or rpm for DC and stepper motor as a master encoder). Controller will keep motor shaft RPM below this value if ENGINE_LIMIT_RPM flag is set. Range: 1..100000.
    unsigned int uNomSpeed; // The fractional part of a nominal speed in microsteps (is only used with stepper motor). Microstep size and the range of valid values for this field depend on selected step division mode (see MicrostepMode field in engine_settings).
    unsigned int EngineFlags;   // flagset_engineflags "Flags of engine settings"
    int Antiplay;   // Number of pulses or steps for backlash (play) compensation procedure. Used if ENGINE_ANTIPLAY flag is set.
    unsigned int MicrostepMode; // flagset_microstepmode "Flags of microstep mode"
    unsigned int StepsPerRev;   // Number of full steps per revolution(Used with stepper motor only). Range: 1..65535.
} engine_settings_t;

#endif
