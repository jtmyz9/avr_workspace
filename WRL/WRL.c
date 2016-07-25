/*
 * WRL.c
 *
 *  Created on: Mar 31, 2012
 *      Author: Administrator
 */


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"

#define UART1_BAUD 115200
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void)
{
	stdin = stdout = &uart_str;

	uart_init(UART1_BAUD);

	printf("Hello World\n");

	while(1);
}

