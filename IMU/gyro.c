/*
 * gyro.c
 *
 *  Created on: Mar 26, 2012
 *      Author: Administrator
 */

#include "gyro.h"
#include <util/atomic.h>

int16_t gyro_x = 0;
int16_t gyro_y = 0;
int16_t gyro_z = 0;

uint8_t gyro_init(void)
{
	if(gyro_write(GYRO_DLPF_FS,0x19))
	{
		return 1;
	}

	return 0;
}

uint8_t gyro_write(uint8_t reg,uint8_t val)
{
	if(i2c_start(GYRO_ADDR+I2C_WRITE))
	{
		i2c_stop();
		return 1;
	}

	i2c_write(reg);
	i2c_write(val);
	i2c_stop();

	return 0;
}

uint8_t gyro_read(uint8_t start_reg,uint8_t *buf,uint8_t num)
{
	uint8_t i;

	if(i2c_start(GYRO_ADDR+I2C_WRITE))
	{
		return 1;
	}

	i2c_write(start_reg);
	if(i2c_rep_start(GYRO_ADDR+I2C_READ))
	{
		i2c_stop();
		return 2;
	}
	for(i = 0;i < num-1;i++)
	{
		buf[i] = i2c_read(ACK);
	}

	buf[num-1] = i2c_read(NACK);

	i2c_stop();

	return 0;
}

void gyro_get_values(void)
{
	uint8_t data[7];
	int32_t x,y,z;

	gyro_read(GYRO_XOUT_H,data,7);

	x = ((((int16_t)(data[0])) << 8) &0xFF00) + (((int16_t)data[1]) & 0x00FF);
	y = ((((int16_t)(data[2])) << 8) &0xFF00) + (((int16_t)data[3]) & 0x00FF);
	z = ((((int16_t)(data[4])) << 8) &0xFF00) + (((int16_t)data[5]) & 0x00FF);

	x = (GYRO_SCALE_NUM*((int32_t)(x)))/GYRO_SCALE_DEN;
	y = (GYRO_SCALE_NUM*((int32_t)(y)))/GYRO_SCALE_DEN;
	z = (GYRO_SCALE_NUM*((int32_t)(z)))/GYRO_SCALE_DEN;

	// Guarantee atomic write
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		gyro_x = x;
		gyro_y = y;
		gyro_z = z;
	}




}








