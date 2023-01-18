/*
 * Test support data structures for urpc-xinet
*/
#ifndef URMC_MIN_H
#define URMC_MIN_H

typedef struct
{
    uint8_t Manufacturer[16]; // Manufacturer name. The name is set by the manufacturer.
    uint8_t ProductName[16]; // Product name. The name is set by the manufacturer.
    uint8_t ControllerName[16]; // User controller name. This name can be set by user via additional command.
    uint8_t HardwareMajor; // The major number of the hardware version
    uint8_t HardwareMinor; // Minor number of the hardware version
    uint16_t HardwareBugfix; // Number of edits for this release of hardware
    uint8_t BootloaderMajor; // Bootloader major version number
    uint8_t BootloaderMinor; // Bootloader minor version number 
    uint16_t BootloaderBugfix; // Bootloader release version number 
    uint8_t FirmwareMajor; // Firmware major version number
    uint8_t FirmwareMinor; // Firmware minor version number
    uint16_t FirmwareBugfix; // Firmware release version number
    uint32_t SerialNumber; // Device serial number
    uint8_t reserved[8]; 
} urmc_get_identity_information_t;

typedef struct
{
    uint8_t MoveSts; // Move state
    uint8_t MvCmdSts; // Move command state
    uint8_t PWRSts; // Power state of the stepper motor (used only with stepper motor)
    uint8_t EncSts; // Encoder state. 
    uint8_t reserved0[1]; 
    int32_t CurPosition; // Current position
    int16_t uCurPosition; // Step motor shaft position in 1/256 microsteps. Used only with stepper motor.
    int64_t EncPosition; // Current encoder position. 
    int32_t CurSpeed; // Motor shaft speed.
    int16_t uCurSpeed; // Part of motor shaft speed in 1/256 microsteps. Used only with stepper motor.
    uint8_t reserved1[2]; 
    int16_t Upwr; // Power supply voltage, tens of mV.
    uint8_t reserved2[6]; 
    uint32_t Flags; //Set of flags specify motor shaft movement algorithm and list of limitations
    uint32_t GPIOFlags; //Set of flags of gpio states
} urmc_status_impl_t;
#endif
