/*
 * Test support data structures for urpc-xinet
*/
#ifndef URMC_MIN_H
#define URMC_MIN_H
 typedef struct
    {
        uint8_t Manufacturer[16]; /**< \~english Manufacturer name. The name is set by the manufacturer. \~russian ��� �������������. ��������������� ��������������. */
        uint8_t ProductName[16]; /**< \~english Product name. The name is set by the manufacturer. \~russian �������� ��������. ��������������� ��������������. */
        uint8_t ControllerName[16]; /**< \~english User controller name. This name can be set by user via additional command. \~russian ���������������� ��� �����������. ����� ���� ����������� ������������� � ������� ��������� �������. */
        uint8_t HardwareMajor; /**< \~english The major number of the hardware version. \~russian �������� ����� ������ ������. */
        uint8_t HardwareMinor; /**< \~english Minor number of the hardware version. \~russian �������������� ����� ������ ������. */
        uint16_t HardwareBugfix; /**< \~english Number of edits for this release of hardware. \~russian ����� ������ ���� ������ ������. */
        uint8_t BootloaderMajor; /**< \~english Bootloader major version number. \~russian �������� ����� ������ ����������. */
        uint8_t BootloaderMinor; /**< \~english Bootloader minor version number. \~russian �������� ����� ������ ����������. */
        uint16_t BootloaderBugfix; /**< \~english Bootloader release version number. \~russian ����� ������ ������ ����������. */
        uint8_t FirmwareMajor; /**< \~english Firmware major version number. \~russian �������� ����� ������ ��������. */
        uint8_t FirmwareMinor; /**< \~english Firmware minor version number. \~russian �������� ����� ������ ��������. */
        uint16_t FirmwareBugfix; /**< \~english Firmware release version number. \~russian ����� ������ ������ ��������. */
        uint32_t SerialNumber; /**< \~english Device serial number. \~russian �������� ����� �������. */
        uint8_t reserved[8]; /**< \~english  \~russian  */

    } urmc_get_identity_information_t;

    typedef struct
    {
        int32_t Position; /**< \~english Desired position (whole steps). \~russian �������� ������� (����� �����). */
        int16_t uPosition; /**< \~english The fractional part of a position in microsteps. Is only used with stepper motor. Range: -255..255. \~russian ������� ����� ������� � ����������. ������������ ������ � ������� ����������. ��������: -255..255. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_command_move_t;

    typedef struct
    {
        int32_t DeltaPosition; /**< \~english Shift (delta) of position \~russian �������� (������) ������� */
        int16_t uDeltaPosition; /**< \~english Fractional part of the shift in micro steps is only used with stepper motor. Range: -255..255. \~russian ������� ����� �������� � ����������, ������������ ������ � ������� ����������. ��������: -255..255. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_command_movr_t;

    typedef struct
    {
        int32_t Position; /**< \~english The position of the whole steps in the engine \~russian ������� � �������� ����� ��������� */
        int16_t uPosition; /**< \~english Microstep position is only used with stepper motors \~russian ������� � ����������(������������ ������ � �������� �����������). */
        int64_t EncPosition; /**< \~english Encoder position. \~russian ������� ��������. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_get_position_t;

    typedef struct
    {
        int32_t Position; /**< \~english The position of the whole steps in the engine \~russian ������� � �������� ����� ��������� */
        int16_t uPosition; /**< \~english Microstep position is only used with stepper motors \~russian ������� � ����������(������������ ������ � �������� �����������). */
        int64_t EncPosition; /**< \~english Encoder position. \~russian ������� ��������. */
        uint8_t PosFlags; /**< \~english Flags \~russian ����� */
        uint8_t reserved[5]; /**< \~english  \~russian  */

    } urmc_set_position_t;

    typedef struct
    {
        uint8_t MoveSts; /**< \~english Move state. \~russian ��������� ��������. */
        uint8_t MvCmdSts; /**< \~english Move command state. \~russian ��������� ������� �������� (�������� command_move, command_movr, command_left, command_right, command_stop, command_home, command_loft). */
        uint8_t PWRSts; /**< \~english Power state of the stepper motor (used only with stepper motor). \~russian ��������� ������� �������� ��������� (������������ ������ � ������� ����������). */
        uint8_t EncSts; /**< \~english Encoder state. \~russian ��������� ��������. */
        uint8_t reserved0[1]; /**< \~english  \~russian  */
        int32_t CurPosition; /**< \~english Current position. \~russian ��������� ����, � ������� �������� ������� �������, ��� �� �� ���� �������� �������� �����. � ������ ������ � DC-������� � ���� ���� ��������� ������� ������� �� ������ � ��������, � ������ ������ � ��-������� � ������, ����� ���������� �������� ��������, ���������� �� �����, � ���� ���� ���������� ����� �������� ����� ������� �������. */
        int16_t uCurPosition; /**< \~english Step motor shaft position in 1/256 microsteps. Used only with stepper motor. \~russian ������� ����� ������� ������� � ���������� (-255..255). ������������ ������ � ������� ����������. */
        int64_t EncPosition; /**< \~english Current encoder position. \~russian ������� ������� �� ������ � �������� � ��������� ��������, ������������ ������ ���� ������� ����������, ������������� � �� �������� �������� �������� ���������, �������� ��� ������������� �������� ��������� � ������� ���������� ��� �������� ���������������. */
        int32_t CurSpeed; /**< \~english Motor shaft speed. \~russian ������� ��������. */
        int16_t uCurSpeed; /**< \~english Part of motor shaft speed in 1/256 microsteps. Used only with stepper motor. \~russian ������� ����� ������� �������� � ���������� (-255..255). ������������ ������ � ������� ����������. */
        uint8_t reserved1[2]; /**< \~english  \~russian  */
        int16_t Upwr; /**< \~english Power supply voltage, tens of mV. \~russian ���������� �� ������� �����, ������� ��. */
        uint8_t reserved2[6]; /**< \~english  \~russian  */
        uint32_t Flags; /**< \~english Set of flags specify motor shaft movement algorithm and list of limitations. \~russian ����� ���������. */
        uint32_t GPIOFlags; /**< \~english Set of flags of gpio states \~russian ����� ��������� GPIO ������. */
        uint8_t reserved[5]; /**< \~english  \~russian  */

    } urmc_status_impl_t;
   
#endif