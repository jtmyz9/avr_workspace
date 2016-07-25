/*
 * log_structure.h
 *
 *  Created on: Mar 27, 2012
 *      Author: Administrator
 */

#ifndef LOG_STRUCTURE_H_
#define LOG_STRUCTURE_H_

#include "avr/io.h"
#include <stdio.h>



#define LOG_100HZ_ID	0x01
#define LOG_10HZ_ID		0x02
#define LOG_5HZ_ID		0x03
#define LOG_1HZ_ID		0x04
#define LOG_PRM_ID		0x05
#define LOG_STATUS_ID	0x06
#define LOG_EVT_ID		0x07
#define LOG_EOF			0xEE

typedef struct
{
	unsigned char blk_id;
	unsigned long time;
	unsigned short size;
}LOG_hdr_t;

typedef	struct
{
	LOG_hdr_t hdr;
	signed short fl_susp_trvl;	// Susp FL
	signed short fr_susp_trvl;	// Susp FR
	signed short rl_susp_trvl;	// Susp RL
	signed short rr_susp_trvl;	// Susp RR
	signed short acl_x;
	signed short acl_y;
	signed short acl_z;
	signed short gyro_x;
	signed short gyro_y;
	signed short gyro_z;
	unsigned short curr_tick;
}LOG_100HZ_t; //100HZ

typedef struct
{
	LOG_hdr_t hdr;
	unsigned short rpm;			// 10 Hz
	signed short tps;			// 10 Hz
	signed short oil_pres;		// 10 HZ
	signed short fbrk_pres;		// 10 HZ
	signed short rbrk_pres;		// 10 HZ
	signed short fuel_open_time;// 10 HZ
	signed short ign_angle;		// 10 HZ
	signed short map;			// 10 HZ
	signed short lambda;		// 10 HZ
	signed short fuel_slip_err;	// 10 HZ
	unsigned short fuel_comp;	// 10 HZ
	signed short timing_err;	// 10 HZ
	unsigned short timing_comp;	// 10 HZ
	signed short fl_wspd;		// 10 HZ
	signed short fr_wspd;		// 10 HZ
	signed short rl_wspd;		// 10 HZ
	signed short rr_wspd;		// 10 HZ
	signed short lng_slip;		// 10 HZ
	signed short lat_slip;		// 10 HZ
	signed short sw_angle;		// 10 HZ
}LOG_10HZ_t;

typedef struct
{
	LOG_hdr_t hdr;
	unsigned short ttemp_fli;	// 5HZ
	unsigned short ttemp_flm;	// 5HZ
	unsigned short ttemp_flo;	// 5HZ
	unsigned short ttemp_fri;	// 5HZ
	unsigned short ttemp_frm;	// 5HZ
	unsigned short ttemp_fro;	// 5HZ
	unsigned short ttemp_rli;	// 5HZ
	unsigned short ttemp_rlm;	// 5HZ
	unsigned short ttemp_rlo;	// 5HZ
	unsigned short ttemp_rri;	// 5HZ
	unsigned short ttemp_rrm;	// 5HZ
	unsigned short ttemp_rro;	// 5HZ
	unsigned long lat;			// 5HZ
	unsigned long lng;			// 5HZ
	unsigned short heading;		// 5HZ
	unsigned short speed;		// 5HZ
	unsigned short last_tick;
}LOG_5HZ_t;

typedef struct
{
	LOG_hdr_t hdr;

	signed short batt_volt;			// 1HZ
	signed short air_temp;			// 1HZ
	signed short wtr_temp;			// 1HZ
	signed short oil_temp;			// 1HZ
}LOG_1HZ_t;

typedef struct
{
	LOG_hdr_t hdr;

	// Log info
	char log_num;				// Parameter
	unsigned char driver;		// Parameter
	unsigned char date;			// Parameter
	unsigned long time;
	unsigned char hmsphr;

	// ECU info
	unsigned char ver_mjr;			// Parameter
	unsigned char ver_min;			// Parameter
	unsigned char ver_bld;			// Parameter
	unsigned char temp_type;		// Parameter
	unsigned char pres_type;		// Parameter

	// Traction Control Parameters
	unsigned short fuel_kp;			// Parameter
	unsigned short fuel_kd;			// Parameter
	unsigned short fuel_ki;			// Parameter
	unsigned short timing_kp;		// Parameter
	unsigned short timing_kd;		// Parameter
	unsigned short timing_ki;		// Parameter
	unsigned short tcs_slip_pct;	// Parameter
	unsigned short lnc_dsb_spd;		// Parameter

	// Accelerometer Parameters
	signed short x_acl_comp;		// Parameter
	signed short y_acl_comp;		// Parameter
	signed short z_acl_comp;		// Parameter
}LOG_PRM_t;

