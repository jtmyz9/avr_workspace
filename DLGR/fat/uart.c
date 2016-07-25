
/*
 * Copyright (c) 2006-2011 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>

#include "include/uart.h"

/* some mcus have multiple uarts */
/*
#ifdef UDR1
#define UBRRH UBRR1H
#define UBRRL UBRR1L
#define UDR UDR1

#define UCSRA UCSR1A
#define UDRE UDRE1
#define RXC RXC1

#define UCSRB UCSR1B
#define RXEN RXEN1
#define TXEN TXEN1
#define RXCIE RXCIE1

#define UCSRC UCSR1C
#define URSEL 
#define UCSZ0 UCSZ10
#define UCSZ1 UCSZ11
#define UCSRC_SELECT 0
#else
#define UCSRC_SELECT (1 << URSEL)
#endif

#ifndef USART_RXC_vect
#if defined(UART1_RX_vect)
#define USART_RXC_vect UART1_RX_vect
#elif defined(UART_RX_vect)
#define USART_RXC_vect UART_RX_vect
#elif defined(USART1_RX_vect)
#define USART_RXC_vect USART1_RX_vect
#elif defined(USART_RX_vect)
#define USART_RXC_vect USART_RX_vect
#elif defined(USART1_RXC_vect)
#define USART_RXC_vect USART1_RXC_vect
#elif defined(USART_RXC_vect)
#define USART_RXC_vect USART_RXC_vect
#else
#error "Uart receive complete interrupt not defined!"
#endif
#endif
*/


#define UART_BAUD 115200
#define UBRRVAL (F_CPU/(BAUD*16)-1)
#define USE_SLEEP 1

void uart_init()
{
	//initialize UART1
	UCSR1A=_BV(U2X1);
	UCSR1B=0x18;
	UCSR1C=0x06;
	UBRR1 = (F_CPU / (8UL * UART_BAUD)) - 1;

    UCSR1B = (1 << RXEN) | (1 << TXEN);
}

void uart_putc(uint8_t c)
{
    /* wait until transmit buffer is empty */
    while(!(UCSR1A & (1 << UDRE1)));

    /* send next byte */
    UDR1 = c;
}

void uart_putc_hex(uint8_t b)
{
    /* upper nibble */
    if((b >> 4) < 0x0a)
        uart_putc((b >> 4) + '0');
    else
        uart_putc((b >> 4) - 0x0a + 'a');

    /* lower nibble */
    if((b & 0x0f) < 0x0a)
        uart_putc((b & 0x0f) + '0');
    else
        uart_putc((b & 0x0f) - 0x0a + 'a');
}

void uart_putw_hex(uint16_t w)
{
    uart_putc_hex((uint8_t) (w >> 8));
    uart_putc_hex((uint8_t) (w & 0xff));
}

void uart_putdw_hex(uint32_t dw)
{
    uart_putw_hex((uint16_t) (dw >> 16));
    uart_putw_hex((uint16_t) (dw & 0xffff));
}

void uart_putw_dec(uint16_t w)
{
    uint16_t num = 10000;
    uint8_t started = 0;

    while(num > 0)
    {
        uint8_t b = w / num;
        if(b > 0 || started || num == 1)
        {
            uart_putc('0' + b);
            started = 1;
        }
        w -= b * num;

        num /= 10;
    }
}

void uart_putdw_dec(uint32_t dw)
{
    uint32_t num = 1000000000;
    uint8_t started = 0;

    while(num > 0)
    {
        uint8_t b = dw / num;
        if(b > 0 || started || num == 1)
        {
            uart_putc('0' + b);
            started = 1;
        }
        dw -= b * num;

        num /= 10;
    }
}

void uart_puts(const char* str)
{
    while(*str)
        uart_putc(*str++);
}

void uart_puts_p(PGM_P str)
{
    while(1)
    {
        uint8_t b = pgm_read_byte_near(str++);
        if(!b)
            break;

        uart_putc(b);
    }
}

uint8_t uart_getc()
{
    /* wait until receive buffer is full */
#if USE_SLEEP
    uint8_t sreg = SREG;
    sei();

    while(!(UCSR1A & (1 << RXC1)))
        sleep_mode();

    SREG = sreg;
#else
    while(!(UCSRA & (1 << RXC)));
#endif

    uint8_t b = UDR1;
    if(b == '\r')
        b = '\n';

    return b;
}

EMPTY_INTERRUPT(USART1_RX_vect)
