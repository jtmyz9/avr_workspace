/*
 * dac.h
 *
 *  Created on: Apr 27, 2012
 *      Author: Administrator
 */

#ifndef DAC_H_
#define DAC_H_


#include <avr/io.h>
#include "i2cmaster.h"

#define DAC0 0xC0
#define DAC1 0xC2

#define DAC_FAST	0x00
#define DAC_REG		0b01000000
#define DAC_EEPROM	0b11000000

#define DAC_NORMAL		0b00000000
#define DAC_PWRDWN_1K 	0b00000010
#define DAC_PWRDWN_100K 0b00000100
#define DAC_PWRDWN_500K	0b00000110

uint8_t dac_init(void);
uint8_t dac_write(uint8_t addr,uint8_t mode,uint8_t pwr,uint16_t val);
#endif /* DAC_H_ */
