/*
 * glcd.c
 *
 *  Created on: Jan 10, 2012
 *      Author: Administrator
 */

#include "glcd.h"

#define GLCD_BAUD	115200UL
#define GLCD_INIT_TIME 3000 //ms

void glcd_puts0(const char *s)
{
	uint8_t i = 0;
	while(s[i]!=0)
	{
		uart0_tx(s[i]);
		i++;
	}
}

void uart0_init(uint32_t baud)
{
	PORTE |= _BV(PE1)|_BV(PE0);
	DDRE |= _BV(PE1);
	UCSR0A=_BV(U2X1);
	UCSR0B=0x18;
	UCSR0C=0x06;
	UBRR0 = (F_CPU / (8UL * baud)) - 1;
}

void uart0_tx( unsigned char data )
{
	PORTD |= _BV(PD4);
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) ) ;
	/* Put data into buffer, sends the data */
	UDR0 = data;
	PORTD &= ~_BV(PD4);
}

uint8_t uart0_rx(char *c)
{
	/* Wait for data to be received */
	if (UCSR0A & (1<<RXC0))
	{
		PORTD |=_BV(PD7);
		*c = UDR0;
		return 1;
	}
	else
	{
		PORTD &=~_BV(PD7);
		return 0;
	}
}

uint16_t clear_rx_buf(void)
{
	uint16_t a = UDR0;

	a = a<<8;
	return a + (0x00FF & ((uint16_t)UDR0));
}

void glcd_reset(void)
{
	GLCD_RST_PORT &= ~_BV(GLCD_RST);
	GLCD_RST_DDR |= _BV(GLCD_RST);
	_delay_us(100);
	GLCD_RST_DDR &= ~_BV(GLCD_RST);
	_delay_us(100);
}

void glcd_init(void)
{
	glcd_reset();
	_delay_ms(GLCD_INIT_TIME);
	uart0_init(GLCD_BAUD);
}

int8_t glcd_set(uint8_t reg,int16_t val)
{
	uint8_t resp;
	char s[30];

	clear_rx_buf();

	sprintf(s,"/S%02u0%i\n",reg,val);
	glcd_puts0(s);

	resp = glcd_resp(650000,s);

	if(resp)
	{
		if(resp == 'Y')
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return 0;
	}
}

uint8_t glcd_resp(uint32_t timeout,char *s)
{
	uint8_t resp;
	char c = 0;
	s[0]=0;
	uint8_t idx = 0;

	for(;timeout >0;timeout--)
	{
		resp = uart0_rx(&c);
		if(resp)
		{
			s[idx] = c;
			if((c == '\n') || (c == '\r'))
			{
				s[idx+1] = 0;
				PORTD &=~_BV(PD7);
				return s[0];
			}
			else
			{
				idx++;
			}
		}
		_delay_us(1);
	}

	PORTD &=~_BV(PD7);

	return 0;
}

int8_t glcd_update(void)
{
	char s[5] = "/U\n";
	uint8_t resp;

	clear_rx_buf();

	glcd_puts0(s);

	resp = glcd_resp(650000,s);

	if(resp)
	{
		if(resp == 'Y')
		{
			_delay_ms(100);
			return 1;
		}
		else
		{
			_delay_ms(100);
			return -1;
		}
	}
	else
	{
		return 0;
	}


}

















