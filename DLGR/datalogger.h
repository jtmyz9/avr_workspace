/*
 * datalogger.h
 *
 *  Created on: Jan 7, 2012
 *      Author: Administrator
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "fat/include/uart.h"
#include "fat/include/fat.h"
#include "fat/include/fat_config.h"
#include "fat/include/partition.h"
#include "fat/include/sd_raw.h"
#include "fat/include/sd_raw_config.h"
#include "can.h"
#include "systimer.h"
#include "log_structure.h"
#include "log.h"





volatile uint32_t glb_tmr = 0;



void handle_10ms_tick(void);
uint8_t ProcessCanMsg(CAN_packet *pkt);

#endif /* DATALOGGER_H_ */
