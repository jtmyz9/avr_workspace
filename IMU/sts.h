/*
 * sts.h
 *
 *  Created on: Mar 31, 2012
 *      Author: Administrator
 */

#ifndef STS_H_
#define STS_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

extern volatile uint16_t susp_trvl[4];

void sts_init(void);
void sts_get_values(void);

#endif /* STS_H_ */
