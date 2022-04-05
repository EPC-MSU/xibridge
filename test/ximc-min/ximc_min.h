/*
* Test support data structures for ximc-xinet
*/

#ifndef XIMC_MIN_H
#define XIMC_MIN_H

// from ximc.h
typedef struct
{
	float Speed;	/**< \english Target speed. \endenglish \russian Заданная скорость. \endrussian */
	float Accel;	/**< \english Motor shaft acceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian Ускорение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). \endrussian */
	float Decel;	/**< \english Motor shaft deceleration, steps/s^2(stepper motor) or RPM/s(DC). \endenglish \russian Торможение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). \endrussian */
	float AntiplaySpeed;	/**< \english Speed in antiplay mode. \endenglish \russian Скорость в режиме антилюфта. \endrussian */
	unsigned int MoveFlags;	/**< \english \ref flagset_moveflags "Flags of the motion parameters". \endenglish \russian \ref flagset_moveflags "Флаги параметров движения". \endrussian */
} move_settings_calb_t;


#endif