/*
 * tach.c
 *
 *  Created on: Jan 10, 2012
 *      Author: Administrator
 */

#include "tach.h"

volatile uint8_t tach_flag;

void tach_leds(uint16_t leds)
{
	uint8_t i = 0;
	uint8_t tach_hld = 0;

	tach_flag = 0;

	if(tach_sts()) 	tach_hld = 1;
	else			tach_hld = 0;

	COM_PORT &= ~(_BV(TACH_CLK)|_BV(TACH_DATA));

	tach_off();

	for(i = 0;i<16;i++)
	{
		if((leds >> i) & 0x0001)
		{
			COM_PORT |= _BV(TACH_DATA);
		}
		COM_PORT |= _BV(TACH_CLK);	// Clock High
		COM_PORT &= ~_BV(TACH_DATA);	// Data low
		COM_PORT &= ~_BV(TACH_CLK);
		TACH_PORT |= _BV(TACH_LAT);
		TACH_PORT &= ~_BV(TACH_LAT);
	}

	if(tach_hld == 1)	tach_on();

	tach_flag = 1;
}

void tach_out(uint8_t val)
{
	uint16_t led_vals = 0;
	uint8_t i = 0;

	for(i = 0;i<val;i++)
	{
		led_vals = (led_vals >> 1);
		led_vals |= 0x8000;
	}

	tach_leds(led_vals);
}

void tach_init(void)
{
	TACH_DDR |= _BV(TACH_LAT)|_BV(TACH_EN);
	COM_DDR |= _BV(TACH_CLK)|_BV(TACH_DATA);

	tach_flag = 1;
}
