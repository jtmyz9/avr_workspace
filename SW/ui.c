/*
 * ui.c
 *
 *  Created on: Jan 10, 2012
 *      Author: Administrator
 */

#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

volatile uint8_t ui_cntr1 = 0;
volatile uint16_t ui_cntr2 = 0;
volatile uint8_t pot_disp = 1;

#define min(a,b) ((a > b) ?   (b) : (a))

void btn_init(uint8_t idx,volatile uint8_t *prt,uint8_t pn,uint8_t p,uint8_t h)
{
	btns[idx].port = prt;
	btns[idx].pin = pn;
	btns[idx].state = BTN_STATE_START;
	btns[idx].pshd = p;
	btns[idx].held = h;
	btns[idx].hcnt = 0;
}

void ui_init(void)
{
	uint8_t i;

	char str[10];

	glcd_init();
	//glcd_set(0,1);
	//glcd_set(4,1);
	//glcd_set(5,1);
	//glcd_update();
	//_delay_ms(200);

	// Initialize Buttons
	btn_init(0,&BTN_1_PINS,BTN_1,BTN_1_PSHD,BTN_1_HELD);
	btn_init(1,&BTN_2_PINS,BTN_2,BTN_2_PSHD,BTN_2_HELD);
	btn_init(2,&BTN_3_PINS,BTN_3,BTN_3_PSHD,BTN_3_HELD);
	//btn_init(3,&BTN_4_PINS,BTN_4,BTN_4_PSHD,BTN_4_HELD);
	btn_init(4,&BTN_U_PINS,BTN_U,BTN_U_PSHD,BTN_U_HELD);
	btn_init(5,&BTN_D_PINS,BTN_D,BTN_D_PSHD,BTN_D_HELD);
	btn_init(6,&BTN_L_PINS,BTN_L,BTN_L_PSHD,BTN_L_HELD);
	btn_init(7,&BTN_R_PINS,BTN_R,BTN_R_PSHD,BTN_R_HELD);
	btn_init(8,&BTN_C_PINS,BTN_C,BTN_C_PSHD,BTN_C_HELD);
	btn_init(9,&BTN_E_PINS,BTN_E,BTN_E_PSHD,BTN_E_HELD);

	BTN_GP1_PORT = 0xFF;
	BTN_GP2_PORT |= _BV(BTN_E)|_BV(BTN_C);

	ENC_A_PORT |= _BV(ENC_A);
	ENC_B_PORT |= _BV(ENC_B);

	enc.a = 0;
	enc.b = 0;

	enc.hist.buf[3] = 2;
	enc.hist.buf[2] = 2;
	enc.hist.buf[1] = 2;
	enc.hist.buf[0] = 2;
	enc.val = 0;

	enc.cnt = 0;

	last_btn = BTN_NONE_PSHD;

	// Leds
	LED_PORT = 0xFF;
	LED_DDR = 0xFF;
	leds.curr = 0;
	leds.on = 0;
	leds.blink_slw = 0;
	leds.blink_fst = 0;

	for(i = 0;i<8;i++)
	{
		leds.rem[i] = LED_NO_TMR;
	}


	// Tach Leds
	tach_init();
	tach_off();
	tach_out(0);

	// Seven Segment
	svn_seg_init();
	_delay_ms(10);


	// Set up LED PWM timer
	/*
	ui_cntr1 = 0;
	OCR0A = 255;	// Output Compare Value
	TIMSK0 = 0x03;	// Output Compare interrupt
	TCCR0A = 0x04;
	*/

	// Setup UI General Timer
	ui_cntr2 = 0;
	OCR2A = 156;
	TIMSK2 = 0x02;
	TCCR2A = 0x0F;
}



void led_level(uint8_t lvl)
{

}

void led_ctrl(uint8_t led,uint8_t state,uint16_t tmr)
{
	leds.rem[led] = LED_NO_TMR;
	switch(state)
	{
		case LED_OFF:
			leds.on &= ~_BV(led);
			leds.blink_fst &= ~_BV(led);
			leds.blink_slw &= ~_BV(led);
			PORTC |= _BV(led);
			break;
		case LED_ON:
			leds.on |=	_BV(led);
			leds.blink_fst &= ~_BV(led);
			leds.blink_slw &= ~_BV(led);
			PORTC &= ~_BV(led);
			break;
		case LED_BLINK_FAST:
			leds.on &= ~_BV(led);
			leds.blink_fst |= _BV(led);
			leds.blink_slw &= ~_BV(led);
			break;
		case LED_BLINK_SLOW:
			leds.on &= ~_BV(led);
			leds.blink_fst &= ~_BV(led);
			leds.blink_slw |= _BV(led);
			break;
		default :
			break;
	}
	leds.rem[led] = tmr;
}

void handle_btn(uint8_t id)
{
	switch(btns[id].state)
	{
		case BTN_STATE_START :
			if(!(*(btns[id].port) & _BV(btns[id].pin)))
			{
				btns[id].hcnt = 0;
				btns[id].state = BTN_STATE_WAIT;
			}
			break;
		case BTN_STATE_WAIT :
			if(btns[id].hcnt >= BTN_HELD_TIME)
			{
				btns[id].state = BTN_STATE_HELD;
			}
			else if(*(btns[id].port) & _BV(btns[id].pin))
			{
				last_btn = btns[id].pshd;
				btns[id].state = BTN_STATE_START;
			}
			btns[id].hcnt++;
			break;
		case BTN_STATE_HELD :
			last_btn = btns[id].held;
			if(*(btns[id].port) & _BV(btns[id].pin))
			{
				btns[id].state = BTN_STATE_START;
			}
			else
			{
				btns[id].state = BTN_STATE_RELEASE;
			}
			break;
		case BTN_STATE_RELEASE :
			if(*(btns[id].port) & _BV(btns[id].pin))
			{
				btns[id].state = BTN_STATE_START;
			}
			break;
		default:
			break;
	}
}

