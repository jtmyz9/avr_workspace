/*
 * tmp_sensor.c
 *
 *  Created on: Mar 24, 2012
 *      Author: Administrator
 */

#include <util/atomic.h>
#include "tmp_sensor.h"

volatile uint8_t temps[12] = {255,255,255,255,255,255,255,255,255,255,255,255};

uint8_t temp_init(uint8_t addr)
{
	if(addr == 0) return 1;

	if(i2c_start(addr+I2C_READ))
	{
		i2c_stop();
		return 1;
	}
	i2c_stop();
	return 0;
}

uint8_t temp_get_value(uint8_t addr,uint8_t idx)
{
	uint8_t t0,t1,pec;

	int32_t t;

	if(addr == 0)	return(1);

	if(i2c_start(addr+I2C_WRITE))
	{
		i2c_stop();
		return(1);
	}

	i2c_write(TMPSNSR_VAL);

	if(i2c_rep_start(addr+I2C_READ))
	{
		i2c_stop();
		return 2;
	}

	t0 = i2c_read(ACK);
	t1 = i2c_read(ACK);
	pec = i2c_read(ACK);
	i2c_stop();

	if(t1 & 0x80) // Error bit set
	{
		return 3;
	}
	t = (int16_t)(((uint16_t)(t1 & 0x7F) << 8) + (uint16_t)t0);

	t = (9*(t/50 - 273))/5 + 32;

	if(t < 0)	t = 0;
	if(t > 255) t = 255;

	// Guarantee atomic write
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		temps[idx] = (uint8_t) (t & 0x000000FF);
	}

	return 0;
}
