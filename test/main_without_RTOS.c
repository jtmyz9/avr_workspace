/*! \file
	Purpose: \n
	Interrupt-driven reception of CAN packets \n
	Polled transmission of can packets \n
	This version can be linked with can.c without using FreeRTOS.

	CAN packets with ID 0x150 will switch the LEDs at port E. \n
	CAN packets with ID 0x151 will be echoed at ID 0x152.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "general.h"
#include "assert.h"
#include "can.h"
#include "uart.h"
#include <util/delay.h>

#define UART_BAUD 115200
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

//! interrupt callback function for CAN receiver interrupt.
void receive150( CAN_packet *p, unsigned char mob) // interrupt callback
{
	(void)mob;
	PORTE^=_BV(PE6);
}

//! interrupt callback function for CAN receiver interrupt.
void receive151( CAN_packet *p, unsigned char mob) // interrupt callback
{
	(void)mob;
	p->id=0x152;
	can_tx( 14, p);
}

//! main program.

//! used to test and demonstrate the CAN rx+tx functionality \n
//! uncomment main() to use this version (without FreeRTOS)
int main (void)
//int main (void)
{
	CAN_packet pkt;


	char c;

	stdout = stdin = &uart_str;

	//initialize UART1
	UCSR1A=_BV(U2X1);
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1 = (F_CPU / (8UL * UART_BAUD)) - 1;

	printf("\nStart\n-------\n");

	pkt.id = 0x150;
	pkt.length = 0;
	DDRE|=_BV(PE6);
	DDRD|=_BV(PD7);

	printf("Initializing Can...");
	can_init();
	printf("DONE\n");

	while(1)
	{
		if(scanf("%c",&c))
		{
			printf("Transmitting...");
			can_tx(0,&pkt);

			CANPAGE = 0;

			while(!(CANSTMOB & _BV(TXOK)));

			printf("Done\n");
		}
	}
/*
	PORTD|=_BV(PD7);
	ret=prepare_rx( 0, 0x150, 0x7ff, receive150);

	ASSERT( ret==0);

	PORTD|=_BV(PD7);
	ret=prepare_rx( 1, 0x151, 0x7ff, receive151);

	ASSERT( ret==0);
	PORTD&=~_BV(PD7);
*/

	sei();	// CAN RX is interrupt driven, switch interrupts ON
	
	for(;;)
		asm("sleep");;
	return 0;
}
