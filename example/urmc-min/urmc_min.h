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
    * @name ������� �����������
    */
    //@{
    /**
    * \~english
    * Logging level - error
    * \~russian
    * ������� ����������� - ������
    */
#define LOGLEVEL_ERROR      0x01
    /**
    * \~english
    * Logging level - warning
    * \~russian
    * ������� ����������� - ��������������
    */
#define LOGLEVEL_WARNING    0x02
    /**
    * \~english
    * Logging level - info
    * \~russian
    * ������� ����������� - ����������
    */
#define LOGLEVEL_INFO       0x03
    /**
    * \~english
    * Logging level - debug
    * \~russian
    * ������� ����������� - �������
    */
#define LOGLEVEL_DEBUG      0x04
    //@}

    /**
    * \~english
    * Logging callback prototype.
    * @param loglevel - A logging level.
    * @param message - A message.
    * \~russian
    * �������� ������� ��������� ������ ��� �����������.
    * @param loglevel - ������� �����������.
    * @param message - ���������.
    */
    typedef void (URMC_URPC_CALLING_CONVENTION *urmc_logging_callback_t)(int loglevel, const wchar_t *message, void *user_data);

    /**
    * \~english
    * Simple callback for logging to stderr in wide chars.
    * @param loglevel - A logging level.
    * @param message - A message.
    * \~russian
    * ������� ������� ����������� �� stderr � ������� ��������.
    * @param loglevel - ������� �����������.
    * @param message - ���������.
    */
    URMC_URPC_API_EXPORT void URMC_URPC_CALLING_CONVENTION urmc_logging_callback_stderr_wide(int loglevel, const wchar_t *message, void *user_data);

    /**
    * \~english
    * Simple callback for logging to stderr in narrow (single byte) chars.
    * @param loglevel - A logging level.
    * @param message - A message.
    * \~russian
    * ������� ������� ����������� �� stderr � ����� (�����������) ��������.
    * @param loglevel - ������� �����������.
    * @param message - ���������.
    */
    URMC_URPC_API_EXPORT void URMC_URPC_CALLING_CONVENTION urmc_logging_callback_stderr_narrow(int loglevel, const wchar_t *message, void *user_data);

    /**
    * \~english
    * Sets a logging callback.
    * Passing NULL disables logging.
    * @param logging_callback a callback for log messages
    * \~russian
    * ������������� ������� ��������� ������ ��� �����������.
    * �������� NULL � �������� ��������� ��������� �����������.
    * @param logging_callback ��������� �� ������� ��������� ������
    */
    URMC_URPC_API_EXPORT void URMC_URPC_CALLING_CONVENTION urmc_set_logging_callback(urmc_logging_callback_t cb, void *data);
#define URMC_SETPOS_IGNORE_POSITION    0x1 /**< \~english Will not reload position in steps/microsteps if this flag is set. \~russian ���� ����������, �� ������� � ����� � ���������� �� �����������. */
#define URMC_SETPOS_IGNORE_ENCODER    0x2 /**< \~english Will not reload encoder state if this flag is set. \~russian ���� ����������, �� ������� �������� �� �����������. */

#define URMC_MOVE_STATE_MOVING    0x1 /**< \~english This flag indicates that controller is trying to move the motor. Don't use this flag for waiting of completion of the movement command. Use MVCMD_RUNNING flag from the MvCmdSts field instead. \~russian ���� ���� ����������, �� ���������� �������� ������� ����������. �� ����������� ���� ���� ��� �������� ���������� ������� ��������. ������ ���� ����������� MVCMD_RUNNING �� ���� MvCmdSts. */
#define URMC_MOVE_STATE_TARGET_SPEED    0x2 /**< \~english Target speed is reached, if flag set. \~russian ���� ��������������� ��� ���������� �������� ��������. */
#define URMC_MOVE_STATE_ANTIPLAY    0x4 /**< \~english Motor is playing compensation, if flag set. \~russian ����������� ����������� �����, ���� ���� ����������. */

#define URMC_MVCMD_NAME_BITS    0x3f /**< \~english Move command bit mask. \~russian ������� ����� �������� �������. */
#define URMC_MVCMD_UKNWN    0x0 /**< \~english Unknown command. \~russian ����������� �������. */
#define URMC_MVCMD_MOVE    0x1 /**< \~english Command move. \~russian ������� move. */
#define URMC_MVCMD_MOVR    0x2 /**< \~english Command movr. \~russian ������� movr. */
#define URMC_MVCMD_LEFT    0x3 /**< \~english Command left. \~russian ������� left. */
#define URMC_MVCMD_RIGHT    0x4 /**< \~english Command rigt. \~russian ������� rigt. */
#define URMC_MVCMD_STOP    0x5 /**< \~english Command stop. \~russian ������� stop. */
#define URMC_MVCMD_HOME    0x6 /**< \~english Command home. \~russian ������� home. */
#define URMC_MVCMD_LOFT    0x7 /**< \~english Command loft. \~russian ������� loft. */
#define URMC_MVCMD_SSTP    0x8 /**< \~english Command soft stop. \~russian ������� ������� ���������(SSTP). */
#define URMC_MVCMD_ERROR    0x40 /**< \~english Finish state (1 - move command have finished with an error, 0 - move command have finished correctly). This flags is actual when MVCMD_RUNNING signals movement finish. \~russian ��������� ���������� �������� (1 - ������� �������� ��������� � �������, 0 - ������� �������� ��������� ���������). ����� ����� ���� MVCMD_RUNNING ��������� �� ���������� ��������. */
#define URMC_MVCMD_RUNNING    0x80 /**< \~english Move command state (0 - move command have finished, 1 - move command is being executed). \~russian ��������� ������� �������� (0 - ������� �������� ���������, 1 - ������� �������� ������ �����������). */

#define URMC_PWR_STATE_UNKNOWN    0x0 /**< \~english Unknown state, should never happen. \~russian ����������� ���������, ������� �� ������ ������� ���������������. */
#define URMC_PWR_STATE_OFF    0x1 /**< \~english Motor windings are disconnected from the driver. \~russian ������� ������ ���������� � �� ����������� ���������. */
#define URMC_PWR_STATE_NORM    0x3 /**< \~english Motor windings are powered by nominal current. \~russian ������� �������� ����������� �����. */
#define URMC_PWR_STATE_REDUCT    0x4 /**< \~english Motor windings are powered by reduced current to lower power consumption. \~russian ������� ��������� �������� ����������� ����� �� �������� ��� �������� ������������ ��������. */
#define URMC_PWR_STATE_MAX    0x5 /**< \~english Motor windings are powered by maximum current driver can provide at this voltage. \~russian ������� �������� ����������� ��������� �����, ������� ����� ������ ����� ��� ������ ���������� �������. */

