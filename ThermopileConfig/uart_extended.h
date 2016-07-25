/*
 * uart_extended.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Administrator
 */

#ifndef UART_EXTENDED_H_
#define UART_EXTENDED_H_

int uart_getstring(char* c, FILE *stream);
int uart_getint(FILE *stream);
float uart_getfloat(FILE *stream);


#endif /* UART_EXTENDED_H_ */
