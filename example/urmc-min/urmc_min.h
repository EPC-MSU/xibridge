/*
 * Test support data structures for urpc-xinet
*/
#ifndef URMC_MIN_H
#define URMC_MIN_H
#define URMC_BUILDER_VERSION_MAJOR 0
#define URMC_BUILDER_VERSION_MINOR 10
#define URMC_BUILDER_VERSION_BUGFIX 13
#define URMC_BUILDER_VERSION_SUFFIX ""
#define URMC_BUILDER_VERSION "0.10.13"


#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <wchar.h>


#undef URMC_URPC_API_EXPORT
#if defined(_WIN32)
#if URMC_URPC_BUILDING_STAGE
#define URMC_URPC_API_EXPORT __declspec(dllexport)
#else
#define URMC_URPC_API_EXPORT __declspec(dllimport)
#endif
#else
#define URMC_URPC_API_EXPORT __attribute__((visibility("default")))
#endif

#undef URMC_URPC_CALLING_CONVENTION
#if defined(_WIN32)
#define URMC_URPC_CALLING_CONVENTION __cdecl
#else
#define URMC_URPC_CALLING_CONVENTION
#endif

    typedef int device_t;
#define device_undefined (-1)
    typedef int result_t;
#define result_ok 0
#define result_error (-1)
#define result_not_implemented (-2)
#define result_value_error (-3)
#define result_nodevice (-4)
    /**
    * \~english
    * @name Logging level
    * \~russian
    * @name Уровень логирования
    */
    //@{
    /**
    * \~english
    * Logging level - error
    * \~russian
    * Уровень логирования - ошибка
    */
#define LOGLEVEL_ERROR      0x01
    /**
    * \~english
    * Logging level - warning
    * \~russian
    * Уровень логирования - предупреждение
    */
#define LOGLEVEL_WARNING    0x02
    /**
    * \~english
    * Logging level - info
    * \~russian
    * Уровень логирования - информация
    */
#define LOGLEVEL_INFO       0x03
    /**
    * \~english
    * Logging level - debug
    * \~russian
    * Уровень логирования - отладка
    */
#define LOGLEVEL_DEBUG      0x04
    //@}

    /**
    * \~english
    * Logging callback prototype.
    * @param loglevel - A logging level.
    * @param message - A message.
    * \~russian
    * Прототип функции обратного вызова для логирования.
    * @param loglevel - Уровень логирования.
    * @param message - Сообщение.
    */
    typedef void (URMC_URPC_CALLING_CONVENTION *urmc_logging_callback_t)(int loglevel, const wchar_t *message, void *user_data);

    /**
    * \~english
    * Simple callback for logging to stderr in wide chars.
    * @param loglevel - A logging level.
    * @param message - A message.
    * \~russian
    * Простая функция логирования на stderr в широких символах.
    * @param loglevel - Уровень логирования.
    * @param message - Сообщение.
    */
    URMC_URPC_API_EXPORT void URMC_URPC_CALLING_CONVENTION urmc_logging_callback_stderr_wide(int loglevel, const wchar_t *message, void *user_data);

    /**
    * \~english
    * Simple callback for logging to stderr in narrow (single byte) chars.
    * @param loglevel - A logging level.
    * @param message - A message.
    * \~russian
    * Простая функция логирования на stderr в узких (однобайтных) символах.
    * @param loglevel - Уровень логирования.
    * @param message - Сообщение.
    */
    URMC_URPC_API_EXPORT void URMC_URPC_CALLING_CONVENTION urmc_logging_callback_stderr_narrow(int loglevel, const wchar_t *message, void *user_data);

    /**
    * \~english
    * Sets a logging callback.
    * Passing NULL disables logging.
    * @param logging_callback a callback for log messages
    * \~russian
    * Устанавливает функцию обратного вызова для логирования.
    * Передача NULL в качестве аргумента отключает логирование.
    * @param logging_callback указатель на функцию обратного вызова
    */
    URMC_URPC_API_EXPORT void URMC_URPC_CALLING_CONVENTION urmc_set_logging_callback(urmc_logging_callback_t cb, void *data);
#define URMC_SETPOS_IGNORE_POSITION    0x1 /**< \~english Will not reload position in steps/microsteps if this flag is set. \~russian Если установлен, то позиция в шагах и микрошагах не обновляется. */
#define URMC_SETPOS_IGNORE_ENCODER    0x2 /**< \~english Will not reload encoder state if this flag is set. \~russian Если установлен, то счётчик энкодера не обновляется. */

#define URMC_MOVE_STATE_MOVING    0x1 /**< \~english This flag indicates that controller is trying to move the motor. Don't use this flag for waiting of completion of the movement command. Use MVCMD_RUNNING flag from the MvCmdSts field instead. \~russian Если флаг установлен, то контроллер пытается вращать двигателем. Не используйте этот флаг для ожидания завершения команды движения. Вместо него используйте MVCMD_RUNNING из поля MvCmdSts. */
#define URMC_MOVE_STATE_TARGET_SPEED    0x2 /**< \~english Target speed is reached, if flag set. \~russian Флаг устанавливается при достижении заданной скорости. */
#define URMC_MOVE_STATE_ANTIPLAY    0x4 /**< \~english Motor is playing compensation, if flag set. \~russian Выполняется компенсация люфта, если флаг установлен. */

#define URMC_MVCMD_NAME_BITS    0x3f /**< \~english Move command bit mask. \~russian Битовая маска активной команды. */
#define URMC_MVCMD_UKNWN    0x0 /**< \~english Unknown command. \~russian Неизвестная команда. */
#define URMC_MVCMD_MOVE    0x1 /**< \~english Command move. \~russian Команда move. */
#define URMC_MVCMD_MOVR    0x2 /**< \~english Command movr. \~russian Команда movr. */
#define URMC_MVCMD_LEFT    0x3 /**< \~english Command left. \~russian Команда left. */
#define URMC_MVCMD_RIGHT    0x4 /**< \~english Command rigt. \~russian Команда rigt. */
#define URMC_MVCMD_STOP    0x5 /**< \~english Command stop. \~russian Команда stop. */
#define URMC_MVCMD_HOME    0x6 /**< \~english Command home. \~russian Команда home. */
#define URMC_MVCMD_LOFT    0x7 /**< \~english Command loft. \~russian Команда loft. */
#define URMC_MVCMD_SSTP    0x8 /**< \~english Command soft stop. \~russian Команда плавной остановки(SSTP). */
#define URMC_MVCMD_ERROR    0x40 /**< \~english Finish state (1 - move command have finished with an error, 0 - move command have finished correctly). This flags is actual when MVCMD_RUNNING signals movement finish. \~russian Состояние завершения движения (1 - команда движения выполнена с ошибкой, 0 - команда движения выполнена корректно). Имеет смысл если MVCMD_RUNNING указывает на завершение движения. */
#define URMC_MVCMD_RUNNING    0x80 /**< \~english Move command state (0 - move command have finished, 1 - move command is being executed). \~russian Состояние команды движения (0 - команда движения выполнена, 1 - команда движения сейчас выполняется). */

