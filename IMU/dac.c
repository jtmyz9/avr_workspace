/*
 * dac.c
 *
 *  Created on: Jan 5, 2012
 *      Author: Administrator
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "i2c.h"

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

#define UART_BAUD 115200

#define DAC0_RD 0xC1
#define DAC0_WR 0xC0
#define DAC1_RD 0xC3
#define DAC1_WR 0xC2

int main()
{
	unsigned char resp = 0;
	unsigned char i = 0;

	DDRD|=_BV(PB4)|_BV(PB3)|_BV(PB2)|_BV(PB1)|_BV(PB0);
	PORTD|=_BV(PB4)|_BV(PB3)|_BV(PB2)|_BV(PB1)|_BV(PB0);

	//initialize UART1
	stdout = stdin = &uart_str;
	UCSR1A=_BV(U2X1);
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1 = (F_CPU / (8UL * UART_BAUD)) - 1;

	PORTD |= _BV(PD0)|_BV(PD1);
	PORTE |= _BV(PE5);

	printf("\n>------| START |------<\n\n");

	printf("Initializing I2C...");
	i2c_init();
	printf("DONE\n");

	printf("Sending Address\n");

	resp = i2c_start();
	resp = i2c_sendAddress(0x3C);
	if(resp)
	{
		printf("SendAddress Failed\n");
	}

	i2c_stop();

	printf("Done\n");

	while(1);
}
