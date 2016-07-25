/*
 * glcd.h
 *
 *  Created on: Jan 10, 2012
 *      Author: Administrator
 */

#ifndef GLCD_H_
#define GLCD_H_

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#define rgb(r,g,b) 	(((uint16_t)(r & 0x001F)) << 11) 	|\
					(((uint16_t)(g & 0x003F)) << 5) 	|\
					((uint16_t)(b & 0x001F))

#define GLCD_RST_DDR	DDRE
#define GLCD_RST_PORT	PORTE
#define GLCD_RST		PE2

int8_t glcd_update(void);
void glcd_init(void);
uint16_t clear_rx_buf(void);
void glcd_reset(void);
uint8_t uart0_rx(char *c);
void uart0_tx( unsigned char data );
void uart0_init(uint32_t baud);
int8_t glcd_set(uint8_t reg,int16_t val);
uint8_t glcd_resp(uint32_t timeout,char *s);
#endif /* GLCD_H_ */