#define URMC_ENC_STATE_ABSENT    0x0 /**< \~english Encoder is absent. \~russian ������� �� ���������. */
#define URMC_ENC_STATE_UNKNOWN    0x1 /**< \~english Encoder state is unknown. \~russian C�������� �������� ����������. */
#define URMC_ENC_STATE_MALFUNC    0x2 /**< \~english Encoder is connected and malfunctioning. \~russian ������� ��������� � ����������. */
#define URMC_ENC_STATE_REVERS    0x3 /**< \~english Encoder is connected and operational but counts in other direction. \~russian ������� ��������� � ��������, �� ������� � ������ �������. */
#define URMC_ENC_STATE_OK    0x4 /**< \~english Encoder is connected and working properly. \~russian ������� ��������� � �������� ���������. */

#define URMC_STATE_CONTR    0x3f /**< \~english Flags of controller states. \~russian ����� ��������� �����������. */
#define URMC_STATE_IS_HOMED    0x20 /**< \~english Calibration performed \~russian ���������� ��������� */
#define URMC_STATE_SECUR    0x13ffc0 /**< \~english Flags of security. \~russian ����� ���������. */
#define URMC_STATE_ALARM    0x40 /**< \~english Controller is in alarm state indicating that something dangerous had happened. Most commands are ignored in this state. To reset the flag a STOP command must be issued. \~russian ���������� ��������� � ��������� ALARM, ���������, ��� ��������� �����-�� ������� ��������. � ��������� ALARM ��� ������� ������������ ���� �� ����� ������� ������� STOP � ��������� ALARM ��������������. */
#define URMC_STATE_CTP_ERROR    0x80 /**< \~english Control position error(is only used with stepper motor). \~russian �������� ������� �������(������������ ������ � ������� ����������). */
#define URMC_STATE_POWER_OVERHEAT    0x100 /**< \~english Power driver overheat. \~russian ����������� ������� ����� �����. */
#define URMC_STATE_OVERLOAD_POWER_VOLTAGE    0x400 /**< \~english Power voltage exceeds safe limit. \~russian ��������� ���������� �� ������� �����. */
#define URMC_STATE_WINDINGS_MALFUNC    0x4000 /**< \~english The windings are malfunc. Detection occurs by comparing current integrals over several periods. If the difference is greater than the threshold, then the decision is made to enter the Alarm state. \~russian ������� ����������. �������������� ���������� ���� ��������� ���������� ����� �� ��������� ��������. ���� ������� ������ ���������, �� ����������� ������� ����� � ��������� Alarm. */
#define URMC_STATE_BORDERS_SWAP_MISSET    0x8000 /**< \~english Engine stuck at the wrong edge. \~russian ���������� �������� �������. */
#define URMC_STATE_LOW_POWER_VOLTAGE    0x10000 /**< \~english Power voltage is lower than Low Voltage Protection limit \~russian ���������� �� ������� ����� ���� ��� ���������� Low Voltage Protection */
#define URMC_STATE_H_BRIDGE_FAULT    0x20000 /**< \~english Signal from the driver that fault happened \~russian ������� ������ �� �������� � ������������� */
#define URMC_STATE_CURRENT_MOTOR_BITS    0xc0000 /**< \~english Bits indicating the current operating motor on boards with multiple outputs for engine mounting. \~russian ����, ������������ ������� ������� ����� �� ������ � ����������� �������� ��� ����������. */
#define URMC_STATE_CURRENT_MOTOR0    0x0 /**< \~english Motor 0. \~russian ����� 0. */
#define URMC_STATE_CURRENT_MOTOR1    0x40000 /**< \~english Motor 1. \~russian ����� 1. */
#define URMC_STATE_CURRENT_MOTOR2    0x80000 /**< \~english Motor 2. \~russian ����� 2. */
#define URMC_STATE_CURRENT_MOTOR3    0xc0000 /**< \~english Motor 3. \~russian ����� 3. */
#define URMC_STATE_INCORRECT_MOTOR_SWITCH    0x100000 /**< \~english Error status when trying to switch the motor while driving. \~russian ��������� ������ ��� ������� ����������� ����� �� ����� ��������. */

#define URMC_STATE_DIG_SIGNAL    0xffff /**< \~english Flags of digital signals. \~russian ����� �������� ��������. */
#define URMC_STATE_RIGHT_EDGE    0x1 /**< \~english Engine stuck at the right edge. \~russian ���������� ������ �������. */
#define URMC_STATE_LEFT_EDGE    0x2 /**< \~english Engine stuck at the left edge. \~russian ���������� ����� �������. */
#define URMC_STATE_REV_SENSOR    0x400 /**< \~english State of Revolution sensor pin. \~russian ��������� ������ ������� ��������(���� "1", ���� ������ �������). */

#define URMC_FEEDBACK_ENCODER    0x1 /**< \~english Feedback by encoder. \~russian �������� ����� � ������� ��������. */
#define URMC_FEEDBACK_NONE    0x5 /**< \~english Feedback is absent. \~russian �������� ����� �����������. */

#define URMC_FEEDBACK_ENC_REVERSE    0x1 /**< \~english Reverse count of encoder. \~russian �������� ���� � ��������. */
#define URMC_FEEDBACK_ENC_TYPE_BITS    0xe0 /**< \~english Bits of the encoder type. \~russian ����, ���������� �� ��� ��������. */
#define URMC_FEEDBACK_ENC_TYPE_AUTO    0x0 /**< \~english Auto detect encoder type. \~russian ���������� ��� �������� �������������. */
#define URMC_FEEDBACK_ENC_TYPE_SINGLE_ENDED    0x40 /**< \~english Single ended encoder. \~russian ������������������ �������. */
#define URMC_FEEDBACK_ENC_TYPE_DIFFERENTIAL    0x80 /**< \~english Differential encoder. \~russian ���������������� �������. */
#define URMC_FEEDBACK_ENC_TYPE_POTENTIOMETER    0x20 /**< \~english Potentiometer encoder (input of the ADC) \~russian ������������ ������� (���� ���) */

