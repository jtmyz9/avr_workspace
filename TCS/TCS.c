/*
 * TCS.c
 *
 *  Created on: Jan 5, 2012
 *      Author: Administrator
 */

#include <avr/io.h>
#include "TCS.h"
#include "util/delay.h"
#include "systimer.h"

volatile struct
{
	uint8_t tc_status;
	uint8_t lc_status;
	uint32_t fl;
	uint32_t fr;
	uint32_t rl;
	uint32_t rr;
	uint16_t last_com;
	uint32_t slip;
	uint32_t slip_error;
	uint32_t comp;
	uint32_t rspd;
	uint32_t fspd;
}data;

void lc_off(void);
void lc_on(void);

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

#define UART_BAUD 115200

void handle_10ms_tick(void);
uint8_t ProcessCanMsg(CAN_packet *pkt);

volatile uint8_t tc = 0;
volatile uint8_t lc = 0;

int main(void)
{
	unsigned  i = 0;
	unsigned char addr = 0;

	uint16_t val0 = 0;
	uint16_t val1 = 0;

	DDRA |= _BV(PA7);
	PORTA &= _BV(PA7);

	DDRE |= _BV(PE2)|_BV(PE6);

	DDRD |=_BV(PD7);


	//initialize UART1
	stdout = stdin = &uart_str;
	UCSR1A=_BV(U2X1);
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1 = (F_CPU / (8UL * UART_BAUD)) - 1;

	printf("\n>------| START |------<\n\n");

	//printf("Waiting...\n\n");

	if(!dac_init())
	{
		printf("Failed\n");
		while(1);
	}

	can_init();

	uint32_t tag = CAN_DATA_TC_TGL;
	tag = tag << 8;
	prepare_rx(0,tag,0x00FFFF00,CAN_EXT_MSG,CAN_DATABRDCST);
	tag = CAN_DATA_LC_TGL;
	tag = tag << 8;
	prepare_rx(1,tag,0x00FFFF00,CAN_EXT_MSG,CAN_DATABRDCST);
	tag = CAN_DATA_PE5;
	tag = tag << 8;
	prepare_rx(2,tag,0x00FFFF00,CAN_EXT_MSG,CAN_DATABRDCST);

	CANPAGE = 0;

	systimer_10ms_restart(0);

	lc_off();

	sei();

	printf("\n\n>------| DONE |------<\n\n");

	while(1)
	{
	}
}

//! CAN interrupt service routine
ISR( CANIT_vect)
{
	unsigned char i;
	CAN_packet packet;
	unsigned char save_canpage;
	unsigned char mob;
	unsigned char length;

	/*-------------Device Specific-----------------*/
	//led_ctrl(LED8,LED_ON,1);
	PORTD |= _BV(PD7);
	/*---------------------------------------------*/

	mob=CANHPMOB; // get highest prio mob

	save_canpage = 0xF0 | mob;

    CANPAGE = mob & 0xf0;
	mob >>= 4; // -> mob number 0..15

	packet.msg_type = msg_type_lu[mob];

	if(packet.msg_type == CAN_TX_MSG)
	{
		CANSTMOB = 0;
		CANCDMOB = 0;

		CANIDT1 = 0;
		CANIDT2 = 0;
		CANIDT3 = 0;
		CANIDT4 = 0;

		for(i = 0;i<8;i++)
		{
			CANMSG = 0;
		}

		msg_type_lu[mob] = 0;
	}
	else
	{
		if(CANCDMOB & _BV(IDE))
		{
			packet.id_type = CAN_EXT_MSG;
			packet.id = 	(((uint32_t)CANIDT1) << 21)
						| (((uint32_t)CANIDT2) << 13)
						| (((uint32_t)CANIDT3) << 5)
						| (((uint32_t)CANIDT4) >> 3);
		}

		else
		{
			packet.id_type = CAN_STD_MSG;
			packet.id = (CANIDT2>>5) | (CANIDT1 <<3);
		}

		length=CANCDMOB & 0x0f;
		packet.length=length;

		for (i = 0; i <length; ++i)
		{
			packet.data[i] = CANMSG;
		}

		ProcessCanMsg(&packet);

		CANSTMOB=0;		// reset INT reason
		CANCDMOB &= 0x80|_BV(IDE);	// re-enable RX on this channel
	}

	CANPAGE=save_canpage;	// restore CANPAGE

	/*-------------Device Specific-----------------*/
	PORTD &= ~_BV(PD7);
	//led_ctrl(LED8,LED_OFF,1);

	/*---------------------------------------------*/
}

void lc_off(void)
{
	PORTA &= ~_BV(PA7);
}

void lc_on(void)
{
	PORTA |= _BV(PA7);
}

#define ch2word(msb,lsb)	((((uint16_t)(msb) & 0x00FF) << 8) + lsb)
#define ch2dblword(b3,b2,b1,b0)	((((uint32_t)(b3) & 0x000000FF) << 24) + (((uint32_t)(b2) & 0x000000FF) << 16) + (((uint32_t)(b1) & 0x000000FF) << 8) + b0)

