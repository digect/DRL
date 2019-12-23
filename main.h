/*
 * main.h
 *
 * Created: 02.03.2017 13:13:28
 *  Author: Вячеслав Потоцкий
 */


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 9600000UL
#include "type.h"
#include <util/delay.h>

#define	  LIGHT_ON					0

/*	Настройка опроса входа датчика		*/
#define   UPDATE_READ_LEVEL_US		100  // Частота опроса входа, по умолчания 50 мкс в два раза выше чем максимальня частота.
#define	  PERCENT_POSITIVE			5	// Процент положительных измерений
#define   COUNT_FOR_SEC				1000000/UPDATE_READ_LEVEL_US/2  // Количество измерений в секунду (Полупериод)!
#define   POSITIVE_COUNT_FOR_ENABLE (COUNT_FOR_SEC/100*PERCENT_POSITIVE)	// Количество положительных измерений
/*	Время в течении которого увеличивается или уменьшается мощность в нагрузке.
	Плавность зажигания ламп.	*/
#define   TIME_POWER_SEC			2
/*	Время в секундах, в течении которых при отсутствия сигнала движения автомобиля,
	мощность продолжает подаваться в нагрузку.	*/
#define	  DELAY_POWER_OFF_SEC		3
/*	Уровень мощности при установленной перемычке, от 0 до 0xFF*/
#define  POWER_LEVEL_WITH_JUMPER	0xC0
/*	Уровень мощности при без перемычки, от 0 до 0xFF*/
#define  POWER_LEVEL_WITHOUT_JUMPER	0xFF


void Init 				(void);
bool isEnable			(void);
bool isSpeed			(void);
void sensorRead 		(void);
void seconds			(void);
void lightEnable		(bool enable);
void initFastPWM		(void);
void power				(byte time,byte value);    /*	Подать мощность 0 - 255 в течении time				*/
void delay				(byte value);			   /*	Задержка мс (0-255)									*/
void delaysec			(byte sec);


/*===========================================        PORTB  	==================================================  */

#define MOSFET			PB0	    /*	OUTPUT	MOSFET	         На затвор транзистора					             */
#define PIN_SENSOR		PB4	    /*	INPUT	SPEED_BIT        Датчик скорости				                     */
#define PIN_DISENBL		PB3	    /*	INPUT	enable/disable 	 Габариты							                 */
#define JUMPER_POWER	PB2	    /*	INPUT	Перемычка отвечающая за уровень мощности на выходе				     */
#define POWER_ENABLE	PB1		/*	Выход   При активации мощности в нагрузке подается 1 (Открытый коллектор)    */


#endif /* MAIN_H_ */