#define URMC_PWR_STATE_UNKNOWN    0x0 /**< \~english Unknown state, should never happen. \~russian Неизвестное состояние, которое не должно никогда реализовываться. */
#define URMC_PWR_STATE_OFF    0x1 /**< \~english Motor windings are disconnected from the driver. \~russian Обмотки мотора разомкнуты и не управляются драйвером. */
#define URMC_PWR_STATE_NORM    0x3 /**< \~english Motor windings are powered by nominal current. \~russian Обмотки запитаны номинальным током. */
#define URMC_PWR_STATE_REDUCT    0x4 /**< \~english Motor windings are powered by reduced current to lower power consumption. \~russian Обмотки намеренно запитаны уменьшенным током от рабочего для снижения потребляемой мощности. */
#define URMC_PWR_STATE_MAX    0x5 /**< \~english Motor windings are powered by maximum current driver can provide at this voltage. \~russian Обмотки запитаны максимально доступным током, который может выдать схема при данном напряжении питания. */

#define URMC_ENC_STATE_ABSENT    0x0 /**< \~english Encoder is absent. \~russian Энкодер не подключен. */
#define URMC_ENC_STATE_UNKNOWN    0x1 /**< \~english Encoder state is unknown. \~russian Cостояние энкодера неизвестно. */
#define URMC_ENC_STATE_MALFUNC    0x2 /**< \~english Encoder is connected and malfunctioning. \~russian Энкодер подключен и неисправен. */
#define URMC_ENC_STATE_REVERS    0x3 /**< \~english Encoder is connected and operational but counts in other direction. \~russian Энкодер подключен и исправен, но считает в другую сторону. */
#define URMC_ENC_STATE_OK    0x4 /**< \~english Encoder is connected and working properly. \~russian Энкодер подключен и работает адекватно. */

#define URMC_STATE_CONTR    0x3f /**< \~english Flags of controller states. \~russian Флаги состояния контроллера. */
#define URMC_STATE_IS_HOMED    0x20 /**< \~english Calibration performed \~russian Калибровка выполнена */
#define URMC_STATE_SECUR    0x13ffc0 /**< \~english Flags of security. \~russian Флаги опасности. */
#define URMC_STATE_ALARM    0x40 /**< \~english Controller is in alarm state indicating that something dangerous had happened. Most commands are ignored in this state. To reset the flag a STOP command must be issued. \~russian Контроллер находится в состоянии ALARM, показывая, что случилась какая-то опасная ситуация. В состоянии ALARM все команды игнорируются пока не будет послана команда STOP и состояние ALARM деактивируется. */
#define URMC_STATE_CTP_ERROR    0x80 /**< \~english Control position error(is only used with stepper motor). \~russian Контроль позиции нарушен(используется только с шаговым двигателем). */
#define URMC_STATE_POWER_OVERHEAT    0x100 /**< \~english Power driver overheat. \~russian Перегрелась силовая часть платы. */
#define URMC_STATE_OVERLOAD_POWER_VOLTAGE    0x400 /**< \~english Power voltage exceeds safe limit. \~russian Превышено напряжение на силовой части. */
#define URMC_STATE_WINDINGS_MALFUNC    0x4000 /**< \~english The windings are malfunc. Detection occurs by comparing current integrals over several periods. If the difference is greater than the threshold, then the decision is made to enter the Alarm state. \~russian Обмотки повреждены. Детектирование происходит путём сравнения интегралов токов за несколько периодов. Если разница больше пороговой, то принимается решение войти в состояние Alarm. */
#define URMC_STATE_BORDERS_SWAP_MISSET    0x8000 /**< \~english Engine stuck at the wrong edge. \~russian Достижение неверной границы. */
#define URMC_STATE_LOW_POWER_VOLTAGE    0x10000 /**< \~english Power voltage is lower than Low Voltage Protection limit \~russian Напряжение на силовой части ниже чем напряжение Low Voltage Protection */
#define URMC_STATE_H_BRIDGE_FAULT    0x20000 /**< \~english Signal from the driver that fault happened \~russian Получен сигнал от драйвера о неисправности */
#define URMC_STATE_CURRENT_MOTOR_BITS    0xc0000 /**< \~english Bits indicating the current operating motor on boards with multiple outputs for engine mounting. \~russian Биты, показывающие текущий рабочий мотор на платах с несколькими выходами для двигателей. */
#define URMC_STATE_CURRENT_MOTOR0    0x0 /**< \~english Motor 0. \~russian Мотор 0. */
#define URMC_STATE_CURRENT_MOTOR1    0x40000 /**< \~english Motor 1. \~russian Мотор 1. */
#define URMC_STATE_CURRENT_MOTOR2    0x80000 /**< \~english Motor 2. \~russian Мотор 2. */
#define URMC_STATE_CURRENT_MOTOR3    0xc0000 /**< \~english Motor 3. \~russian Мотор 3. */
#define URMC_STATE_INCORRECT_MOTOR_SWITCH    0x100000 /**< \~english Error status when trying to switch the motor while driving. \~russian Состояние ошибки при попытке переключить мотор во время движения. */

#define URMC_STATE_DIG_SIGNAL    0xffff /**< \~english Flags of digital signals. \~russian Флаги цифровых сигналов. */
#define URMC_STATE_RIGHT_EDGE    0x1 /**< \~english Engine stuck at the right edge. \~russian Достижение правой границы. */
#define URMC_STATE_LEFT_EDGE    0x2 /**< \~english Engine stuck at the left edge. \~russian Достижение левой границы. */
#define URMC_STATE_REV_SENSOR    0x400 /**< \~english State of Revolution sensor pin. \~russian Состояние вывода датчика оборотов(флаг "1", если датчик активен). */

#define URMC_FEEDBACK_ENCODER    0x1 /**< \~english Feedback by encoder. \~russian Обратная связь с помощью энкодера. */
#define URMC_FEEDBACK_NONE    0x5 /**< \~english Feedback is absent. \~russian Обратная связь отсутствует. */

#define URMC_FEEDBACK_ENC_REVERSE    0x1 /**< \~english Reverse count of encoder. \~russian Обратный счет у энкодера. */
#define URMC_FEEDBACK_ENC_TYPE_BITS    0xe0 /**< \~english Bits of the encoder type. \~russian Биты, отвечающие за тип энкодера. */
#define URMC_FEEDBACK_ENC_TYPE_AUTO    0x0 /**< \~english Auto detect encoder type. \~russian Определять тип энкодера автоматически. */
#define URMC_FEEDBACK_ENC_TYPE_SINGLE_ENDED    0x40 /**< \~english Single ended encoder. \~russian Недифференциальный энкодер. */
#define URMC_FEEDBACK_ENC_TYPE_DIFFERENTIAL    0x80 /**< \~english Differential encoder. \~russian Дифференциальный энкодер. */
#define URMC_FEEDBACK_ENC_TYPE_POTENTIOMETER    0x20 /**< \~english Potentiometer encoder (input of the ADC) \~russian Потенциометр энкодер (вход АЦП) */

