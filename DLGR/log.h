/*
 * log.h
 *
 *  Created on: Mar 29, 2012
 *      Author: Administrator
 */

#ifndef LOG_H_
#define LOG_H_

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "fat/include/uart.h"
#include "fat/include/fat.h"
#include "fat/include/fat_config.h"
#include "fat/include/partition.h"
#include "fat/include/sd_raw.h"
#include "fat/include/sd_raw_config.h"
#include "log_structure.h"

#define NOT_LOGGING 	0
#define LOGGING 		1

//#define CLEAR_LOG_ON_FLUSH

uint8_t _log(uint8_t *blk);
void close_log(void);
void flush_log(void);
uint8_t open_log(const char *path);
uint8_t init_log_system(void);
void clear_log(void);
void print_log_buf(void);
char fat_init(void);

#define LOG_BUF_SIZE 512

typedef struct
{
	unsigned char data[LOG_BUF_SIZE];
	unsigned short size;
	unsigned short next;
}LOG_buf_t;

extern volatile uint32_t log_timer;
extern volatile LOG_t sys;
extern struct fat_file_struct* f_log;
extern volatile uint8_t log_status;

#define log_space()	(LOG_BUF_SIZE - log_buf.next)
#define log(blk)	_log((uint8_t *)&blk)

#endif /* LOG_H_ */
