/*
 * tmp_sensor.h
 *
 *  Created on: Mar 24, 2012
 *      Author: Administrator
 */

#ifndef TMP_SENSOR_H_
#define TMP_SENSOR_H_

#include <avr/io.h>
#include "i2cmaster.h"

// Temperature index
#define TMP_FLI	0
#define TMP_FLM	1
#define TMP_FLO	2
#define TMP_FRI	3
#define TMP_FRM	4
#define TMP_FRO	5
#define TMP_RLI	6
#define TMP_RLM	7
#define TMP_RLO	8
#define TMP_RRI	9
#define TMP_RRM	10
#define TMP_RRO	11

// Sensor I2C Addresses
#define TMP_FLI_ADDR	0x28
#define TMP_FLM_ADDR	0x14
#define TMP_FLO_ADDR	0x2A
//#define TMP_FRO_ADDR	0x16
//#define TMP_FRM_ADDR	0x18
//#define TMP_FRO_ADDR	0x2C
#define TMP_FRI_ADDR	0x18
#define TMP_FRM_ADDR	0x2C
#define TMP_FRO_ADDR	0x16

//I=0x18
//M=0x2C
//O=0x16

#define TMP_RLI_ADDR	0x3C
#define TMP_RLM_ADDR	0x54
#define TMP_RLO_ADDR	0x3E
#define TMP_RRI_ADDR	0x50
#define TMP_RRM_ADDR	0x52
#define TMP_RRO_ADDR	0x40

//#define TMP_RLI_ADDR	0
//#define TMP_RLM_ADDR	0
//#define TMP_RLO_ADDR	0
//#define TMP_RRI_ADDR	0
//#define TMP_RRM_ADDR	0
//#define TMP_RRO_ADDR	0


// Temp Sensor registers
#define TMPSNSR_VAL 0x07

extern volatile uint8_t temps[12];

uint8_t temp_init(uint8_t addr);
uint8_t temp_get_value(uint8_t addr,uint8_t idx);

#endif /* TMP_SENSOR_H_ */
