/*
 * IMU.h
 *
 *  Created on: Mar 7, 2012
 *      Author: Administrator
 */

#ifndef IMU_H_
#define IMU_H_

#include "i2cmaster.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

//#include "uart.h"
#include "acl.h"
//#include "ZOUnet.h"			//think i dont need this
//#include "can.h"
#include "systimer.h"
#include "tmp_sensor.h"
#include "sts.h"
#include "gyro.h"
#include "can_lib.h"

#define UART1_BAUD 115200
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

#define GYR_RD	0xD0
#define GYR_WR	0xD1

#define ch2word(msb,lsb)	((((uint16_t)(msb) & 0x00FF) << 8) + lsb)
#define ch2dblword(b3,b2,b1,b0)	((((uint32_t)(b3) & 0x000000FF) << 24) +\
								(((uint32_t)(b2) & 0x000000FF) << 16) +\
								(((uint32_t)(b1) & 0x000000FF) << 8) + b0)

//uint8_t ProcessCanMsg(CAN_packet *pkt);
//void InitCanMsgs(void);
void handle_10ms_tick(void);
//volatile CAN_packet imu2,sts3,sts4,sts5,sts2;



#endif /* IMU_H_ */
