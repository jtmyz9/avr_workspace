/*
 * tach.h
 *
 *  Created on: Jan 10, 2012
 *      Author: Administrator
 */

#ifndef TACH_H_
#define TACH_H_

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>



#define COM_DDR		DDRF
#define COM_PORT	PORTF
#define TACH_DDR	DDRB
#define TACH_PORT	PORTB
#define TACH_LAT	PB4
#define TACH_EN		PB5
#define TACH_CLK	PF0
#define TACH_DATA	PF2

void tach_leds(uint16_t leds);

void tach_out(uint8_t val);
void tach_init(void);

#define tach_on()	(TACH_PORT &=~_BV(TACH_EN))
#define tach_off() (TACH_PORT |= _BV(TACH_EN))
#define tach_sts()	(!(TACH_PORT & _BV(TACH_EN)))

#endif /* TACH_H_ */
