/*
 * gps.c
 *
 *  Created on: Apr 14, 2012
 *      Author: Administrator
 */



#include "gps.h"
#include "util/atomic.h"



volatile struct
{
	GPS_RECV_t  bfr[GPS_RECV_BFR_SIZE];
	uint8_t idx;
}gps_recv;

void gps_init(void)
{
	uart0_init(GPS_BAUD);
	TCCR3B = 0x04;
}

void uart0_init(uint32_t baud)
{
	PORTE |= _BV(PE1)|_BV(PE0);
	DDRE |= _BV(PE1);
	//UCSR0A=_BV(U2X1);
	UCSR0B=0x98;
	UCSR0C=0x06;
	UBRR0 = (F_CPU / (16UL * baud)) - 1;
}

void uart0_tx( unsigned char data )
{
	//PORTD |= _BV(PD4);
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) ) ;
	/* Put data into buffer, sends the data */
	UDR0 = data;
	//PORTD &= ~_BV(PD4);
}

uint8_t uart0_rx(char *c)
{
	/* Wait for data to be received */
	if (UCSR0A & (1<<RXC0))
	{
		//PORTD |=_BV(PD7);
		*c = UDR0;
		return 1;
	}
	else
	{
		//PORTD &=~_BV(PD7);
		return 0;
	}
}

//volatile uint8_t cmp_str[] = "PGRMF";
//volatile uint8_t cmp_idx = 0;

volatile struct
{
	uint8_t bfr[RX_BUF_SIZE];
	uint8_t head;
	uint8_t tail;
	uint8_t num_bytes;
	uint16_t overrun;
}rx;

uint8_t gps_getc(uint8_t *c)
{
	uint8_t rtn;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(rx.num_bytes)
		{
			c[0] = rx.bfr[rx.tail];
			rx.tail = (rx.tail+1)%RX_BUF_SIZE;
			rx.num_bytes--;
			rtn = 1;
		}
		else
		{
			rtn = 0;
		}
	}
	return rtn;
}

ISR(USART0_RX_vect)
{
	uint8_t data;

	data = UDR0;

	if(data == '$')
	{
		gps.last_tick = TCNT3;
	}


	if(rx.num_bytes < RX_BUF_SIZE)
	{
		rx.bfr[rx.head] = data;
		rx.head = (rx.head+1)%RX_BUF_SIZE;
		rx.num_bytes++;
	}
	else
	{
		rx.overrun++;
	}
}
