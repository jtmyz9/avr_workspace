/*
 * ui.h
 *
 *  Created on: Jan 10, 2012
 *      Author: Administrator
 */

#ifndef UI_H_
#define UI_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#include "ui_defs.h"
#include "tach.h"
#include "glcd.h"
#include "svn_seg.h"
#include "ui_pages.h"

#define uart0_puts(s) for(i = 0;i<strlen(s);i++)	uart0_tx(s[i]);

extern volatile uint8_t tach_flag;
extern volatile uint8_t pot_disp;

#define RPM_MULT 1000;
#define RPM_ON 1
#define RPM_OFF 0

volatile struct
{
	unsigned char PE3_status;
	unsigned int rpm;
	signed int tps;
	signed int fuel_open_time;
	signed int ign_angle;
	signed int barometer;
	signed int map;
	signed int lambda;
	signed int pe3_a1;
	signed int pe3_a2;
	signed int pe3_a3;
	signed int pe3_a4;
	signed int pe3_a5;
	signed int pe3_a6;
	signed int pe3_a7;
	signed int pe3_a8;
	signed int freq1;
	signed int freq2;
	signed int freq3;
	signed int freq4;
	signed int batt_volt;
	signed int air_temp;
	signed int wtr_temp;
	signed int therm5;
	signed int therm6;
	unsigned char ver_mjr;
	unsigned char ver_min;
	unsigned char ver_bld;
	// TCS
	unsigned char TCS_status;
	unsigned char trac_status;
	unsigned char lnc_status;
	unsigned short lnc_dsb_spd;
	signed short fuel_slip_err;
	unsigned short fuel_comp;
	signed short timing_err;
	unsigned short timing_comp;
	unsigned short tcs_a0;
	unsigned short tcs_a1;
	unsigned short tcs_a2;
	unsigned short tcs_a3;
	unsigned short fuel_kp;
	unsigned short fuel_kd;
	unsigned short fuel_ki;
	unsigned short timing_kp;
	unsigned short timing_kd;
	unsigned short timing_ki;
	// IMU
	unsigned char IMU_status;
	signed short x_comp;
	signed short y_comp;
	signed short z_comp;
	signed short acl_x;
	signed short acl_y;
	signed short acl_z;
	signed short gyro_x;
	signed short gyro_y;
	signed short gyro_z;
	signed short mag_x;
	signed short mag_y;
	signed short mag_z;
	signed short mag_heading;
	// STS
	unsigned short STS_status;
	unsigned short ttemp_inr_fl;
	unsigned short ttemp_mdl_fl;
	unsigned short ttemp_otr_fl;
	unsigned short ttemp_inr_fr;
	unsigned short ttemp_mdl_fr;
	unsigned short ttemp_otr_fr;
	unsigned short ttemp_inr_rl;
	unsigned short ttemp_mdl_rl;
	unsigned short ttemp_otr_rl;
	unsigned short ttemp_inr_rr;
	unsigned short ttemp_mdl_rr;
	unsigned short ttemp_otr_rr;

	unsigned short susp_trvl_fl;
	unsigned short susp_trvl_fr;
	unsigned short susp_trvl_rl;
	unsigned short susp_trvl_rr;
	// DLG
	unsigned char DLG_status;
	unsigned char hmsphr;
	unsigned char pos_dil;
	unsigned char time_dil;
	unsigned char date;
	unsigned long lat;
	unsigned long lng;
	unsigned short heading;
	unsigned char speed;
	unsigned long time;
	unsigned char log_status;
	char log_name[8];
	uint32_t rpm_disp;
	uint32_t rpm_mult;
	uint8_t rpm_mode;

	// STW
	unsigned char STW_status;

	// WRL
	unsigned char WRL_status;

}data[2];

// Led Control Structure
volatile struct
{
	uint8_t curr;
	uint8_t curr_sv;
	uint8_t on;
	uint8_t blink_slw;
	uint8_t blink_fst;
	uint16_t rem[8];
	volatile uint8_t lvl;
} leds;

void btn_init(uint8_t idx,volatile uint8_t *prt,uint8_t pn,uint8_t p,uint8_t h);

typedef struct
{
	volatile uint8_t *port;
	uint8_t pin;
	uint8_t state;
	uint8_t pshd;
	uint8_t held;
	uint8_t hcnt;
}BTN_t;

#define BTN_STATE_START		0
#define BTN_STATE_WAIT 		1
#define BTN_STATE_HELD 		2
#define BTN_STATE_RELEASE 	3

#define BTN_NONE_PSHD	0x00
#define BTN_1_PSHD		0x01
#define BTN_1_HELD		0x11
#define BTN_2_PSHD		0x02
#define BTN_2_HELD		0x12
#define BTN_3_PSHD		0x03
#define BTN_3_HELD		0x13
#define BTN_4_PSHD		0x04
#define BTN_4_HELD		0x14
#define BTN_U_PSHD		0x05
#define BTN_U_HELD		0x15
#define BTN_D_PSHD		0x06
#define BTN_D_HELD		0x16
#define BTN_L_PSHD		0x07
#define BTN_L_HELD		0x17
#define BTN_R_PSHD		0x08
#define BTN_R_HELD		0x18
#define BTN_C_PSHD		0x09
#define BTN_C_HELD		0x19
#define BTN_E_PSHD		0x0A
#define BTN_E_HELD		0x1A

#define LED_TIME_EXP 0x0000
#define LED_NO_TMR	0xFFFF

volatile BTN_t btns[10];

volatile uint8_t last_btn;

#define BTN_HELD_TIME 100

#define LED_OFF			0
#define LED_ON			1
#define LED_BLINK_FAST 	2
#define LED_BLINK_SLOW	3



void handle_enc(void);

volatile struct
{
	union
	{
		int8_t buf[4];
		uint32_t shft;
	} hist;
	uint8_t a;
	uint8_t b;
	uint8_t last;
	int8_t val;
	uint8_t chg;
	uint8_t cnt;
}enc;

void led_ctrl(uint8_t led,uint8_t state,uint16_t tmr);
void leds_init(void);
void ui_init(void);
void ui_timer_init(void);
void led_level(uint8_t lvl);
void handle_tach(void);


#endif /* UI_H_ */
