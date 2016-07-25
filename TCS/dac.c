/*
 * dac.c
 *
 *  Created on: Apr 27, 2012
 *      Author: Administrator
 */
#include "dac.h"

uint8_t dac_init(void)
{
	i2c_init();
	if(!dac_write(DAC0,DAC_FAST,DAC_NORMAL,0))
	{
		return 0;
	}
	if(!dac_write(DAC1,DAC_FAST,DAC_NORMAL,0))
	{
		return 0;
	}

	return 1;
}

uint8_t dac_write(uint8_t addr,uint8_t mode,uint8_t pwr,uint16_t val)
{
	if(i2c_start(addr|I2C_WRITE))
	{
		return 0;
	}

	if((mode == DAC_EEPROM) || (mode == DAC_REG))
	{
		i2c_write(mode|pwr);
		i2c_write(0x00FF & (val >> 4));
		i2c_write(0x00FF & (val << 4));
	}
	else
	{
		i2c_write(mode|(pwr << 3)|(0x00FF & (val >> 8)));
		i2c_write(0x00FF & val);
		i2c_stop();
	}

	return 1;
}