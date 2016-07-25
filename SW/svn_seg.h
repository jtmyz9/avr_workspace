/*
 * svn_seg.h
 *
 *  Created on: Jan 11, 2012
 *      Author: Administrator
 */

#ifndef SEG_H_
#define SEG_H_

#include <avr/io.h>

#define	SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define SPI_SCK		PB1
#define SPI_MOSI	PB2
#define SPI_MISO	PB3

#define SEG_ADDR_NOP	0x00
#define SEG_ADDR_D0		0x01
#define SEG_ADDR_D1		0x02
#define SEG_ADDR_D2		0x03
#define SEG_ADDR_D3		0x04
#define SEG_ADDR_D4		0x05
#define SEG_ADDR_D5		0x06
#define SEG_ADDR_D6		0x07
#define SEG_ADDR_D7		0x08
#define SEG_ADDR_DCMD	0x09
#define SEG_ADDR_INT	0x0A
#define SEG_ADDR_SCNLMT	0x0B
#define SEG_ADDR_SHTDWN	0x0C
	#define SEG_SHTDWN	0x00
	#define SEG_NORMAL 	0x01
#define SEG_ADDR_TST	0x0F
	#define SEG_TST_OFF	0x00
	#define SEG_TST_ON	0x01

#define	SVN_SEG_DDR		DDRB
#define SVN_SEG_PORT	PORTB
#define SVN_SEG_CS	PB0

#define SEG_DP 		0x80
#define SEG_NO_DP 	0x00
#define SEG_DASH 	0x0A
#define SEG_E		0x0B
#define SEG_H		0x0C
#define SEG_L		0x0D
#define SEG_P		0x0E
#define SEG_SPC		0x0F
#define SEG_CLN_TOP	0x40
#define SEG_CLN_BTM	0x20
#define SEG_APS		0x10

#define svn_seg_select()	(SVN_SEG_PORT &= ~_BV(SVN_SEG_CS))
#define svn_seg_deselect()	(SVN_SEG_PORT |= _BV(SVN_SEG_CS))

void svn_seg_clr(void);
void svn_seg_write(unsigned char addr,unsigned char data);
void svn_seg_put_val(unsigned int val);
unsigned char svn_seg_spi(unsigned char data);
void svn_seg_init(void);
void svn_seg_pnum(uint8_t dig,uint8_t val,uint8_t dp);



#endif /* SEG_H_ */