uint8_t ProcessCanMsg(CAN_packet *pkt)
{
	uint8_t msg_type;

	unsigned char i;

	CAN_packet tx_pkt;

	// Get the Message Type
	msg_type = pkt->msg_type;
	// Clear the message type in the lookup table

	uint16_t id = (uint16_t)((uint32_t)(pkt->id)>>8);

	switch(msg_type)
	{
		case CAN_MSGTYPE_NONE:
			return 0;
		case CAN_DATABRDCST:
			switch(id)
			{
				case CAN_DATA_TC_TGL:
					if(data.tc_status == 0)
					{
						data.tc_status = 1;
						PORTE |= _BV(PE2);
					}
					else
					{
						data.tc_status = 0;
						PORTE &= ~_BV(PE2);
					}

					return(msg_type);
				case CAN_DATA_LC_TGL:
					if(data.lc_status == 0)
					{
						data.lc_status = 1;
						PORTE |= _BV(PE6);
					}
					else
					{
						data.lc_status = 0;
						PORTE &= ~_BV(PE6);
					}
					return(msg_type);
				case CAN_DATA_TC_SET:
					return(msg_type);
				case CAN_DATA_LC_SET:
					return(msg_type);
				case CAN_DATA_PE1:
					return(msg_type);
				case CAN_DATA_PE5:
					// X/1000 = MPH
					data.fl = 100*(uint32_t)ch2word(pkt->data[1],pkt->data[0]);
					data.fr = (uint32_t)ch2word(pkt->data[3],pkt->data[2]);
					data.rl = 100*36*(uint32_t)ch2word(pkt->data[5],pkt->data[4]);
					data.rr = 100*36*(uint32_t)ch2word(pkt->data[7],pkt->data[6]);

					data.rl = data.rl/(21UL);
					data.rr = data.rr/(21UL);

					data.fspd = (data.fr);
					data.rspd = (data.rl +data.rr)/2;


#define SLIP_ERROR_CEILING 4000 // 400.0%
#define COMP_MAX 4095
#define ALLOWED_SLIP 200 // 20.0%
					if(data.fspd != 0)
					{
						data.slip = (1000*data.rspd - 1000*data.fspd)/(data.fspd);
					}
					else
					{
						data.slip = (1000*data.rspd - 1000);
					}

					if(data.slip <= ALLOWED_SLIP)
					{
						data.slip_error = 0;
					}
					else
					{
						data.slip_error = data.slip - ALLOWED_SLIP;
					}

					if(data.lc_status == 1)
					{
						if((data.fl == 0) && (data.fr == 0) && (data.rl == 0) && (data.rr == 0))
						{
							lc_on();
						}
						else
						{
							lc_off();
						}
					}
					else
					{
						lc_off();
					}




					if(data.tc_status == 1)
					{

						if(data.slip_error == 0)
						{
							data.comp = 0;
						}
						else
						{
							data.comp = (data.slip_error * COMP_MAX)/SLIP_ERROR_CEILING;
						}

						dac_write(DAC0,DAC_FAST,DAC_NORMAL,data.comp);
						dac_write(DAC1,DAC_FAST,DAC_NORMAL,data.comp);

						tx_pkt.id = CAN_DATA_TCS2;
						tx_pkt.id = tx_pkt.id << 8;
						tx_pkt.id_type = CAN_EXT_MSG;
						tx_pkt.msg_type = CAN_TX_MSG;
						tx_pkt.length = 8;
						tx_pkt.data[0]=(data.slip_error >> 8) & 0x00FF;
						tx_pkt.data[1]=(data.slip_error & 0x00FF);
						tx_pkt.data[2]=(data.comp >> 8) & 0x00FF;
						tx_pkt.data[3]=(data.comp & 0x00FF);
						tx_pkt.data[4]=(data.slip >> 8) & 0x00FF;
						tx_pkt.data[5]=(data.slip & 0x00FF);
						tx_pkt.data[6] = 0;
						tx_pkt.data[7] = 0;

						can_tx(get_open_mob(),&tx_pkt);

					}

					return(msg_type);
				default:
					return(msg_type);
			}
			break;

		default:
			return(0);
	}
	return(0);
}

uint16_t glb_tmr = 0;
void handle_10ms_tick(void)
{
	CAN_packet pkt;
	glb_tmr++;

	if(glb_tmr%100 == 0)
	{
		pkt.id = CAN_DATA_TCS1;
		pkt.id = pkt.id << 8;
		pkt.id_type = CAN_EXT_MSG;
		pkt.msg_type = CAN_TX_MSG;
		pkt.length = 5;
		pkt.data[1] = data.tc_status;
		pkt.data[2] = data.lc_status;

		can_tx(get_open_mob(),&pkt);
	}
}