const int8_t enc_table[4][4] = {{0,-1,1,2},
								{1,0,2,-1},
								{-1,2,0,1},
								{2,1,-1,0}};

const uint8_t enc_state_vals[2][2] = 	{{0,1},
										 {2,3}};

void handle_enc(void)
{
	uint8_t a_new = 0;
	uint8_t b_new = 0;
	uint8_t i;
	uint8_t sum = 0;

	if(ENC_A_PINS & _BV(ENC_A))	a_new = 1;
	if(ENC_B_PINS & _BV(ENC_B)) b_new = 1;

	if(a_new == enc.a && b_new == enc.b)
	{
		return;
	}

	enc.hist.shft = (enc.hist.shft) << 8;
	enc.hist.buf[0] = enc_table[a_new + b_new*2][enc.a +enc.b*2];

	enc.a = a_new;
	enc.b = b_new;

	for(i = 0;i<4;i++)
	{
		if(enc.hist.buf[i] == 2)	return;

		sum+= enc.hist.buf[i];
	}

	enc.val +=sum;
}

volatile uint16_t last_tach = 0;
volatile uint16_t last_tach_disp = 0;

#define SHIFT_POINT 14500

void handle_tach(void)
{
	uint16_t curr_tach;
	uint16_t curr_tach_disp;
	if(data[0].rpm_mode == RPM_OFF)
	{
		curr_tach = data[0].rpm;
	}
	else
	{
		curr_tach = data[0].rpm_disp;
	}

//	if(curr_tach > SHIFT_POINT)
//	{
//		//PORTC &= ~_BV(LED5);
//		//PORTC &= ~_BV(LED6);
//		tach_on();
//		tach_out(15);
//	}
//	else
//	{
//		//PORTC |= _BV(LED5);
//		//PORTC |= _BV(LED6);
//		tach_off();
//	}

		if(tach_sts())
		{


				curr_tach_disp = (((uint32_t)curr_tach*16)/13500);
				if(curr_tach_disp != last_tach_disp)
				{
					tach_out(min(((uint32_t)curr_tach*16)/13500,16));
					last_tach_disp = curr_tach_disp;
				}

		}

		svn_seg_put_val(curr_tach);

		last_tach = curr_tach;

}

volatile int16_t lastx = 0;
volatile int16_t lasty = 0;
volatile int16_t lastz = 0;
volatile uint8_t acl_frozen = 0;
volatile uint8_t last_acl_chg = 0;

ISR(TIMER2_COMP_vect)
{
	uint8_t i;

	ui_cntr2++;
	if(ui_cntr2 == 100000)
	{
		ui_cntr2 = 0;
	}

	handle_enc();


	for(i = 0;i<10;i++)
	{
		if(i != 3)
		{
			handle_btn(i);
		}
	}

	handle_tach();

	if((data[0].acl_x == lastx)&&(data[0].acl_y == lasty)&&(data[0].acl_z == lastz))
	{
		if(last_acl_chg < 100)
		{
				last_acl_chg++;
		}
		else
		{
			acl_frozen = 1;
		}
	}
	else
	{
		last_acl_chg = 0;
		acl_frozen = 0;
	}

	lastx=data[0].acl_x;
	lasty=data[0].acl_y;
	lastz=data[0].acl_z;

	if(data[0].rpm_mode == RPM_ON)
	{
		PORTC &= ~_BV(LED4);
	}
	else
	{
		PORTC |= _BV(LED4);
	}

	if(data[0].DLG_status == 1)
	{
		PORTC &= ~_BV(LED1);
	}
	else
	{
		PORTC |= _BV(LED1);
	}

	if(!(PINE & _BV(PE5)))
	{
		PORTC &= ~_BV(LED7);
	}
	else
	{
		PORTC |= _BV(LED7);
	}

	if(acl_frozen == 1)
	{
		PORTC &= ~_BV(LED5);
		PORTC &= ~_BV(LED6);
	}
	else
	{
		PORTC |= _BV(LED5);
		PORTC |= _BV(LED6);
	}

	if(data[0].lnc_status == 1)
	{
		PORTC &= ~_BV(LED3);
	}
	else
	{
		PORTC |= _BV(LED3);
	}


		/*


		// Check for expired led timer
		for(i = 0;i<8;i++)
		{
			if((leds.on | leds.blink_fst | leds.blink_slw) & _BV(i))
			{
				if(leds.rem[i] == LED_TIME_EXP)
				{
					leds.on &= ~_BV(i);
					leds.blink_fst &= ~_BV(i);
					leds.blink_slw &= ~_BV(i);
				}
				else if(leds.rem[i] != LED_NO_TMR)
				{
					leds.rem[i]--;
				}
			}
		}


		leds.curr &= (leds.blink_fst | leds.blink_slw|leds.on);
		leds.curr |= leds.on;


		if((ui_cntr2 % 50) == 0)
		{
			leds.curr ^= leds.blink_slw;
		}

		if((ui_cntr2 % 10) == 0)
		{
			leds.curr ^= leds.blink_fst;
		}
		*/

		//PORTC = ~leds.on;


}