#define URMC_HOME_DIR_FIRST    0x1 /**< \~english Flag defines direction of 1st motion after execution of home command. Direction is right, if set; otherwise left. \~russian Определяет направление первоначального движения мотора после поступления команды HOME. Если флаг установлен - вправо; иначе - влево. */
#define URMC_HOME_DIR_SECOND    0x2 /**< \~english Flag defines direction of 2nd motion. Direction is right, if set; otherwise left. \~russian Определяет направление второго движения мотора. Если флаг установлен - вправо; иначе - влево. */
#define URMC_HOME_MV_SEC_EN    0x4 /**< \~english Use the second phase of calibration to the home position, if set; otherwise the second phase is skipped. \~russian Если флаг установлен, реализуется второй этап доводки в домашнюю позицию; иначе - этап пропускается. */
#define URMC_HOME_HALF_MV    0x8 /**< \~english If the flag is set, the stop signals are ignored in start of second movement the first half-turn. \~russian Если флаг установлен, в начале второго движения первые пол оборота сигналы завершения движения игнорируются. */
#define URMC_HOME_STOP_FIRST_BITS    0x30 /**< \~english Bits of the first stop selector. \~russian Биты, отвечающие за выбор сигнала завершения первого движения. */
#define URMC_HOME_STOP_FIRST_REV    0x10 /**< \~english First motion stops by  revolution sensor. \~russian Первое движение завершается по сигналу с Revolution sensor. */
#define URMC_HOME_STOP_FIRST_LIM    0x30 /**< \~english First motion stops by limit switch. \~russian Первое движение завершается по сигналу с концевика. */
#define URMC_HOME_STOP_SECOND_BITS    0xc0 /**< \~english Bits of the second stop selector. \~russian Биты, отвечающие за выбор сигнала завершения второго движения. */
#define URMC_HOME_STOP_SECOND_REV    0x40 /**< \~english Second motion stops by  revolution sensor. \~russian Второе движение завершается по сигналу с Revolution sensor. */
#define URMC_HOME_STOP_SECOND_LIM    0xc0 /**< \~english Second motion stops by limit switch. \~russian Второе движение завершается по сигналу с концевика. */
#define URMC_HOME_USE_FAST    0x100 /**< \~english Use the fast algorithm of calibration to the home position, if set; otherwise the traditional algorithm. \~russian Если флаг установлен, используется быстрый поиск домашней позиции; иначе - традиционный. */

#define URMC_ENGINE_REVERSE    0x1 /**< \~english Reverse flag. It determines motor shaft rotation direction that corresponds to feedback counts increasing. If not set (default), motor shaft rotation direction under positive voltage corresponds to feedback counts increasing and vice versa. Change it if you see that positive directions on motor and feedback are opposite. \~russian Флаг реверса. Связывает направление вращения мотора с направлением счета текущей позиции. При сброшенном флаге (по умолчанию) прикладываемое к мотору положительное напряжение увеличивает счетчик позиции. И наоборот, при установленном флаге счетчик позиции увеличивается, когда к мотору приложено отрицательное напряжение. Измените состояние флага, если положительное вращение мотора уменьшает счетчик позиции. */
#define URMC_ENGINE_CURRENT_AS_RMS    0x2 /**< \~english Engine current meaning flag. If the flag is set, then engine current value is interpreted as root mean square current value. If the flag is unset, then engine current value is interpreted as maximum amplitude value. \~russian Флаг интерпретации значения тока. Если флаг установлен, то задаваемое значение тока интерпретируется как среднеквадратичное значение тока, если флаг снят, то задаваемое значение тока интерпретируется как максимальная амплитуда тока. */
#define URMC_ENGINE_MAX_SPEED    0x4 /**< \~english Max speed flag. If it is set, engine uses maximum speed achievable with the present engine settings as nominal speed. \~russian Флаг максимальной скорости. Если флаг установлен, движение происходит на максимальной скорости. */
#define URMC_ENGINE_ANTIPLAY    0x8 /**< \~english Play compensation flag. If it set, engine makes backlash (play) compensation procedure and reach the predetermined position accurately on low speed. \~russian Компенсация люфта. Если флаг установлен, позиционер будет подходить к заданной точке всегда с одной стороны. Например, при подходе слева никаких дополнительных действий не совершается, а при подходе справа позиционер проходит целевую позицию на заданное расстояния и возвращается к ней опять же справа. */
#define URMC_ENGINE_ACCEL_ON    0x10 /**< \~english Acceleration enable flag. If it set, motion begins with acceleration and ends with deceleration. \~russian Ускорение. Если флаг установлен, движение происходит с ускорением. */
#define URMC_ENGINE_LIMIT_VOLT    0x20 /**< \~english Maximum motor voltage limit enable flag(is only used with DC motor). \~russian Номинальное напряжение мотора. Если флаг установлен, напряжение на моторе ограничивается заданным номинальным значением(используется только с DC двигателем). */
#define URMC_ENGINE_LIMIT_CURR    0x40 /**< \~english Maximum motor current limit enable flag(is only used with DC motor). \~russian Номинальный ток мотора. Если флаг установлен, ток через мотор ограничивается заданным номинальным значением(используется только с DC двигателем). */
#define URMC_ENGINE_LIMIT_RPM    0x80 /**< \~english Maximum motor speed limit enable flag. \~russian Номинальная частота вращения мотора. Если флаг установлен, частота вращения ограничивается заданным номинальным значением. */

#define URMC_MICROSTEP_MODE_FULL    0x1 /**< \~english Full step mode. \~russian Полношаговый режим. */
#define URMC_MICROSTEP_MODE_FRAC_2    0x2 /**< \~english 1/2 step mode. \~russian Деление шага 1/2. */
#define URMC_MICROSTEP_MODE_FRAC_4    0x3 /**< \~english 1/4 step mode. \~russian Деление шага 1/4. */
#define URMC_MICROSTEP_MODE_FRAC_8    0x4 /**< \~english 1/8 step mode. \~russian Деление шага 1/8. */
#define URMC_MICROSTEP_MODE_FRAC_16    0x5 /**< \~english 1/16 step mode. \~russian Деление шага 1/16. */
#define URMC_MICROSTEP_MODE_FRAC_32    0x6 /**< \~english 1/32 step mode. \~russian Деление шага 1/32. */
#define URMC_MICROSTEP_MODE_FRAC_64    0x7 /**< \~english 1/64 step mode. \~russian Деление шага 1/64. */
#define URMC_MICROSTEP_MODE_FRAC_128    0x8 /**< \~english 1/128 step mode. \~russian Деление шага 1/128. */
#define URMC_MICROSTEP_MODE_FRAC_256    0x9 /**< \~english 1/256 step mode. \~russian Деление шага 1/256. */

