/*
 * svn_seg.c
 *
 *  Created on: Jan 11, 2012
 *      Author: Administrator
 */

#include "svn_seg.h"

unsigned char svn_seg_spi(unsigned char data)
{
	SPDR = data;

	while(!(SPSR & _BV(SPIF)));

	return(SPDR);
}

void svn_seg_write(unsigned char addr,unsigned char data)
{
	svn_seg_select();

	svn_seg_spi(addr & 0x0F);
	svn_seg_spi(data);

	svn_seg_deselect();
}

void svn_seg_put_val(unsigned int val)
{
	unsigned char dig = 0;
	for(int i = 6;i > 0;i--)
	{
		dig = val % 10;
		val = val / 10;

		if(dig == 0 && val == 0)
		{
			svn_seg_write(i,0xF);
		}
		else
		{
			svn_seg_write(i,dig);
		}
	}
}

void svn_seg_init(void)
{
	// Init IO Pins
	SVN_SEG_PORT |= _BV(SVN_SEG_CS);
	SVN_SEG_DDR |= _BV(SVN_SEG_CS);
	SPI_DDR |= _BV(SPI_MOSI)|_BV(SPI_SCK);

	// Init SPI
	SPCR = _BV(SPE)|_BV(MSTR);

	// Normal Operation
	svn_seg_write(SEG_ADDR_TST,SEG_TST_OFF);
	svn_seg_write(SEG_ADDR_SHTDWN,SEG_NORMAL);

	// Set first 6 to decode
	svn_seg_write(SEG_ADDR_DCMD,0x3F);

	// Set for 6 Digits
	svn_seg_write(SEG_ADDR_SCNLMT,0x07);

	// Set Highest Intensity
	svn_seg_write(SEG_ADDR_INT,0x0F);

	svn_seg_clr();

}

void svn_seg_clr(void)
{
	svn_seg_write(SEG_ADDR_D0,SEG_SPC);
	svn_seg_write(SEG_ADDR_D1,SEG_SPC);
	svn_seg_write(SEG_ADDR_D2,SEG_SPC);
	svn_seg_write(SEG_ADDR_D3,SEG_SPC);
	svn_seg_write(SEG_ADDR_D4,SEG_SPC);
	svn_seg_write(SEG_ADDR_D5,SEG_SPC);
	svn_seg_write(SEG_ADDR_D6,0);
	svn_seg_write(SEG_ADDR_D7,0);
}

void svn_seg_pnum(uint8_t dig,uint8_t val,uint8_t dp)
{
	svn_seg_write(SEG_ADDR_D0 + dig,val | dp);
}
