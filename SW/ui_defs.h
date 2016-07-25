/*
 * ui_defs.h
 *
 *  Created on: Jan 11, 2012
 *      Author: Administrator
 */



#ifndef UI_DEFS_H_
#define UI_DEFS_H_

#include <avr/io.h>

// Leds
#define LED_DDR		DDRC
#define LED_PORT	PORTC
#define LED_PINS	PINC
#define LED1		0
#define LED2		1
#define LED3		2
#define LED4		3
#define LED5		4
#define LED6		5
#define LED7		6
#define LED8		7

// Buttons

#define BTN_GP1_PORT 	PORTA
#define BTN_GP1_PINS	PINA

#define BTN_1_PORT		BTN_GP1_PORT
#define BTN_1_PINS		BTN_GP1_PINS
#define BTN_1			PA0
#define BTN_2_PORT		BTN_GP1_PORT
#define BTN_2_PINS		BTN_GP1_PINS
#define BTN_2			PA1
#define BTN_3_PORT		BTN_GP1_PORT
#define BTN_3_PINS		BTN_GP1_PINS
#define BTN_3			PA2
#define BTN_4_PORT		BTN_GP1_PORT
#define BTN_4_PINS		BTN_GP1_PINS
#define BTN_4			PA3
#define BTN_U_PORT		BTN_GP1_PORT
#define BTN_U_PINS		BTN_GP1_PINS
#define BTN_U			PA4
#define BTN_D_PORT		BTN_GP1_PORT
#define BTN_D_PINS		BTN_GP1_PINS
#define BTN_D			PA5
#define BTN_L_PORT		BTN_GP1_PORT
#define BTN_L_PINS		BTN_GP1_PINS
#define BTN_L			PA6
#define BTN_R_PORT		BTN_GP1_PORT
#define BTN_R_PINS		BTN_GP1_PINS
#define BTN_R			PA7

#define BTN_GP2_PORT 	PORTB
#define BTN_GP2_PINS 	PINB

#define BTN_C_PORT		BTN_GP2_PORT
#define BTN_C_PINS		BTN_GP2_PINS
#define BTN_C			PB7
#define BTN_E_PORT		BTN_GP2_PORT
#define BTN_E_PINS		BTN_GP2_PINS
#define BTN_E			PB6

#define ENC_A_PORT	PORTD
#define ENC_A_PINS	PIND
#define ENC_A		PD1
#define ENC_B_PORT	PORTD
#define ENC_B_PINS	PIND
#define ENC_B		PD0

#endif /* UI_DEFS_H_ */
