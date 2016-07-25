/*
 * gps.h
 *
 *  Created on: Apr 3, 2012
 *      Author: Administrator
 */

#include <avr/io.h>

#define RX_STOPPED 		  	0
#define RX_WAIT_FOR_START 	1
#define RX_STARTED			2
#define RX_FINISHED			3
#define RX_FULL				4

//extern volatile struct gps;

volatile struct
{
	uint16_t week_num;
	uint32_t seconds;
	uint32_t utc_day;
	uint32_t utc_month;
	uint32_t utc_year;
	uint32_t utc_hour;
	uint32_t utc_min;
	uint32_t utc_sec;
	uint32_t utc_tenths;
	uint32_t leap_sec;
	char lat_hemi;
	uint32_t lat_deg;
	uint32_t lat_min;
	uint32_t lat_min_frac;
	char lng_hemi;
	uint32_t lng_deg;
	uint32_t lng_min;
	uint32_t lng_min_frac;
	char mode;
	uint16_t speed;
	uint16_t heading;
	uint8_t pos_dil;
	uint8_t time_dil;
	uint8_t fix_type;
	uint16_t last_tick;
}gps;

#define GPS_RECV_BFR_SIZE	4
#define RX_BUF_SIZE 		10
#define GPS_BAUD			4800

#define GPS_RX_STR_SIZE	100

typedef struct
{
	uint8_t data[RX_BUF_SIZE+1];
	uint8_t num_bytes;
	uint8_t state;
}GPS_RECV_t;

extern volatile struct gps_recv;

void gps_init(void);
void uart0_init(uint32_t baud);
void uart0_tx( unsigned char data );
uint8_t uart0_rx(char *c);
uint8_t gps_getc(uint8_t *c);
