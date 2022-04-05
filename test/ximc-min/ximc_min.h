/*
* Test support data structures for ximc-xinet
*/

#ifndef XIMC_MIN_H
#define XIMC_MIN_H

// from ximc.h
typedef struct
{
	float Speed;	/**< \english Target speed. \endenglish \russian �������� ��������. \endrussian */
	float Accel;	/**< \english Motor shaft acceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian ���������, �������� � ����� � �������^2(��) ��� � �������� � ������ �� �������(DC). \endrussian */
	float Decel;	/**< \english Motor shaft deceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian ����������, �������� � ����� � �������^2(��) ��� � �������� � ������ �� �������(DC). \endrussian */
	float AntiplaySpeed;	/**< \english Speed in antiplay mode. \endenglish \russian �������� � ������ ���������. \endrussian */
	unsigned int MoveFlags;	/**< \english \ref flagset_moveflags "Flags of the motion parameters". \endenglish \russian \ref flagset_moveflags "����� ���������� ��������". \endrussian */
} move_settings_calb_t;


#endif