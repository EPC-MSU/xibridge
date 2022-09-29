/*
 * Test support data structures for urpc-xinet
*/
#ifndef URMC_MIN_H
#define URMC_MIN_H
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

    typedef struct
    {
        int32_t Position; /**< \~english Desired position (whole steps). \~russian Желаемая позиция (целая часть). */
        int16_t uPosition; /**< \~english The fractional part of a position in microsteps. Is only used with stepper motor. Range: -255..255. \~russian Дробная часть позиции в микрошагах. Используется только с шаговым двигателем. Диапазон: -255..255. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_command_move_t;

    typedef struct
    {
        int32_t DeltaPosition; /**< \~english Shift (delta) of position \~russian Смещение (дельта) позиции */
        int16_t uDeltaPosition; /**< \~english Fractional part of the shift in micro steps is only used with stepper motor. Range: -255..255. \~russian Дробная часть смещения в микрошагах, используется только с шаговым двигателем. Диапазон: -255..255. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_command_movr_t;

    typedef struct
    {
        int32_t Position; /**< \~english The position of the whole steps in the engine \~russian Позиция в основных шагах двигателя */
        int16_t uPosition; /**< \~english Microstep position is only used with stepper motors \~russian Позиция в микрошагах(используется только с шаговыми двигателями). */
        int64_t EncPosition; /**< \~english Encoder position. \~russian Позиция энкодера. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_get_position_t;

    typedef struct
    {
        int32_t Position; /**< \~english The position of the whole steps in the engine \~russian Позиция в основных шагах двигателя */
        int16_t uPosition; /**< \~english Microstep position is only used with stepper motors \~russian Позиция в микрошагах(используется только с шаговыми двигателями). */
        int64_t EncPosition; /**< \~english Encoder position. \~russian Позиция энкодера. */
        uint8_t PosFlags; /**< \~english Flags \~russian Флаги */
        uint8_t reserved[5]; /**< \~english  \~russian  */

    } urmc_set_position_t;

    typedef struct
    {
        uint8_t MoveSts; /**< \~english Move state. \~russian Состояние движения. */
        uint8_t MvCmdSts; /**< \~english Move command state. \~russian Состояние команды движения (касается command_move, command_movr, command_left, command_right, command_stop, command_home, command_loft). */
        uint8_t PWRSts; /**< \~english Power state of the stepper motor (used only with stepper motor). \~russian Состояние питания шагового двигателя (используется только с шаговым двигателем). */
        uint8_t EncSts; /**< \~english Encoder state. \~russian Состояние энкодера. */
        uint8_t reserved0[1]; /**< \~english  \~russian  */
        int32_t CurPosition; /**< \~english Current position. \~russian Первичное поле, в котором хранится текущая позиция, как бы ни была устроена обратная связь. В случае работы с DC-мотором в этом поле находится текущая позиция по данным с энкодера, в случае работы с ШД-мотором в режиме, когда первичными являются импульсы, подаваемые на мотор, в этом поле содержится целое значение шагов текущей позиции. */
        int16_t uCurPosition; /**< \~english Step motor shaft position in 1/256 microsteps. Used only with stepper motor. \~russian Дробная часть текущей позиции в микрошагах (-255..255). Используется только с шаговым двигателем. */
        int64_t EncPosition; /**< \~english Current encoder position. \~russian Текущая позиция по данным с энкодера в импульсах энкодера, используется только если энкодер установлен, активизирован и не является основным датчиком положения, например при использовании энкодера совместно с шаговым двигателем для контроля проскальзования. */
        int32_t CurSpeed; /**< \~english Motor shaft speed. \~russian Текущая скорость. */
        int16_t uCurSpeed; /**< \~english Part of motor shaft speed in 1/256 microsteps. Used only with stepper motor. \~russian Дробная часть текущей скорости в микрошагах (-255..255). Используется только с шаговым двигателем. */
        uint8_t reserved1[2]; /**< \~english  \~russian  */
        int16_t Upwr; /**< \~english Power supply voltage, tens of mV. \~russian Напряжение на силовой части, десятки мВ. */
        uint8_t reserved2[6]; /**< \~english  \~russian  */
        uint32_t Flags; /**< \~english Set of flags specify motor shaft movement algorithm and list of limitations. \~russian Флаги состояний. */
        uint32_t GPIOFlags; /**< \~english Set of flags of gpio states \~russian Флаги состояний GPIO входов. */
        uint8_t reserved[5]; /**< \~english  \~russian  */

    } urmc_status_impl_t;
   
#endif