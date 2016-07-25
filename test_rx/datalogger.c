/*
 * datalogger.c
 *
 *  Created on: Dec 30, 2011
 *      Author: Mark
 */


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

#define UART_BAUD 115200

int main()
{
	stdout = stdin = &uart_str;

	//initialize UART1
	UCSR1A=_BV(U2X1);
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1 = (F_CPU / (8UL * UART_BAUD)) - 1;

	//Initialize CAN
	printf("Setting up CAN Timing\n");

	while(1);

	/* Timing */
	CANBT1 = 0x06;
	CANBT2 = 0x0C;
	CANBT3 = 0x37;

	printf("Enabling CAN and listening mode\n");
	CANGCON |= _BV(ENASTB);

	printf("Waiting for CAN to Enable\n");
	while(!(CANGSTA & _BV(ENFG)));

	printf("Setting up MOB0\n");

	// Set MOb page to 0
	CANPAGE = 0;

	CANIDT4 = 0;
	CANIDT3 = 0;
	CANIDT2 = 0;
	CANIDT1 = 0xFF;

	CANIDM1 = 0;
	CANIDM2 = 0;
	CANIDM3 = 0;
	CANIDM4 = 0;

	CANMSG = 0b10101010;

	printf("Setting up MOb 1\n");
	// Set MOb page to 1
	CANPAGE = (1<<4);

	CANIDM1 = 0;
	CANIDM2 = 0;
	CANIDM3 = 0;
	CANIDM4 = 0;

	printf("Enabling reception on MOb1\n");
	CANCDMOB = _BV(CONMOB1)|_BV(DLC0);

	CANPAGE = 0;
	// Setup control register
	printf("Sending Message with MOb0\n");
	CANCDMOB = _BV(CONMOB0)|_BV(DLC0);

	printf("Waiting for transmission\n");
	while(!(CANSTMOB  & _BV(TXOK)))
	{
		printf("CANSTMOB: 0x%02X\n",CANSTMOB);
		_delay_ms(500);

	}
	printf("Transmission Complete!");

	while(1);
}