#define URMC_ENGINE_TYPE_NONE    0x0 /**< \~english A value that shouldn't be used. \~russian Это значение не нужно использовать. */
#define URMC_ENGINE_TYPE_DC    0x1 /**< \~english DC motor. \~russian Мотор постоянного тока. */
#define URMC_ENGINE_TYPE_STEP    0x3 /**< \~english Step motor. \~russian Шаговый мотор. */
#define URMC_ENGINE_TYPE_TEST    0x4 /**< \~english Duty cycle are fixed. Used only manufacturer. \~russian Скважность в обмотках фиксирована. Используется только производителем. */

#define URMC_POWER_REDUCT_ENABLED    0x1 /**< \~english Current reduction enabled after CurrReductDelay, if this flag is set. \~russian Если флаг установлен, уменьшить ток по прошествии CurrReductDelay. Иначе - не уменьшать. */
#define URMC_POWER_OFF_ENABLED    0x2 /**< \~english Power off enabled after PowerOffDelay, if this flag is set. \~russian Если флаг установлен, снять напряжение с обмоток по прошествии PowerOffDelay. Иначе - не снимать. */
#define URMC_POWER_SMOOTH_CURRENT    0x4 /**< \~english Current ramp-up/down is performed smoothly during current_set_time, if this flag is set. \~russian Если установлен, то запитывание обмоток, снятие питания или снижение/повышение тока происходят плавно со скоростью CurrentSetTime, а только потом выполняется та задача, которая вызвала это плавное изменение. */

#define URMC_ALARM_ON_DRIVER_OVERHEATING    0x1 /**< \~english If this flag is set enter Alarm state on driver overheat signal. \~russian Если флаг установлен, то войти в состояние Alarm при получении сигнала подступающего перегрева с драйвера. Иначе - игнорировать подступающий перегрев с драйвера. */
#define URMC_LOW_UPWR_PROTECTION    0x2 /**< \~english If this flag is set turn off motor when voltage is lower than LowUpwrOff. \~russian Если установлен, то выключать силовую часть при напряжении меньшем LowUpwrOff. */
#define URMC_H_BRIDGE_ALERT    0x4 /**< \~english If this flag is set then turn off the power unit with a signal problem in one of the transistor bridge. \~russian Если установлен, то выключать силовую часть при сигнале неполадки в одном из транзисторных мостов. */
#define URMC_ALARM_ON_BORDERS_SWAP_MISSET    0x8 /**< \~english If this flag is set enter Alarm state on borders swap misset \~russian Если флаг установлен, то войти в состояние Alarm при получении сигнала c противоположного концевика. */
#define URMC_ALARM_FLAGS_STICKING    0x10 /**< \~english If this flag is set only a STOP command can turn all alarms to 0 \~russian Если флаг установлен, то только по команде STOP возможен сброс всех флагов ALARM. */
#define URMC_USB_BREAK_RECONNECT    0x20 /**< \~english If this flag is set USB brake reconnect module will be enable \~russian Если флаг установлен, то будет включен блок перезагрузки USB при поломке связи. */
#define URMC_ALARM_ON_WINDINGS_MALFUNC    0x40 /**< \~english If this flag is set then enter Alarm when the WINDINGS_MALFUNC state occurs. \~russian Если флаг установлен то войти Alarm при возникновении состояния  WINDINGS_MALFUNC */

#define URMC_BORDER_IS_ENCODER    0x1 /**< \~english Borders are fixed by predetermined encoder values, if set; borders position on limit switches, if not set. \~russian Если флаг установлен, границы определяются предустановленными точками на шкале позиции. Если флаг сброшен, границы определяются концевыми выключателями. */
#define URMC_BORDER_STOP_LEFT    0x2 /**< \~english Motor should stop on left border. \~russian Если флаг установлен, мотор останавливается при достижении левой границы. */
#define URMC_BORDER_STOP_RIGHT    0x4 /**< \~english Motor should stop on right border. \~russian Если флаг установлен, мотор останавливается при достижении правой границы. */
#define URMC_BORDERS_SWAP_MISSET_DETECTION    0x8 /**< \~english Motor should stop on both borders. Need to save motor then wrong border settings is set \~russian Если флаг установлен, мотор останавливается при достижении обоих границ. Нужен для предотвращения поломки двигателя при неправильных настройках концевиков */

#define URMC_ENDER_SWAP    0x1 /**< \~english First limit switch on the right side, if set; otherwise on the left side. \~russian Если флаг установлен, первый концевой выключатель находится справа; иначе - слева. */
#define URMC_ENDER_SW1_ACTIVE_LOW    0x2 /**< \~english 1 - Limit switch connnected to pin SW1 is triggered by a low level on pin. \~russian 1 - Концевик, подключенный к ножке SW1, считается сработавшим по низкому уровню на контакте. */
#define URMC_ENDER_SW2_ACTIVE_LOW    0x4 /**< \~english 1 - Limit switch connnected to pin SW2 is triggered by a low level on pin. \~russian 1 - Концевик, подключенный к ножке SW2, считается сработавшим по низкому уровню на контакте. */

#define URMC_CTP_ENABLED    0x1 /**< \~english Position control is enabled, if flag set. \~russian Контроль позиции включен, если флаг установлен. */
#define URMC_CTP_BASE    0x2 /**< \~english Position control is based on revolution sensor, if this flag is set; otherwise it is based on encoder. \~russian Опорой является датчик оборотов, если флаг установлен; иначе - энкодер. */
#define URMC_CTP_ALARM_ON_ERROR    0x4 /**< \~english Set ALARM on mismatch, if flag set. \~russian Войти в состояние ALARM при расхождении позиции, если флаг установлен. */
#define URMC_REV_SENS_INV    0x8 /**< \~english Sensor is active when it 0 and invert makes active level 1. That is, if you do not invert, it is normal logic - 0 is the activation. \~russian Сенсор считается активным, когда на нём 0, а инвертирование делает активным уровнем 1. То есть если не инвертировать, то действует обычная логика - 0 это срабатывание/активация/активное состояние. */
#define URMC_CTP_ERROR_CORRECTION    0x10 /**< \~english Correct errors which appear when slippage if the flag is set. It works only with the encoder. Incompatible with flag CTP_ALARM_ON_ERROR. \~russian Корректировать ошибки, возникающие при проскальзывании, если флаг установлен. Работает только с энкодером. Несовместимо с флагом CTP_ALARM_ON_ERROR. */