#define URMC_HOME_DIR_FIRST    0x1 /**< \~english Flag defines direction of 1st motion after execution of home command. Direction is right, if set; otherwise left. \~russian ���������� ����������� ��������������� �������� ������ ����� ����������� ������� HOME. ���� ���� ���������� - ������; ����� - �����. */
#define URMC_HOME_DIR_SECOND    0x2 /**< \~english Flag defines direction of 2nd motion. Direction is right, if set; otherwise left. \~russian ���������� ����������� ������� �������� ������. ���� ���� ���������� - ������; ����� - �����. */
#define URMC_HOME_MV_SEC_EN    0x4 /**< \~english Use the second phase of calibration to the home position, if set; otherwise the second phase is skipped. \~russian ���� ���� ����������, ����������� ������ ���� ������� � �������� �������; ����� - ���� ������������. */
#define URMC_HOME_HALF_MV    0x8 /**< \~english If the flag is set, the stop signals are ignored in start of second movement the first half-turn. \~russian ���� ���� ����������, � ������ ������� �������� ������ ��� ������� ������� ���������� �������� ������������. */
#define URMC_HOME_STOP_FIRST_BITS    0x30 /**< \~english Bits of the first stop selector. \~russian ����, ���������� �� ����� ������� ���������� ������� ��������. */
#define URMC_HOME_STOP_FIRST_REV    0x10 /**< \~english First motion stops by  revolution sensor. \~russian ������ �������� ����������� �� ������� � Revolution sensor. */
#define URMC_HOME_STOP_FIRST_LIM    0x30 /**< \~english First motion stops by limit switch. \~russian ������ �������� ����������� �� ������� � ���������. */
#define URMC_HOME_STOP_SECOND_BITS    0xc0 /**< \~english Bits of the second stop selector. \~russian ����, ���������� �� ����� ������� ���������� ������� ��������. */
#define URMC_HOME_STOP_SECOND_REV    0x40 /**< \~english Second motion stops by  revolution sensor. \~russian ������ �������� ����������� �� ������� � Revolution sensor. */
#define URMC_HOME_STOP_SECOND_LIM    0xc0 /**< \~english Second motion stops by limit switch. \~russian ������ �������� ����������� �� ������� � ���������. */
#define URMC_HOME_USE_FAST    0x100 /**< \~english Use the fast algorithm of calibration to the home position, if set; otherwise the traditional algorithm. \~russian ���� ���� ����������, ������������ ������� ����� �������� �������; ����� - ������������. */

#define URMC_ENGINE_REVERSE    0x1 /**< \~english Reverse flag. It determines motor shaft rotation direction that corresponds to feedback counts increasing. If not set (default), motor shaft rotation direction under positive voltage corresponds to feedback counts increasing and vice versa. Change it if you see that positive directions on motor and feedback are opposite. \~russian ���� �������. ��������� ����������� �������� ������ � ������������ ����� ������� �������. ��� ���������� ����� (�� ���������) �������������� � ������ ������������� ���������� ����������� ������� �������. � ��������, ��� ������������� ����� ������� ������� �������������, ����� � ������ ��������� ������������� ����������. �������� ��������� �����, ���� ������������� �������� ������ ��������� ������� �������. */
#define URMC_ENGINE_CURRENT_AS_RMS    0x2 /**< \~english Engine current meaning flag. If the flag is set, then engine current value is interpreted as root mean square current value. If the flag is unset, then engine current value is interpreted as maximum amplitude value. \~russian ���� ������������� �������� ����. ���� ���� ����������, �� ���������� �������� ���� ���������������� ��� ������������������ �������� ����, ���� ���� ����, �� ���������� �������� ���� ���������������� ��� ������������ ��������� ����. */
#define URMC_ENGINE_MAX_SPEED    0x4 /**< \~english Max speed flag. If it is set, engine uses maximum speed achievable with the present engine settings as nominal speed. \~russian ���� ������������ ��������. ���� ���� ����������, �������� ���������� �� ������������ ��������. */
#define URMC_ENGINE_ANTIPLAY    0x8 /**< \~english Play compensation flag. If it set, engine makes backlash (play) compensation procedure and reach the predetermined position accurately on low speed. \~russian ����������� �����. ���� ���� ����������, ���������� ����� ��������� � �������� ����� ������ � ����� �������. ��������, ��� ������� ����� ������� �������������� �������� �� �����������, � ��� ������� ������ ���������� �������� ������� ������� �� �������� ���������� � ������������ � ��� ����� �� ������. */
#define URMC_ENGINE_ACCEL_ON    0x10 /**< \~english Acceleration enable flag. If it set, motion begins with acceleration and ends with deceleration. \~russian ���������. ���� ���� ����������, �������� ���������� � ����������. */
#define URMC_ENGINE_LIMIT_VOLT    0x20 /**< \~english Maximum motor voltage limit enable flag(is only used with DC motor). \~russian ����������� ���������� ������. ���� ���� ����������, ���������� �� ������ �������������� �������� ����������� ���������(������������ ������ � DC ����������). */
#define URMC_ENGINE_LIMIT_CURR    0x40 /**< \~english Maximum motor current limit enable flag(is only used with DC motor). \~russian ����������� ��� ������. ���� ���� ����������, ��� ����� ����� �������������� �������� ����������� ���������(������������ ������ � DC ����������). */
#define URMC_ENGINE_LIMIT_RPM    0x80 /**< \~english Maximum motor speed limit enable flag. \~russian ����������� ������� �������� ������. ���� ���� ����������, ������� �������� �������������� �������� ����������� ���������. */