typedef struct
{
	LOG_hdr_t hdr;

	unsigned char STS_status;	// Status
	unsigned char DLG_status;	// Status
	unsigned char IMU_status;	// Status
	unsigned char STW_status;	// Status
	unsigned char WRL_status;	// Status
	unsigned char PE3_status;	// Status
	unsigned char TCS_status;	// Status
}LOG_STATUS_t;

typedef union
{
	struct
	{
		struct
		{
			// 100 Hz Data
			LOG_hdr_t hdr100;
			signed short fl_susp_trvl;	// Susp FL
			signed short fr_susp_trvl;	// Susp FR
			signed short rl_susp_trvl;	// Susp RL
			signed short rr_susp_trvl;	// Susp RR
			signed short acl_x;
			signed short acl_y;
			signed short acl_z;
			signed short gyro_x;
			signed short gyro_y;
			signed short gyro_z;
			unsigned short curr_tick;

			// 10 Hz Data
			LOG_hdr_t hdr10;
			unsigned short rpm;			// 10 Hz
			signed short tps;			// 10 Hz
			signed short oil_pres;		// 10 HZ
			signed short fbrk_pres;		// 10 HZ
			signed short rbrk_pres;		// 10 HZ
			signed short fuel_open_time;// 10 HZ
			signed short ign_angle;		// 10 HZ
			signed short map;			// 10 HZ
			signed short lambda;		// 10 HZ
			signed short fuel_slip_err;	// 10 HZ
			unsigned short fuel_comp;	// 10 HZ
			signed short timing_err;	// 10 HZ
			unsigned short timing_comp;	// 10 HZ
			signed short fl_wspd;		// 10 HZ
			signed short fr_wspd;		// 10 HZ
			signed short rl_wspd;		// 10 HZ
			signed short rr_wspd;		// 10 HZ
			signed short lng_slip;		// 10 HZ
			signed short lat_slip;		// 10 HZ
			signed short sw_angle;		// 10 HZ

			LOG_hdr_t hdr5;
			unsigned short ttemp_fli;	// 5HZ
			unsigned short ttemp_flm;	// 5HZ
			unsigned short ttemp_flo;	// 5HZ
			unsigned short ttemp_fri;	// 5HZ
			unsigned short ttemp_frm;	// 5HZ
			unsigned short ttemp_fro;	// 5HZ
			unsigned short ttemp_rli;	// 5HZ
			unsigned short ttemp_rlm;	// 5HZ
			unsigned short ttemp_rlo;	// 5HZ
			unsigned short ttemp_rri;	// 5HZ
			unsigned short ttemp_rrm;	// 5HZ
			unsigned short ttemp_rro;	// 5HZ
			unsigned long lat;				// 5HZ
			unsigned long lng;				// 5HZ
			unsigned short heading;			// 5HZ
			unsigned short speed;			// 5HZ
			unsigned short last_tick;

			LOG_hdr_t hdr1;
			signed short batt_volt;			// 1HZ
			signed short air_temp;			// 1HZ
			signed short wtr_temp;			// 1HZ
			signed short oil_temp;			// 1HZ

		}chan;

		LOG_PRM_t prm;
		LOG_STATUS_t status;
	}data;

	struct
	{
		LOG_100HZ_t 	f100;
		LOG_10HZ_t 		f10;
		LOG_5HZ_t		f5;
		LOG_1HZ_t		f1;
		LOG_PRM_t		prm;
		LOG_STATUS_t	status;
	}log;

}LOG_t;

typedef struct
{
	LOG_hdr_t hdr;
	unsigned short id;
	unsigned char data[10];
}LOG_EVT_t;

typedef struct
{
	LOG_hdr_t hdr;
	unsigned char *data;
}LOG_TEXT_t;


/* Analog Inputs
 * ----------------------------------------
 * unsigned short tcs_a1;		// 10 HZ
 * unsigned short tcs_a2;		// 10 HZ
 * unsigned short tcs_a3;		// 10 HZ
 * unsigned short tcs_a0;		// 10 HZ (SWAG)
 * signed short pe3_a1;			// 10 HZ (Fuel Comp)
 * signed short pe3_a2;			// 10 HZ (Timing Comp)
 * signed short pe3_a3;			// 10 HZ (Front BPS)
 * signed short pe3_a4;			// 10 HZ (Rear BPS)
 * signed short pe3_a5;			// Not Used
 * signed short pe3_a6;			// 10 HZ (Oil Pres)
 * signed int pe3_a7;			// (Oil Temp (use therm7))
 * signed int pe3_a8;			// Not Used (O2 Sensor (use lambda))
 * signed int therm7;			// 1HZ (Oil Temp)
 *
 */

#endif /* LOG_STRUCTURE_H_ */