#define URMC_EEPROM_PRECEDENCE    0x1 /**< \~english If the flag is set settings from external EEPROM override controller settings. \~russian Если флаг установлен, то настройки в EEPROM подвижки имеют приоритет над текущими настройками и заменяют их при обнаружении EEPROM. */
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

    typedef struct
    {
        int32_t Speed[25]; /**< \~english Current speed. \~russian Текущая скорость. */
        int32_t Error[25]; /**< \~english Current error. \~russian Текущая скорость. */
        uint32_t Length; /**< \~english Length of actual data in buffer. \~russian Длина фактических данных в буфере. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_measurements_t;

    typedef struct
    {
        int16_t WindingVoltageA; /**< \~english In the case step motor, the voltage across the winding A; in the case of a brushless, the voltage on the first coil, in the case of the only DC. \~russian В случае ШД, напряжение на обмотке A; в случае бесщеточного, напряжение на первой обмотке; в случае DC на единственной. */
        int16_t WindingVoltageB; /**< \~english In the case step motor, the voltage across the winding B; in case of a brushless, the voltage on the second winding, and in the case of DC is not used. \~russian В случае ШД, напряжение на обмотке B; в случае бесщеточного, напряжение на второй обмотке; в случае DC не используется. */
        uint8_t reserved0[2]; /**< \~english  \~russian  */
        int16_t WindingCurrentA; /**< \~english In the case step motor, the current in the coil A; brushless if the current in the first coil, and in the case of a single DC. \~russian В случае ШД, ток в обмотке A; в случае бесщеточного, ток в первой обмотке; в случае DC в единственной. */
        int16_t WindingCurrentB; /**< \~english In the case step motor, the current in the coil B; brushless if the current in the second coil, and in the case of DC is not used. \~russian В случае ШД, ток в обмотке B; в случае бесщеточного, ток в второй обмотке; в случае DC не используется. */
        uint8_t reserved1[6]; /**< \~english  \~russian  */
        int16_t DutyCycle; /**< \~english Duty cycle of PWM. \~russian Коэффициент заполнения ШИМ. */
        uint8_t reserved[14]; /**< \~english  \~russian  */

    } urmc_chart_data_t;

    typedef struct
    {
        int8_t Manufacturer[4]; /**< \~english Manufacturer \~russian Производитель */
        int8_t ManufacturerId[2]; /**< \~english Manufacturer id \~russian Идентификатор производителя */
        int8_t ProductDescription[8]; /**< \~english Product description \~russian Описание продукта */
        uint8_t Major; /**< \~english The major number of the hardware version. \~russian Основной номер версии железа. */
        uint8_t Minor; /**< \~english Minor number of the hardware version. \~russian Второстепенный номер версии железа. */
        uint16_t Release; /**< \~english Number of edits this release of hardware. \~russian Номер правок этой версии железа. */
        uint8_t reserved[12]; /**< \~english  \~russian  */

    } urmc_device_information_impl_t;

    typedef struct
    {
        uint32_t SerialNumber; /**< \~english Board serial number. \~russian Серийный номер платы. */

    } urmc_get_serial_number_t;

    typedef struct
    {
        uint8_t Major; /**< \~english Firmware major version number \~russian Мажорный номер версии прошивки */
        uint8_t Minor; /**< \~english Firmware minor version number \~russian Минорный номер версии прошивки */
        uint16_t Release; /**< \~english Firmware release version number \~russian Номер релиза версии прошивки */

    } urmc_firmware_version_t;

    typedef struct
    {
        uint16_t A1Voltage_ADC; /**< \~english "Voltage on pin 1 winding A" raw data from ADC. \~russian "Выходное напряжение на 1 выводе обмотки А" необработанные данные с АЦП. */
        uint16_t A2Voltage_ADC; /**< \~english "Voltage on pin 2 winding A" raw data from ADC. \~russian "Выходное напряжение на 2 выводе обмотки А" необработанные данные с АЦП. */
        uint16_t B1Voltage_ADC; /**< \~english "Voltage on pin 1 winding B" raw data from ADC. \~russian "Выходное напряжение на 1 выводе обмотки B" необработанные данные с АЦП. */
        uint16_t B2Voltage_ADC; /**< \~english "Voltage on pin 2 winding B" raw data from ADC. \~russian "Выходное напряжение на 2 выводе обмотки B" необработанные данные с АЦП. */
        uint16_t SupVoltage_ADC; /**< \~english "Voltage on the top of MOSFET full bridge" raw data from ADC. \~russian "Напряжение питания ключей Н-моста" необработанные данные с АЦП. */
        uint16_t ACurrent_ADC; /**< \~english "Winding A current" raw data from ADC. \~russian "Ток через обмотку А" необработанные данные с АЦП. */
        uint16_t BCurrent_ADC; /**< \~english "Winding B current" raw data from ADC. \~russian "Ток через обмотку B" необработанные данные с АЦП. */
        uint8_t reserved0[8]; /**< \~english  \~russian  */
        uint16_t L5_ADC; /**< \~english USB supply voltage after the current sense resistor, from ADC. \~russian Напряжение питания USB после current sense резистора, необработанные данные с АЦП. */
        uint8_t reserved1[2]; /**< \~english  \~russian  */
        int16_t A1Voltage; /**< \~english "Voltage on pin 1 winding A" calibrated data. \~russian "Выходное напряжение на 1 выводе обмотки А" откалиброванные данные. */
        int16_t A2Voltage; /**< \~english "Voltage on pin 2 winding A" calibrated data. \~russian "Выходное напряжение на 2 выводе обмотки А" откалиброванные данные. */
        int16_t B1Voltage; /**< \~english "Voltage on pin 1 winding B" calibrated data. \~russian "Выходное напряжение на 1 выводе обмотки B" откалиброванные данные. */
        int16_t B2Voltage; /**< \~english "Voltage on pin 2 winding B" calibrated data. \~russian "Выходное напряжение на 2 выводе обмотки B" откалиброванные данные. */
        int16_t SupVoltage; /**< \~english "Voltage on the top of MOSFET full bridge" calibrated data. \~russian "Напряжение питания ключей Н-моста" откалиброванные данные. */
        int16_t ACurrent; /**< \~english "Winding A current" calibrated data. \~russian "Ток через обмотку А" откалиброванные данные. */
        int16_t BCurrent; /**< \~english "Winding B current" calibrated data. \~russian "Ток через обмотку B" откалиброванные данные. */
        uint8_t reserved2[8]; /**< \~english  \~russian  */
        int16_t L5; /**< \~english USB supply voltage after the current sense resistor. \~russian Напряжение питания USB после current sense резистора */
        uint8_t reserved3[4]; /**< \~english  \~russian  */
        int32_t R; /**< \~english Motor winding resistance in mOhms(is only used with stepper motor). \~russian Сопротивление обмоток двигателя(для шагового двигателя),  в мОм */
        int32_t L; /**< \~english Motor winding pseudo inductance in uHn(is only used with stepper motor). \~russian Псевдоиндуктивность обмоток двигателя(для шагового двигателя),  в мкГн */
        uint8_t reserved[8]; /**< \~english  \~russian  */

    } urmc_analog_data_t;

    typedef struct
    {
        uint8_t DebugData[128]; /**< \~english Arbitrary debug data. \~russian Отладочные данные. */
        uint8_t reserved[8]; /**< \~english  \~russian  */

    } urmc_debug_read_t;

    typedef struct
    {
        uint8_t DebugData[128]; /**< \~english Arbitrary debug data. \~russian Отладочные данные. */
        uint8_t reserved[8]; /**< \~english  \~russian  */

    } urmc_debug_write_t;

    typedef struct
    {
        uint8_t Major; /**< \~english Bootloader major version number \~russian Мажорный номер версии загрузчика */
        uint8_t Minor; /**< \~english Bootloader minor version number \~russian Минорный номер версии загрузчика */
        uint16_t Release; /**< \~english Bootloader release version number \~russian Номер релиза версии загрузчика */

    } urmc_bootloader_version_t;

    typedef struct
    {
        uint8_t Motor; /**< \~english Motor number which it should be switch relay on [0..1] \~russian Номер мотора, на который следует переключить реле [0..1] */
        uint8_t reserved[15]; /**< \~english  \~russian  */

    } urmc_change_motor_t;

    typedef struct
    {
        uint16_t IPS; /**< \~english The number of encoder counts per shaft revolution. Range: 1..655535. The field is obsolete, it is recommended to write 0 to IPS and use the extended CountsPerTurn field. You may need to update the controller firmware to the latest version. \~russian Количество отсчётов энкодера на оборот вала. Диапазон: 1..65535. Поле устарело, рекомендуется записывать 0 в IPS и использовать расширенное поле CountsPerTurn. Может потребоваться обновление микропрограммы контроллера до последней версии. */
        uint8_t FeedbackType; /**< \~english Type of feedback \~russian Тип обратной связи */
        uint8_t FeedbackFlags; /**< \~english Flags \~russian Флаги */
        uint32_t CountsPerTurn; /**< \~english The number of encoder counts per shaft revolution. Range: 1..4294967295. To use the CountsPerTurn field, write 0 in the IPS field, otherwise the value from the IPS field will be used. \~russian Количество отсчётов энкодера на оборот вала. Диапазон: 1..4294967295. Для использования поля CountsPerTurn нужно записать 0 в поле IPS, иначе будет использоваться значение из поля IPS. */
        uint8_t reserved[4]; /**< \~english  \~russian  */

    } urmc_feedback_settings_t;

    typedef struct
    {
        uint32_t FastHome; /**< \~english Speed used for first motion. Range: 0..100000. \~russian Скорость первого движения. Диапазон: 0..100000 */
        uint8_t uFastHome; /**< \~english Part of the speed for first motion, microsteps. \~russian Дробная часть скорости первого движения в микрошагах(используется только с шаговым двигателем). */
        uint32_t SlowHome; /**< \~english Speed used for second motion. Range: 0..100000. \~russian Скорость второго движения. Диапазон: 0..100000. */
        uint8_t uSlowHome; /**< \~english Part of the speed for second motion, microsteps. \~russian Дробная часть скорости второго движения в микрошагах(используется только с шаговым двигателем). */
        int32_t HomeDelta; /**< \~english Distance from break point. \~russian Расстояние отхода от точки останова. */
        int16_t uHomeDelta; /**< \~english Part of the delta distance, microsteps. Range: -255..255. \~russian Дробная часть расстояния отхода от точки останова в микрошагах(используется только с шаговым двигателем). Диапазон: -255..255. */
        uint16_t HomeFlags; /**< \~english Set of flags specify direction and stopping conditions. \~russian Набор флагов, определяющие такие параметры, как направление и условия останова. */
        uint8_t reserved[9]; /**< \~english  \~russian  */

    } urmc_home_settings_t;

    typedef struct
    {
        uint32_t Speed; /**< \~english Target speed (for stepper motor: steps/s, for DC: rpm). Range: 0..100000. \~russian Заданная скорость (для ШД: шагов/c, для DC: rpm). Диапазон: 0..100000. */
        uint8_t uSpeed; /**< \~english Target speed in microstep fractions/s. Using with stepper motor only. \~russian Заданная скорость в единицах деления микрошага в секунду. Используется только с шаговым мотором. */
        uint16_t Accel; /**< \~english Motor shaft acceleration, steps/s^2(stepper motor) or RPM/s(DC). Range: 1..65535. \~russian Ускорение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). Диапазон: 1..65535. */
        uint16_t Decel; /**< \~english Motor shaft deceleration, steps/s^2(stepper motor) or RPM/s(DC). Range: 1..65535. \~russian Торможение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). Диапазон: 1..65535. */
        uint32_t AntiplaySpeed; /**< \~english Speed in antiplay mode, full steps/s(stepper motor) or RPM(DC). Range: 0..100000. \~russian Скорость в режиме антилюфта, заданная в целых шагах/c(ШД) или в оборотах/с(DC). Диапазон: 0..100000. */
        uint8_t uAntiplaySpeed; /**< \~english Speed in antiplay mode, 1/256 microsteps/s. Used with stepper motor only. \~russian Скорость в режиме антилюфта, выраженная в 1/256 микрошагах в секунду. Используется только с шаговым мотором. */
        uint8_t reserved[10]; /**< \~english  \~russian  */

    } urmc_move_settings_t;

    typedef struct
    {
        uint16_t NomVoltage; /**< \~english Rated voltage in tens of mV. Controller will keep the voltage drop on motor below this value if ENGINE_LIMIT_VOLT flag is set (used with DC only). \~russian Номинальное напряжение мотора в десятках мВ. Контроллер будет сохранять напряжение на моторе не выше номинального, если установлен флаг ENGINE_LIMIT_VOLT (используется только с DC двигателем). */
        uint16_t NomCurrent; /**< \~english Rated current. Controller will keep current consumed by motor below this value if ENGINE_LIMIT_CURR flag is set. Range: 15..8000 \~russian Номинальный ток через мотор. Ток стабилизируется для шаговых и может быть ограничен для DC(если установлен флаг ENGINE_LIMIT_CURR). Диапазон: 15..8000 */
        uint32_t NomSpeed; /**< \~english Nominal speed (in whole steps/s or rpm for DC and stepper motor as a master encoder). Controller will keep motor shaft RPM below this value if ENGINE_LIMIT_RPM flag is set. Range: 1..100000. \~russian Номинальная скорость (в целых шагах/с или rpm для DC и шагового двигателя в режиме ведущего энкодера). Контроллер будет сохранять скорость мотора не выше номинальной, если установлен флаг ENGINE_LIMIT_RPM. Диапазон: 1..100000. */
        uint8_t uNomSpeed; /**< \~english The fractional part of a nominal speed in microsteps (is only used with stepper motor). \~russian Микрошаговая часть номинальной скорости мотора (используется только с шаговым двигателем). */
        uint16_t EngineFlags; /**< \~english Set of flags specify motor shaft movement algorithm and list of limitations \~russian Флаги, управляющие работой мотора. */
        int16_t Antiplay; /**< \~english Number of pulses or steps for backlash (play) compensation procedure. Used if ENGINE_ANTIPLAY flag is set. \~russian Количество шагов двигателя или импульсов энкодера, на которое позиционер будет отъезжать от заданной позиции для подхода к ней с одной и той же стороны. Используется, если установлен флаг ENGINE_ANTIPLAY. */
        uint8_t MicrostepMode; /**< \~english Settings of microstep mode(Used with stepper motor only). \~russian Настройки микрошагового режима(используется только с шаговым двигателем). */
        uint16_t StepsPerRev; /**< \~english Number of full steps per revolution(Used with stepper motor only). Range: 1..65535. \~russian Количество полных шагов на оборот(используется только с шаговым двигателем). Диапазон: 1..65535. */
        uint8_t reserved[12]; /**< \~english  \~russian  */

    } urmc_engine_settings_t;

    typedef struct
    {
        uint8_t EngineType; /**< \~english Engine type \~russian Тип мотора */
        uint8_t reserved[7]; /**< \~english  \~russian  */

    } urmc_entype_settings_t;

    typedef struct
    {
        uint8_t HoldCurrent; /**< \~english Current in holding regime, percent of nominal. Range: 0..100. \~russian Ток мотора в режиме удержания, в процентах от номинального. Диапазон: 0..100. */
        uint16_t CurrReductDelay; /**< \~english Time in ms from going to STOP state to reducting current. \~russian Время в мс от перехода в состояние STOP до уменьшения тока. */
        uint16_t PowerOffDelay; /**< \~english Time in s from going to STOP state to turning power off. \~russian Время в с от перехода в состояние STOP до отключения питания мотора. */
        uint16_t CurrentSetTime; /**< \~english Time in ms to reach nominal current. \~russian Время в мс, требуемое для набора номинального тока от 0% до 100%. */
        uint8_t PowerFlags; /**< \~english Flags with parameters of power control. \~russian Флаги параметров управления питанием. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_power_settings_t;

    typedef struct
    {
        uint16_t LowUpwrOff; /**< \~english Lower voltage limit to turn off the motor, tens of mV. \~russian Нижний порог напряжения на силовой части для выключения, десятки мВ. */
        uint8_t reserved0[2]; /**< \~english  \~russian  */
        uint16_t CriticalUpwr; /**< \~english Maximum motor voltage which triggers ALARM state, tens of mV. \~russian Максимальное напряжение на силовой части, вызывающее состояние ALARM, десятки мВ. */
        uint8_t reserved1[8]; /**< \~english  \~russian  */
        uint8_t Flags; /**< \~english Critical parameter flags. \~russian Флаги критических параметров. */
        uint8_t reserved[7]; /**< \~english  \~russian  */

    } urmc_secure_settings_t;

    typedef struct
    {
        uint8_t BorderFlags; /**< \~english Border flags, specify types of borders and motor behaviour on borders. \~russian Флаги, определяющие тип границ и поведение мотора при их достижении. */
        uint8_t EnderFlags; /**< \~english Ender flags, specify electrical behaviour of limit switches like order and pulled positions. \~russian Флаги, определяющие настройки концевых выключателей. */
        int32_t LeftBorder; /**< \~english Left border position, used if BORDER_IS_ENCODER flag is set. \~russian Позиция левой границы, используется если установлен флаг BORDER_IS_ENCODER. */
        int16_t uLeftBorder; /**< \~english Left border position in 1/256 microsteps(used with stepper motor only). Range: -255..255. \~russian Позиция левой границы в 1/256 микрошагах( используется только с шаговым двигателем). Диапазон: -255..255. */
        int32_t RightBorder; /**< \~english Right border position, used if BORDER_IS_ENCODER flag is set. \~russian Позиция правой границы, используется если установлен флаг BORDER_IS_ENCODER. */
        int16_t uRightBorder; /**< \~english Right border position in 1/256 microsteps. Used with stepper motor only. Range: -255..255. \~russian Позиция правой границы в 1/256 микрошагах (используется только с шаговым двигателем). Диапазон: -255..255. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_edges_settings_t;

    typedef struct
    {
        uint16_t KpU; /**< \~english Proportional gain for voltage PID routine \~russian Пропорциональный коэффициент ПИД контура по напряжению */
        uint16_t KiU; /**< \~english Integral gain for voltage PID routine \~russian Интегральный коэффициент ПИД контура по напряжению */
        uint16_t KdU; /**< \~english Differential gain for voltage PID routine \~russian Дифференциальный коэффициент ПИД контура по напряжению */
        float Kpf; /**< \~english Proportional gain for BLDC position PID routine \~russian Пропорциональный коэффициент ПИД контура по позиции для BLDC */
        float Kif; /**< \~english Integral gain for BLDC position PID routine \~russian Интегральный коэффициент ПИД контура по позиции для BLDC */
        float Kdf; /**< \~english Differential gain for BLDC position PID routine \~russian Дифференциальный коэффициент ПИД контура по позиции для BLDC */
        uint8_t reserved[24]; /**< \~english  \~russian  */

    } urmc_pid_settings_t;

    typedef struct
    {
        uint8_t reserved[22]; /**< \~english  \~russian  */

    } urmc_sync_in_settings_t;

    typedef struct
    {
        uint8_t reserved[10]; /**< \~english  \~russian  */

    } urmc_sync_out_settings_t;

    typedef struct
    {
        uint8_t reserved[12]; /**< \~english  \~russian  */

    } urmc_extio_settings_t;

    typedef struct
    {
        uint8_t reserved[19]; /**< \~english  \~russian  */

    } urmc_brake_settings_t;

    typedef struct
    {
        uint8_t reserved[87]; /**< \~english  \~russian  */

    } urmc_control_settings_t;

    typedef struct
    {
        uint8_t reserved[16]; /**< \~english  \~russian  */

    } urmc_joystick_settings_t;

    typedef struct
    {
        uint8_t CTPMinError; /**< \~english Minimum contrast steps from step motor encoder position, wich set STATE_CTP_ERROR flag. Measured in steps step motor. \~russian Минимальное отличие шагов ШД от положения энкодера, устанавливающее флаг STATE_RT_ERROR. Измеряется в шагах ШД. */
        uint8_t CTPFlags; /**< \~english Flags. \~russian Флаги. */
        uint8_t reserved[10]; /**< \~english  \~russian  */

    } urmc_ctp_settings_t;

    typedef struct
    {
        uint8_t reserved[10]; /**< \~english  \~russian  */

    } urmc_uart_settings_t;

    typedef struct
    {
        int8_t ControllerName[16]; /**< \~english User conroller name. Can be set by user for his/her convinience. Max string length: 16 chars. \~russian Пользовательское имя контроллера. Может быть установлено пользователем для его удобства. Максимальная длина строки: 16 символов. */
        uint8_t CtrlFlags; /**< \~english Internal controller settings. \~russian Настройки контроллера. */
        uint8_t reserved[7]; /**< \~english  \~russian  */

    } urmc_controller_name_t;

    typedef struct
    {
        uint8_t reserved[30]; /**< \~english  \~russian  */

    } urmc_nonvolatile_memory_t;

    typedef struct
    {
        uint8_t reserved[24]; /**< \~english  \~russian  */

    } urmc_stage_name_t;

    typedef struct
    {
        uint8_t reserved[64]; /**< \~english  \~russian  */

    } urmc_stage_information_t;

    typedef struct
    {
        uint8_t reserved[64]; /**< \~english  \~russian  */

    } urmc_stage_settings_t;

    typedef struct
    {
        uint8_t reserved[64]; /**< \~english  \~russian  */

    } urmc_motor_information_t;

    typedef struct
    {
        uint8_t reserved[106]; /**< \~english  \~russian  */

    } urmc_motor_settings_t;

    typedef struct
    {
        uint8_t reserved[64]; /**< \~english  \~russian  */

    } urmc_encoder_information_t;

    typedef struct
    {
        uint8_t reserved[48]; /**< \~english  \~russian  */

    } urmc_encoder_settings_t;

    typedef struct
    {
        uint8_t reserved[64]; /**< \~english  \~russian  */

    } urmc_hallsensor_information_t;

    typedef struct
    {
        uint8_t reserved[44]; /**< \~english  \~russian  */

    } urmc_hallsensor_settings_t;

    typedef struct
    {
        uint8_t reserved[64]; /**< \~english  \~russian  */

    } urmc_gear_information_t;

    typedef struct
    {
        uint8_t reserved[52]; /**< \~english  \~russian  */

    } urmc_gear_settings_t;

    typedef struct
    {
        uint8_t reserved[108]; /**< \~english  \~russian  */

    } urmc_accessories_settings_t;
    /**
    * \~english
    * Open a device by name \a name and return identifier of the device which can be used in calls.
    * @param[in] name - A device name.
    * Device name has form "com:port" or "xi-net://host/serial" or "udp://host:port".
    * In case of USB-COM port the "port" is the OS device uri.
    * For example "com:\\.\COM3" in Windows or "com:///dev/ttyACM34" in Linux/Mac.
    * In case of network device the "host" is an IPv4 address or fully qualified domain uri (FQDN),
    * "serial" is the device serial number in hexadecimal system.
    * For example "xi-net://192.168.0.1/00001234" or "xi-net://hostname.com/89ABCDEF".
    * In case of ethernet udp-com adapter the "host" is an IPv4 address, "port" is network port
    * For example: "udp://192.168.0.2:1024"
    * Note: only one program may use COM-device in same time.
    * If errors occur when opening device, you need to make sure that the COM port is in the system and
    * device is not currently used by other programs.
    * \~russian
    * Открывает устройство по имени \a name и возвращает идентификатор устройства.
    * @param[in] name - Имя устройства.
    * Имя устройства имеет вид "com:port" или xi-net://host/serial или udp://host:port.
    * Для COM устройства "port" это имя устройства в ОС.
    * Например "com:\\.\COM3" (Windows) или "com:///dev/tty/ttyACM34" (Linux/Mac).
    * Для сетевого (XiNet) устройства "host" это IPv4 адрес или полностью определённое имя домена,
    * "serial" это серийный номер устройства в шестнадцатеричной системе.
    * Например "xi-net://192.168.0.1/00001234" или "xi-net://hostname.com/89ABCDEF".
    * Для ethernet переходника com-udp "host" это IPv4 адрес переходника, "port" это порт переходника.
    * Например "udp://192.168.0.2:1024"
    * Замечание: в один момент времени COM устройство может использоваться только одной программой.
    * Если при открытии устройства возникают ошибки, нужно убедиться, что COM-порт есть в системе и что это
    * устройство в данный момент не используется другими программами
    */
    // URMC_URPC_API_EXPORT device_t URMC_URPC_CALLING_CONVENTION urmc_open_device(const char *uri);

    /**
    * \~english
    * Get library version.
    * @param[out] lib_version - Library version.
    * \~russian
    * Версия библиотеки.
    * @param[out] lib_version - Версия библиотеки.
    */
    // URMC_URPC_API_EXPORT result_t URMC_URPC_CALLING_CONVENTION urmc_libversion(char *lib_version);

    /**
    * \~english
    * Return device identity information such as firmware version and serial number. It is useful to find your device in a list of available devices. It can be called from the firmware and bootloader.
    * @param[in] handle - Device ID, obtained by urmc_open_device() function.
    * @param[out] output - Device out data.
    * \~russian
    * Возвращает идентификационную информацию об устройстве, такую как номера версий прошивки и серийный номер. Эта информация удобна для поиска нужного устройства среди списка доступных. Может быть вызвана как из прошивки, так и из бутлоадера.
    * @param[in] handle - Идентификатор устройства, полученный от urmc_open_device().
    * @param[out] output - Данные, получаемые с устройства.
    */
    URMC_URPC_API_EXPORT result_t URMC_URPC_CALLING_CONVENTION urmc_get_identity_information(device_t handle, urmc_get_identity_information_t *output);

    /**
    * \~english
    * Immediately stop the engine, the transition to the STOP, mode key BREAK (winding short-circuited), the regime "retention" is deactivated for DC motors, keeping current in the windings for stepper motors (with Power management settings).
    * @param[in] handle - Device ID, obtained by urmc_open_device() function.
    * \~russian
    * Немедленная остановка двигателя, переход в состояние STOP, ключи в режиме BREAK (обмотки накоротко замкнуты), режим "удержания" дезактивируется для DC двигателей, удержание тока в обмотках для шаговых двигателей (с учётом Power management настроек).
    * @param[in] handle - Идентификатор устройства, полученный от urmc_open_device().
    */
    URMC_URPC_API_EXPORT result_t URMC_URPC_CALLING_CONVENTION urmc_command_stop(device_t handle);


#ifdef __cplusplus
}
#endif
#endif