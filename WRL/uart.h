/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo, UART declarations
 *
 * $Id: uart.h,v 1.1 2005/12/28 21:38:59 joerg_wunsch Exp $
 */
#ifndef UART_H
#define UART_H

#include <avr/pgmspace.h>
#include <stdio.h>



/*
 * Perform UART startup initialization.
 */
void uart_init(uint32_t baud);

/*
 * Send one character to the UART.
 */
int	uart_putchar(char c, FILE *stream);

/*
 * Size of internal line buffer used by uart_getchar().
 */
#define RX_BUFSIZE 80

void USART_Transmit( unsigned char data );
uint8_t uart_Receive( void );

/*
 * Receive one character from the UART.  The actual reception is
 * line-buffered, and one character is returned from the buffer at
 * each invokation.
 */
int	uart_getchar(FILE *stream);


#endif