#define URMC_MICROSTEP_MODE_FULL    0x1 /**< \~english Full step mode. \~russian ������������ �����. */
#define URMC_MICROSTEP_MODE_FRAC_2    0x2 /**< \~english 1/2 step mode. \~russian ������� ���� 1/2. */
#define URMC_MICROSTEP_MODE_FRAC_4    0x3 /**< \~english 1/4 step mode. \~russian ������� ���� 1/4. */
#define URMC_MICROSTEP_MODE_FRAC_8    0x4 /**< \~english 1/8 step mode. \~russian ������� ���� 1/8. */
#define URMC_MICROSTEP_MODE_FRAC_16    0x5 /**< \~english 1/16 step mode. \~russian ������� ���� 1/16. */
#define URMC_MICROSTEP_MODE_FRAC_32    0x6 /**< \~english 1/32 step mode. \~russian ������� ���� 1/32. */
#define URMC_MICROSTEP_MODE_FRAC_64    0x7 /**< \~english 1/64 step mode. \~russian ������� ���� 1/64. */
#define URMC_MICROSTEP_MODE_FRAC_128    0x8 /**< \~english 1/128 step mode. \~russian ������� ���� 1/128. */
#define URMC_MICROSTEP_MODE_FRAC_256    0x9 /**< \~english 1/256 step mode. \~russian ������� ���� 1/256. */

#define URMC_ENGINE_TYPE_NONE    0x0 /**< \~english A value that shouldn't be used. \~russian ��� �������� �� ����� ������������. */
#define URMC_ENGINE_TYPE_DC    0x1 /**< \~english DC motor. \~russian ����� ����������� ����. */
#define URMC_ENGINE_TYPE_STEP    0x3 /**< \~english Step motor. \~russian ������� �����. */
#define URMC_ENGINE_TYPE_TEST    0x4 /**< \~english Duty cycle are fixed. Used only manufacturer. \~russian ���������� � �������� �����������. ������������ ������ ��������������. */

#define URMC_POWER_REDUCT_ENABLED    0x1 /**< \~english Current reduction enabled after CurrReductDelay, if this flag is set. \~russian ���� ���� ����������, ��������� ��� �� ���������� CurrReductDelay. ����� - �� ���������. */
#define URMC_POWER_OFF_ENABLED    0x2 /**< \~english Power off enabled after PowerOffDelay, if this flag is set. \~russian ���� ���� ����������, ����� ���������� � ������� �� ���������� PowerOffDelay. ����� - �� �������. */
#define URMC_POWER_SMOOTH_CURRENT    0x4 /**< \~english Current ramp-up/down is performed smoothly during current_set_time, if this flag is set. \~russian ���� ����������, �� ����������� �������, ������ ������� ��� ��������/��������� ���� ���������� ������ �� ��������� CurrentSetTime, � ������ ����� ����������� �� ������, ������� ������� ��� ������� ���������. */

#define URMC_ALARM_ON_DRIVER_OVERHEATING    0x1 /**< \~english If this flag is set enter Alarm state on driver overheat signal. \~russian ���� ���� ����������, �� ����� � ��������� Alarm ��� ��������� ������� ������������� ��������� � ��������. ����� - ������������ ������������ �������� � ��������. */
#define URMC_LOW_UPWR_PROTECTION    0x2 /**< \~english If this flag is set turn off motor when voltage is lower than LowUpwrOff. \~russian ���� ����������, �� ��������� ������� ����� ��� ���������� ������� LowUpwrOff. */
#define URMC_H_BRIDGE_ALERT    0x4 /**< \~english If this flag is set then turn off the power unit with a signal problem in one of the transistor bridge. \~russian ���� ����������, �� ��������� ������� ����� ��� ������� ��������� � ����� �� ������������� ������. */
#define URMC_ALARM_ON_BORDERS_SWAP_MISSET    0x8 /**< \~english If this flag is set enter Alarm state on borders swap misset \~russian ���� ���� ����������, �� ����� � ��������� Alarm ��� ��������� ������� c ���������������� ���������. */
#define URMC_ALARM_FLAGS_STICKING    0x10 /**< \~english If this flag is set only a STOP command can turn all alarms to 0 \~russian ���� ���� ����������, �� ������ �� ������� STOP �������� ����� ���� ������ ALARM. */
#define URMC_USB_BREAK_RECONNECT    0x20 /**< \~english If this flag is set USB brake reconnect module will be enable \~russian ���� ���� ����������, �� ����� ������� ���� ������������ USB ��� ������� �����. */
#define URMC_ALARM_ON_WINDINGS_MALFUNC    0x40 /**< \~english If this flag is set then enter Alarm when the WINDINGS_MALFUNC state occurs. \~russian ���� ���� ���������� �� ����� Alarm ��� ������������� ���������  WINDINGS_MALFUNC */

#define URMC_BORDER_IS_ENCODER    0x1 /**< \~english Borders are fixed by predetermined encoder values, if set; borders position on limit switches, if not set. \~russian ���� ���� ����������, ������� ������������ ������������������ ������� �� ����� �������. ���� ���� �������, ������� ������������ ��������� �������������. */
#define URMC_BORDER_STOP_LEFT    0x2 /**< \~english Motor should stop on left border. \~russian ���� ���� ����������, ����� ��������������� ��� ���������� ����� �������. */
#define URMC_BORDER_STOP_RIGHT    0x4 /**< \~english Motor should stop on right border. \~russian ���� ���� ����������, ����� ��������������� ��� ���������� ������ �������. */
#define URMC_BORDERS_SWAP_MISSET_DETECTION    0x8 /**< \~english Motor should stop on both borders. Need to save motor then wrong border settings is set \~russian ���� ���� ����������, ����� ��������������� ��� ���������� ����� ������. ����� ��� �������������� ������� ��������� ��� ������������ ���������� ���������� */

#define URMC_ENDER_SWAP    0x1 /**< \~english First limit switch on the right side, if set; otherwise on the left side. \~russian ���� ���� ����������, ������ �������� ����������� ��������� ������; ����� - �����. */
#define URMC_ENDER_SW1_ACTIVE_LOW    0x2 /**< \~english 1 - Limit switch connnected to pin SW1 is triggered by a low level on pin. \~russian 1 - ��������, ������������ � ����� SW1, ��������� ����������� �� ������� ������ �� ��������. */
#define URMC_ENDER_SW2_ACTIVE_LOW    0x4 /**< \~english 1 - Limit switch connnected to pin SW2 is triggered by a low level on pin. \~russian 1 - ��������, ������������ � ����� SW2, ��������� ����������� �� ������� ������ �� ��������. */

