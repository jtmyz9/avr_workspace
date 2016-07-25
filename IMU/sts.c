/*
 * sts.c
 *
 *  Created on: Mar 31, 2012
 *      Author: Administrator
 */


#include "sts.h"
#include <stdio.h>
#include <stdlib.h>

volatile uint8_t curr_chan = 0;
volatile uint16_t susp_trvl[4] = {0,0,0,0};

void sts_init(void)
{
	ADMUX |= _BV(REFS0);
	ADCSRA |= _BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0);
}

void sts_get_values(void)
{
	//Channel 0
	uint8_t i;

	for(i = 0;i<4;i++)
	{
		ADMUX &= 0xE0;
		ADMUX |= i;
		ADCSRA |=_BV(ADSC);
		while(!(ADCSRA & _BV(ADIF)));
		susp_trvl[i] = ADC;
		susp_trvl[i] &= 0b0000001111111111;
		ADCSRA |= _BV(ADIF);
	}
}
