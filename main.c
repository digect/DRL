/*
 * nissan.c
 *
 * Created: 02.03.2017 13:06:33
 * Author : Вячеслав Потоцкий
 */
#include "main.h"
#include <avr/sfr_defs.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdbool.h>

uint8_t flag;
uint8_t powerLevel			= 128;
bool	 speedEnable		= false;
uint16_t counterDelaySec	= DELAY_POWER_OFF_SEC;
uint16_t count_for_seconds	= COUNT_FOR_SEC;
uint16_t positive			= 0;
int main (void)
{
	Init ();
	while (true){
		wdt_reset();
		// Установка уровня мощности в зависимости от установленной перемычки
		if (bit_is_set(PINB, JUMPER_POWER)){
			powerLevel = POWER_LEVEL_WITHOUT_JUMPER;
		}else{
			powerLevel = POWER_LEVEL_WITH_JUMPER;
		}
		// Чтение показаний датчика
		sensorRead();
		// Если устройство отключено, (есть сигнал отключения - габариты горят)
	    if (!isEnable()){
					// Убрать мощность с нагрузки
					lightEnable (false);
					continue;
	    }
		// Подать или снять мощность в зависимости от состояния переменной.
	 		lightEnable (speedEnable);
	}
}

void Init (){
	flag	= 0;
	DDRB  	|= (1 << MOSFET) | (1 << POWER_ENABLE) ;			//  Указанные пины на выход
	MCUCR	&= ~(1 << PUD);
	PORTB   |= (1 << JUMPER_POWER);
	wdt_reset();
	wdt_enable (WDTO_2S);
	initFastPWM();
}
void seconds(){
	PORTB ^= 1 << POWER_ENABLE;
	// Восстановление количество итерация переменной чтобы попасть в эту подпрограммы
	count_for_seconds = COUNT_FOR_SEC;
	// Если количество подсчитанных за единицу времени положительных измерений, считать что машина едет.
	if (positive>POSITIVE_COUNT_FOR_ENABLE) {
			speedEnable = true;						//	Машина едет (глобальная переменная)
			counterDelaySec = DELAY_POWER_OFF_SEC;	// Восстановление счетчика отключений (сколько раз надо сюда попасть чтобы снять нагрузку)
	}
	else
	{
		if (!counterDelaySec)		// Если счетчик количества времени до отключение пуст
			speedEnable = false;	//	СНЯТЬ НАГРУЗКУ
		else
			counterDelaySec--;		// Уменьшить счетчик количества времени
	}
	positive = 0;
}
void sensorRead(){
	_delay_us(UPDATE_READ_LEVEL_US);	// Задержка при опросе (цикличность опроса)
	count_for_seconds--;				// Уменьшить счетчик секунд и если он пуст то выполнить подпрограмму
	if (!count_for_seconds) seconds();	// выполняемую раз в секунду
	if (bit_is_set(PINB, PIN_SENSOR))	// Измеряем уровень на входе
		positive++;						// В случае успеха инкрементируем счетчик.
}
bool isEnable   (void)
{
	bool ret = bit_is_clear(PINB, PIN_DISENBL);
	if (!ret){
		positive = 0;
		speedEnable = false;
		count_for_seconds = COUNT_FOR_SEC;
		counterDelaySec = DELAY_POWER_OFF_SEC;
		speedEnable = false;
	}
    return ret;
}
void lightEnable (bool enable )
{
    if (enable){
			if (bit_is_clear(flag,LIGHT_ON)){
				flag  |= (1 << LIGHT_ON);
			//	PORTB |= (1 << POWER_ENABLE);
				power(TIME_POWER_SEC,powerLevel);
			}
    } else{
			if (bit_is_set(flag,LIGHT_ON)){
				flag &= ~(1 << LIGHT_ON);
			//	PORTB &= ~(1 << POWER_ENABLE);
				power(TIME_POWER_SEC,0);
			}
    }
}
void initFastPWM ()
{
	TCCR0A  = _BV(WGM00)  | _BV(WGM01) |     // set Fast PWM
	_BV(COM0A1);							 // не инвертированный;
	TCCR0B  = _BV (CS02) ; 				     // CLKi =		9600 / 256  =	37,5 kHz   _BV (CS00) |
	OCR0A = 0x01;
	TCCR0A &=~_BV(COM0A1);					// Отключить таймер до тех пор пока его не включу явно


}
void power (byte time,byte value){
	if (value==OCR0A) return;
	bool vec = true;
	uint8_t count = 0;
	// Загрузка начального значения в зависимости от типа
	if (value < OCR0A){
			vec = false;
			// уменьшаем
			count = OCR0A - value;
	} else{
			vec = true;
			count = value - OCR0A ;
	}
	uint8_t iterat = (((uint16_t) time *1000)+count)/count;


	TCCR0A |=_BV(COM0A1);					// Включить неинвертированный ШИМ
	for (byte i=0; i<count;i++)
	{
		delay(iterat);
		if (vec)
		OCR0A++;
			else
		OCR0A--;

	}

		if (!OCR0A){
			TCCR0A &=~_BV(COM0A1);					// Отключить Таймер от выхода.
			PORTB &=~_BV(MOSFET);					// Отключаем порт полностью.
			return;
		}
		if (OCR0A==0xFF){
			TCCR0A &=~_BV(COM0A1);					// Отключить Таймер от выхода.
			PORTB |=_BV(MOSFET);					// Включаем порт полностью.

		}
}
void delay (byte value){
	wdt_reset();
	for (byte i = 0; i<value; i++){
		_delay_ms(1);
	}
}
void delaysec (byte value){
	for (byte i = 0; i<value; i++){
		wdt_reset();
		_delay_ms(1000);
	}
	wdt_reset();
}