#define URMC_CTP_ENABLED    0x1 /**< \~english Position control is enabled, if flag set. \~russian �������� ������� �������, ���� ���� ����������. */
#define URMC_CTP_BASE    0x2 /**< \~english Position control is based on revolution sensor, if this flag is set; otherwise it is based on encoder. \~russian ������ �������� ������ ��������, ���� ���� ����������; ����� - �������. */
#define URMC_CTP_ALARM_ON_ERROR    0x4 /**< \~english Set ALARM on mismatch, if flag set. \~russian ����� � ��������� ALARM ��� ����������� �������, ���� ���� ����������. */
#define URMC_REV_SENS_INV    0x8 /**< \~english Sensor is active when it 0 and invert makes active level 1. That is, if you do not invert, it is normal logic - 0 is the activation. \~russian ������ ��������� ��������, ����� �� �� 0, � �������������� ������ �������� ������� 1. �� ���� ���� �� �������������, �� ��������� ������� ������ - 0 ��� ������������/���������/�������� ���������. */
#define URMC_CTP_ERROR_CORRECTION    0x10 /**< \~english Correct errors which appear when slippage if the flag is set. It works only with the encoder. Incompatible with flag CTP_ALARM_ON_ERROR. \~russian �������������� ������, ����������� ��� ���������������, ���� ���� ����������. �������� ������ � ���������. ������������ � ������ CTP_ALARM_ON_ERROR. */

#define URMC_EEPROM_PRECEDENCE    0x1 /**< \~english If the flag is set settings from external EEPROM override controller settings. \~russian ���� ���� ����������, �� ��������� � EEPROM �������� ����� ��������� ��� �������� ����������� � �������� �� ��� ����������� EEPROM. */
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

    typedef struct
    {
        int32_t Speed[25]; /**< \~english Current speed. \~russian ������� ��������. */
        int32_t Error[25]; /**< \~english Current error. \~russian ������� ��������. */
        uint32_t Length; /**< \~english Length of actual data in buffer. \~russian ����� ����������� ������ � ������. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_measurements_t;

    typedef struct
    {
        int16_t WindingVoltageA; /**< \~english In the case step motor, the voltage across the winding A; in the case of a brushless, the voltage on the first coil, in the case of the only DC. \~russian � ������ ��, ���������� �� ������� A; � ������ ������������, ���������� �� ������ �������; � ������ DC �� ������������. */
        int16_t WindingVoltageB; /**< \~english In the case step motor, the voltage across the winding B; in case of a brushless, the voltage on the second winding, and in the case of DC is not used. \~russian � ������ ��, ���������� �� ������� B; � ������ ������������, ���������� �� ������ �������; � ������ DC �� ������������. */
        uint8_t reserved0[2]; /**< \~english  \~russian  */
        int16_t WindingCurrentA; /**< \~english In the case step motor, the current in the coil A; brushless if the current in the first coil, and in the case of a single DC. \~russian � ������ ��, ��� � ������� A; � ������ ������������, ��� � ������ �������; � ������ DC � ������������. */
        int16_t WindingCurrentB; /**< \~english In the case step motor, the current in the coil B; brushless if the current in the second coil, and in the case of DC is not used. \~russian � ������ ��, ��� � ������� B; � ������ ������������, ��� � ������ �������; � ������ DC �� ������������. */
        uint8_t reserved1[6]; /**< \~english  \~russian  */
        int16_t DutyCycle; /**< \~english Duty cycle of PWM. \~russian ����������� ���������� ���. */
        uint8_t reserved[14]; /**< \~english  \~russian  */

    } urmc_chart_data_t;

    typedef struct
    {
        int8_t Manufacturer[4]; /**< \~english Manufacturer \~russian ������������� */
        int8_t ManufacturerId[2]; /**< \~english Manufacturer id \~russian ������������� ������������� */
        int8_t ProductDescription[8]; /**< \~english Product description \~russian �������� �������� */
        uint8_t Major; /**< \~english The major number of the hardware version. \~russian �������� ����� ������ ������. */
        uint8_t Minor; /**< \~english Minor number of the hardware version. \~russian �������������� ����� ������ ������. */
        uint16_t Release; /**< \~english Number of edits this release of hardware. \~russian ����� ������ ���� ������ ������. */
        uint8_t reserved[12]; /**< \~english  \~russian  */

    } urmc_device_information_impl_t;

    typedef struct
    {
        uint32_t SerialNumber; /**< \~english Board serial number. \~russian �������� ����� �����. */

    } urmc_get_serial_number_t;

    typedef struct
    {
        uint8_t Major; /**< \~english Firmware major version number \~russian �������� ����� ������ �������� */
        uint8_t Minor; /**< \~english Firmware minor version number \~russian �������� ����� ������ �������� */
        uint16_t Release; /**< \~english Firmware release version number \~russian ����� ������ ������ �������� */

    } urmc_firmware_version_t;

    typedef struct
    {
        uint16_t A1Voltage_ADC; /**< \~english "Voltage on pin 1 winding A" raw data from ADC. \~russian "�������� ���������� �� 1 ������ ������� �" �������������� ������ � ���. */
        uint16_t A2Voltage_ADC; /**< \~english "Voltage on pin 2 winding A" raw data from ADC. \~russian "�������� ���������� �� 2 ������ ������� �" �������������� ������ � ���. */
        uint16_t B1Voltage_ADC; /**< \~english "Voltage on pin 1 winding B" raw data from ADC. \~russian "�������� ���������� �� 1 ������ ������� B" �������������� ������ � ���. */
        uint16_t B2Voltage_ADC; /**< \~english "Voltage on pin 2 winding B" raw data from ADC. \~russian "�������� ���������� �� 2 ������ ������� B" �������������� ������ � ���. */
        uint16_t SupVoltage_ADC; /**< \~english "Voltage on the top of MOSFET full bridge" raw data from ADC. \~russian "���������� ������� ������ �-�����" �������������� ������ � ���. */
        uint16_t ACurrent_ADC; /**< \~english "Winding A current" raw data from ADC. \~russian "��� ����� ������� �" �������������� ������ � ���. */
        uint16_t BCurrent_ADC; /**< \~english "Winding B current" raw data from ADC. \~russian "��� ����� ������� B" �������������� ������ � ���. */
        uint8_t reserved0[8]; /**< \~english  \~russian  */
        uint16_t L5_ADC; /**< \~english USB supply voltage after the current sense resistor, from ADC. \~russian ���������� ������� USB ����� current sense ���������, �������������� ������ � ���. */
        uint8_t reserved1[2]; /**< \~english  \~russian  */
        int16_t A1Voltage; /**< \~english "Voltage on pin 1 winding A" calibrated data. \~russian "�������� ���������� �� 1 ������ ������� �" ��������������� ������. */
        int16_t A2Voltage; /**< \~english "Voltage on pin 2 winding A" calibrated data. \~russian "�������� ���������� �� 2 ������ ������� �" ��������������� ������. */
        int16_t B1Voltage; /**< \~english "Voltage on pin 1 winding B" calibrated data. \~russian "�������� ���������� �� 1 ������ ������� B" ��������������� ������. */
        int16_t B2Voltage; /**< \~english "Voltage on pin 2 winding B" calibrated data. \~russian "�������� ���������� �� 2 ������ ������� B" ��������������� ������. */
        int16_t SupVoltage; /**< \~english "Voltage on the top of MOSFET full bridge" calibrated data. \~russian "���������� ������� ������ �-�����" ��������������� ������. */
        int16_t ACurrent; /**< \~english "Winding A current" calibrated data. \~russian "��� ����� ������� �" ��������������� ������. */
        int16_t BCurrent; /**< \~english "Winding B current" calibrated data. \~russian "��� ����� ������� B" ��������������� ������. */
        uint8_t reserved2[8]; /**< \~english  \~russian  */
        int16_t L5; /**< \~english USB supply voltage after the current sense resistor. \~russian ���������� ������� USB ����� current sense ��������� */
        uint8_t reserved3[4]; /**< \~english  \~russian  */
        int32_t R; /**< \~english Motor winding resistance in mOhms(is only used with stepper motor). \~russian ������������� ������� ���������(��� �������� ���������),  � ��� */
        int32_t L; /**< \~english Motor winding pseudo inductance in uHn(is only used with stepper motor). \~russian ������������������� ������� ���������(��� �������� ���������),  � ���� */
        uint8_t reserved[8]; /**< \~english  \~russian  */

    } urmc_analog_data_t;

    typedef struct
    {
        uint8_t DebugData[128]; /**< \~english Arbitrary debug data. \~russian ���������� ������. */
        uint8_t reserved[8]; /**< \~english  \~russian  */

    } urmc_debug_read_t;

    typedef struct
    {
        uint8_t DebugData[128]; /**< \~english Arbitrary debug data. \~russian ���������� ������. */
        uint8_t reserved[8]; /**< \~english  \~russian  */

    } urmc_debug_write_t;

    typedef struct
    {
        uint8_t Major; /**< \~english Bootloader major version number \~russian �������� ����� ������ ���������� */
        uint8_t Minor; /**< \~english Bootloader minor version number \~russian �������� ����� ������ ���������� */
        uint16_t Release; /**< \~english Bootloader release version number \~russian ����� ������ ������ ���������� */

    } urmc_bootloader_version_t;

    typedef struct
    {
        uint8_t Motor; /**< \~english Motor number which it should be switch relay on [0..1] \~russian ����� ������, �� ������� ������� ����������� ���� [0..1] */
        uint8_t reserved[15]; /**< \~english  \~russian  */

    } urmc_change_motor_t;

    typedef struct
    {
        uint16_t IPS; /**< \~english The number of encoder counts per shaft revolution. Range: 1..655535. The field is obsolete, it is recommended to write 0 to IPS and use the extended CountsPerTurn field. You may need to update the controller firmware to the latest version. \~russian ���������� �������� �������� �� ������ ����. ��������: 1..65535. ���� ��������, ������������� ���������� 0 � IPS � ������������ ����������� ���� CountsPerTurn. ����� ������������� ���������� �������������� ����������� �� ��������� ������. */
        uint8_t FeedbackType; /**< \~english Type of feedback \~russian ��� �������� ����� */
        uint8_t FeedbackFlags; /**< \~english Flags \~russian ����� */
        uint32_t CountsPerTurn; /**< \~english The number of encoder counts per shaft revolution. Range: 1..4294967295. To use the CountsPerTurn field, write 0 in the IPS field, otherwise the value from the IPS field will be used. \~russian ���������� �������� �������� �� ������ ����. ��������: 1..4294967295. ��� ������������� ���� CountsPerTurn ����� �������� 0 � ���� IPS, ����� ����� �������������� �������� �� ���� IPS. */
        uint8_t reserved[4]; /**< \~english  \~russian  */

    } urmc_feedback_settings_t;

    typedef struct
    {
        uint32_t FastHome; /**< \~english Speed used for first motion. Range: 0..100000. \~russian �������� ������� ��������. ��������: 0..100000 */
        uint8_t uFastHome; /**< \~english Part of the speed for first motion, microsteps. \~russian ������� ����� �������� ������� �������� � ����������(������������ ������ � ������� ����������). */
        uint32_t SlowHome; /**< \~english Speed used for second motion. Range: 0..100000. \~russian �������� ������� ��������. ��������: 0..100000. */
        uint8_t uSlowHome; /**< \~english Part of the speed for second motion, microsteps. \~russian ������� ����� �������� ������� �������� � ����������(������������ ������ � ������� ����������). */
        int32_t HomeDelta; /**< \~english Distance from break point. \~russian ���������� ������ �� ����� ��������. */
        int16_t uHomeDelta; /**< \~english Part of the delta distance, microsteps. Range: -255..255. \~russian ������� ����� ���������� ������ �� ����� �������� � ����������(������������ ������ � ������� ����������). ��������: -255..255. */
        uint16_t HomeFlags; /**< \~english Set of flags specify direction and stopping conditions. \~russian ����� ������, ������������ ����� ���������, ��� ����������� � ������� ��������. */
        uint8_t reserved[9]; /**< \~english  \~russian  */

    } urmc_home_settings_t;

    typedef struct
    {
        uint32_t Speed; /**< \~english Target speed (for stepper motor: steps/s, for DC: rpm). Range: 0..100000. \~russian �������� �������� (��� ��: �����/c, ��� DC: rpm). ��������: 0..100000. */
        uint8_t uSpeed; /**< \~english Target speed in microstep fractions/s. Using with stepper motor only. \~russian �������� �������� � �������� ������� ��������� � �������. ������������ ������ � ������� �������. */
        uint16_t Accel; /**< \~english Motor shaft acceleration, steps/s^2(stepper motor) or RPM/s(DC). Range: 1..65535. \~russian ���������, �������� � ����� � �������^2(��) ��� � �������� � ������ �� �������(DC). ��������: 1..65535. */
        uint16_t Decel; /**< \~english Motor shaft deceleration, steps/s^2(stepper motor) or RPM/s(DC). Range: 1..65535. \~russian ����������, �������� � ����� � �������^2(��) ��� � �������� � ������ �� �������(DC). ��������: 1..65535. */
        uint32_t AntiplaySpeed; /**< \~english Speed in antiplay mode, full steps/s(stepper motor) or RPM(DC). Range: 0..100000. \~russian �������� � ������ ���������, �������� � ����� �����/c(��) ��� � ��������/�(DC). ��������: 0..100000. */
        uint8_t uAntiplaySpeed; /**< \~english Speed in antiplay mode, 1/256 microsteps/s. Used with stepper motor only. \~russian �������� � ������ ���������, ���������� � 1/256 ���������� � �������. ������������ ������ � ������� �������. */
        uint8_t reserved[10]; /**< \~english  \~russian  */

    } urmc_move_settings_t;

    typedef struct
    {
        uint16_t NomVoltage; /**< \~english Rated voltage in tens of mV. Controller will keep the voltage drop on motor below this value if ENGINE_LIMIT_VOLT flag is set (used with DC only). \~russian ����������� ���������� ������ � �������� ��. ���������� ����� ��������� ���������� �� ������ �� ���� ������������, ���� ���������� ���� ENGINE_LIMIT_VOLT (������������ ������ � DC ����������). */
        uint16_t NomCurrent; /**< \~english Rated current. Controller will keep current consumed by motor below this value if ENGINE_LIMIT_CURR flag is set. Range: 15..8000 \~russian ����������� ��� ����� �����. ��� ��������������� ��� ������� � ����� ���� ��������� ��� DC(���� ���������� ���� ENGINE_LIMIT_CURR). ��������: 15..8000 */
        uint32_t NomSpeed; /**< \~english Nominal speed (in whole steps/s or rpm for DC and stepper motor as a master encoder). Controller will keep motor shaft RPM below this value if ENGINE_LIMIT_RPM flag is set. Range: 1..100000. \~russian ����������� �������� (� ����� �����/� ��� rpm ��� DC � �������� ��������� � ������ �������� ��������). ���������� ����� ��������� �������� ������ �� ���� �����������, ���� ���������� ���� ENGINE_LIMIT_RPM. ��������: 1..100000. */
        uint8_t uNomSpeed; /**< \~english The fractional part of a nominal speed in microsteps (is only used with stepper motor). \~russian ������������ ����� ����������� �������� ������ (������������ ������ � ������� ����������). */
        uint16_t EngineFlags; /**< \~english Set of flags specify motor shaft movement algorithm and list of limitations \~russian �����, ����������� ������� ������. */
        int16_t Antiplay; /**< \~english Number of pulses or steps for backlash (play) compensation procedure. Used if ENGINE_ANTIPLAY flag is set. \~russian ���������� ����� ��������� ��� ��������� ��������, �� ������� ���������� ����� ��������� �� �������� ������� ��� ������� � ��� � ����� � ��� �� �������. ������������, ���� ���������� ���� ENGINE_ANTIPLAY. */
        uint8_t MicrostepMode; /**< \~english Settings of microstep mode(Used with stepper motor only). \~russian ��������� ������������� ������(������������ ������ � ������� ����������). */
        uint16_t StepsPerRev; /**< \~english Number of full steps per revolution(Used with stepper motor only). Range: 1..65535. \~russian ���������� ������ ����� �� ������(������������ ������ � ������� ����������). ��������: 1..65535. */
        uint8_t reserved[12]; /**< \~english  \~russian  */

    } urmc_engine_settings_t;

    typedef struct
    {
        uint8_t EngineType; /**< \~english Engine type \~russian ��� ������ */
        uint8_t reserved[7]; /**< \~english  \~russian  */

    } urmc_entype_settings_t;

    typedef struct
    {
        uint8_t HoldCurrent; /**< \~english Current in holding regime, percent of nominal. Range: 0..100. \~russian ��� ������ � ������ ���������, � ��������� �� ������������. ��������: 0..100. */
        uint16_t CurrReductDelay; /**< \~english Time in ms from going to STOP state to reducting current. \~russian ����� � �� �� �������� � ��������� STOP �� ���������� ����. */
        uint16_t PowerOffDelay; /**< \~english Time in s from going to STOP state to turning power off. \~russian ����� � � �� �������� � ��������� STOP �� ���������� ������� ������. */
        uint16_t CurrentSetTime; /**< \~english Time in ms to reach nominal current. \~russian ����� � ��, ��������� ��� ������ ������������ ���� �� 0% �� 100%. */
        uint8_t PowerFlags; /**< \~english Flags with parameters of power control. \~russian ����� ���������� ���������� ��������. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_power_settings_t;

    typedef struct
    {
        uint16_t LowUpwrOff; /**< \~english Lower voltage limit to turn off the motor, tens of mV. \~russian ������ ����� ���������� �� ������� ����� ��� ����������, ������� ��. */
        uint8_t reserved0[2]; /**< \~english  \~russian  */
        uint16_t CriticalUpwr; /**< \~english Maximum motor voltage which triggers ALARM state, tens of mV. \~russian ������������ ���������� �� ������� �����, ���������� ��������� ALARM, ������� ��. */
        uint8_t reserved1[8]; /**< \~english  \~russian  */
        uint8_t Flags; /**< \~english Critical parameter flags. \~russian ����� ����������� ����������. */
        uint8_t reserved[7]; /**< \~english  \~russian  */

    } urmc_secure_settings_t;

    typedef struct
    {
        uint8_t BorderFlags; /**< \~english Border flags, specify types of borders and motor behaviour on borders. \~russian �����, ������������ ��� ������ � ��������� ������ ��� �� ����������. */
        uint8_t EnderFlags; /**< \~english Ender flags, specify electrical behaviour of limit switches like order and pulled positions. \~russian �����, ������������ ��������� �������� ������������. */
        int32_t LeftBorder; /**< \~english Left border position, used if BORDER_IS_ENCODER flag is set. \~russian ������� ����� �������, ������������ ���� ���������� ���� BORDER_IS_ENCODER. */
        int16_t uLeftBorder; /**< \~english Left border position in 1/256 microsteps(used with stepper motor only). Range: -255..255. \~russian ������� ����� ������� � 1/256 ����������( ������������ ������ � ������� ����������). ��������: -255..255. */
        int32_t RightBorder; /**< \~english Right border position, used if BORDER_IS_ENCODER flag is set. \~russian ������� ������ �������, ������������ ���� ���������� ���� BORDER_IS_ENCODER. */
        int16_t uRightBorder; /**< \~english Right border position in 1/256 microsteps. Used with stepper motor only. Range: -255..255. \~russian ������� ������ ������� � 1/256 ���������� (������������ ������ � ������� ����������). ��������: -255..255. */
        uint8_t reserved[6]; /**< \~english  \~russian  */

    } urmc_edges_settings_t;

    typedef struct
    {
        uint16_t KpU; /**< \~english Proportional gain for voltage PID routine \~russian ���������������� ����������� ��� ������� �� ���������� */
        uint16_t KiU; /**< \~english Integral gain for voltage PID routine \~russian ������������ ����������� ��� ������� �� ���������� */
        uint16_t KdU; /**< \~english Differential gain for voltage PID routine \~russian ���������������� ����������� ��� ������� �� ���������� */
        float Kpf; /**< \~english Proportional gain for BLDC position PID routine \~russian ���������������� ����������� ��� ������� �� ������� ��� BLDC */
        float Kif; /**< \~english Integral gain for BLDC position PID routine \~russian ������������ ����������� ��� ������� �� ������� ��� BLDC */
        float Kdf; /**< \~english Differential gain for BLDC position PID routine \~russian ���������������� ����������� ��� ������� �� ������� ��� BLDC */
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
        uint8_t CTPMinError; /**< \~english Minimum contrast steps from step motor encoder position, wich set STATE_CTP_ERROR flag. Measured in steps step motor. \~russian ����������� ������� ����� �� �� ��������� ��������, ��������������� ���� STATE_RT_ERROR. ���������� � ����� ��. */
        uint8_t CTPFlags; /**< \~english Flags. \~russian �����. */
        uint8_t reserved[10]; /**< \~english  \~russian  */

    } urmc_ctp_settings_t;

    typedef struct
    {
        uint8_t reserved[10]; /**< \~english  \~russian  */

    } urmc_uart_settings_t;

    typedef struct
    {
        int8_t ControllerName[16]; /**< \~english User conroller name. Can be set by user for his/her convinience. Max string length: 16 chars. \~russian ���������������� ��� �����������. ����� ���� ����������� ������������� ��� ��� ��������. ������������ ����� ������: 16 ��������. */
        uint8_t CtrlFlags; /**< \~english Internal controller settings. \~russian ��������� �����������. */
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
    * ��������� ���������� �� ����� \a name � ���������� ������������� ����������.
    * @param[in] name - ��� ����������.
    * ��� ���������� ����� ��� "com:port" ��� xi-net://host/serial ��� udp://host:port.
    * ��� COM ���������� "port" ��� ��� ���������� � ��.
    * �������� "com:\\.\COM3" (Windows) ��� "com:///dev/tty/ttyACM34" (Linux/Mac).
    * ��� �������� (XiNet) ���������� "host" ��� IPv4 ����� ��� ��������� ����������� ��� ������,
    * "serial" ��� �������� ����� ���������� � ����������������� �������.
    * �������� "xi-net://192.168.0.1/00001234" ��� "xi-net://hostname.com/89ABCDEF".
    * ��� ethernet ����������� com-udp "host" ��� IPv4 ����� �����������, "port" ��� ���� �����������.
    * �������� "udp://192.168.0.2:1024"
    * ���������: � ���� ������ ������� COM ���������� ����� �������������� ������ ����� ����������.
    * ���� ��� �������� ���������� ��������� ������, ����� ���������, ��� COM-���� ���� � ������� � ��� ���
    * ���������� � ������ ������ �� ������������ ������� �����������
    */
    // URMC_URPC_API_EXPORT device_t URMC_URPC_CALLING_CONVENTION urmc_open_device(const char *uri);

    /**
    * \~english
    * Get library version.
    * @param[out] lib_version - Library version.
    * \~russian
    * ������ ����������.
    * @param[out] lib_version - ������ ����������.
    */
    // URMC_URPC_API_EXPORT result_t URMC_URPC_CALLING_CONVENTION urmc_libversion(char *lib_version);

    /**
    * \~english
    * Return device identity information such as firmware version and serial number. It is useful to find your device in a list of available devices. It can be called from the firmware and bootloader.
    * @param[in] handle - Device ID, obtained by urmc_open_device() function.
    * @param[out] output - Device out data.
    * \~russian
    * ���������� ����������������� ���������� �� ����������, ����� ��� ������ ������ �������� � �������� �����. ��� ���������� ������ ��� ������ ������� ���������� ����� ������ ���������. ����� ���� ������� ��� �� ��������, ��� � �� ����������.
    * @param[in] handle - ������������� ����������, ���������� �� urmc_open_device().
    * @param[out] output - ������, ���������� � ����������.
    */
    URMC_URPC_API_EXPORT result_t URMC_URPC_CALLING_CONVENTION urmc_get_identity_information(device_t handle, urmc_get_identity_information_t *output);

    /**
    * \~english
    * Immediately stop the engine, the transition to the STOP, mode key BREAK (winding short-circuited), the regime "retention" is deactivated for DC motors, keeping current in the windings for stepper motors (with Power management settings).
    * @param[in] handle - Device ID, obtained by urmc_open_device() function.
    * \~russian
    * ����������� ��������� ���������, ������� � ��������� STOP, ����� � ������ BREAK (������� ��������� ��������), ����� "���������" ��������������� ��� DC ����������, ��������� ���� � �������� ��� ������� ���������� (� ������ Power management ��������).
    * @param[in] handle - ������������� ����������, ���������� �� urmc_open_device().
    */
    URMC_URPC_API_EXPORT result_t URMC_URPC_CALLING_CONVENTION urmc_command_stop(device_t handle);


#ifdef __cplusplus
}
#endif
#endif