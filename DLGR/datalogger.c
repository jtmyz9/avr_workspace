/*
 * datalogger.c
 *
 *  Created on: Dec 30, 2011
 *      Author: Mark
 */

#include "datalogger.h"
#include "gps.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "util/atomic.h"

static uint8_t print_disk_info(const struct fat_fs_struct* fs);

uint8_t ProcessGpsString(const char *rx_str);
uint8_t GetParameterString(const char *in_str,uint8_t *index,char *pstr,uint8_t max,uint8_t min);
#define print(s)	uart_puts_p(PSTR(s))
#define println(s)	uart_puts_p(PSTR(s));\
					uart_puts_p(PSTR("\n"))

#define NUM_ELEMENTS 30

volatile char test_string[6];

int main()
{
	char c;
	char gps_rx_str[GPS_RX_STR_SIZE+2] = "";
	uint8_t gps_rx_idx = 0;

	sys.data.status.DLG_status = 0;
	log_status = 0;
	uint8_t gps_rx_state = RX_WAIT_FOR_START;

	uart_init();

	//uart_puts_p(PSTR("\n------| START |------\n"));

	DDRE|=_BV(PE6);
	DDRD|=_BV(PD7);

    init_log_system();
    gps_init();

	can_init();

	prepare_rx(0,0,0,CAN_EXT_MSG,CAN_DATABRDCST);
	prepare_rx(1,0,0,CAN_EXT_MSG,CAN_DATABRDCST);
	prepare_rx(2,0,0,CAN_EXT_MSG,CAN_DATABRDCST);
	prepare_rx(3,0,0,CAN_EXT_MSG,CAN_DATABRDCST);
	prepare_rx(4,0,0,CAN_EXT_MSG,CAN_DATABRDCST);
	prepare_rx(5,0,0,CAN_EXT_MSG,CAN_DATABRDCST);
	prepare_rx(6,0,0,CAN_EXT_MSG,CAN_DATABRDCST);
	prepare_rx(7,0,0,CAN_EXT_MSG,CAN_DATABRDCST);

	systimer_10ms_restart(0);

	sei();

	char out_str[100] = "";

	while(1)
	{
		if(gps_getc((uint8_t *)&c))
		{
			//uart_putc(c);
			switch(gps_rx_state)
			{
				case RX_STOPPED:
					break;
				case RX_WAIT_FOR_START:
					if(c == '$')
					{
						gps_rx_state = RX_STARTED;
						gps_rx_str[0] = '$';
						gps_rx_idx = 1;
					}
					break;
				case RX_STARTED:
					if(c == '$')
					{
						gps_rx_str[0] = '$';
						gps_rx_idx = 1;
					}
					else if((c == '\n')||(c == '\r')||(c == '*'))
					{

						// Guarantee null byte at the end of string
						gps_rx_str[gps_rx_idx++] = ',';
						gps_rx_str[gps_rx_idx++] = 0;

						uart_puts(gps_rx_str);
						uart_puts("\r\n");
						if(ProcessGpsString(gps_rx_str))
						{
							ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
							{
								sys.data.chan.lat = gps.lat_min_frac + gps.lat_min*100000 + gps.lat_deg*6000000;
								sys.data.chan.lng = gps.lng_min_frac + gps.lng_min*100000 + gps.lng_deg*6000000;
								sys.data.chan.heading = (uint16_t)gps.heading;
								sys.data.chan.speed = (uint16_t)gps.speed;
								sys.data.chan.last_tick = gps.last_tick;
							}
							//sprintf(out_str,"%u %u\r\n",gps.speed,gps.heading);
							//uart_puts(out_str);

						}
						else
						{
//							ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
//							{
//								sys.data.chan.lat = 0xFFFFFFFF;
//								sys.data.chan.lng = 0xFFFFFFFF;
//								sys.data.chan.last_tick = gps.last_tick;
//							}
						}
						gps_rx_state = RX_WAIT_FOR_START;
					}
					else
					{
						gps_rx_str[gps_rx_idx++] = c;

						if(gps_rx_idx == GPS_RX_STR_SIZE)
						{
							gps_rx_state = RX_WAIT_FOR_START;
						}
					}
					break;
				default:
					break;

			}
		}
	}
}

uint8_t GetParameterString(const char *in_str,uint8_t *index,char *pstr,uint8_t max,uint8_t min)
{
	uint8_t pstr_idx = 0;
	char c;
	//char test_str[50];
	//sprintf(test_str,"Index[%u]\n\r",*index);
	//uart_puts(test_str);

	while(1)
	{
		if(pstr_idx == max+1)
		{
			return 0xFF;
		}

		c = in_str[(*index)++];

		if(c == ',')
		{
//			if(pstr_idx >= min)	return pstr_idx;
//			else				return 0xFF;
			return pstr_idx;
		}
		else
		{
			pstr[pstr_idx++] = c;
		}
	}

	return 0;
}



uint8_t ProcessGpsString(const char *rx_str)
{
	uint8_t idx = 0;
	uint8_t parm_idx = 0;
	char parm_str[15];
	uint8_t parm_resp;
	uint8_t i;

	//char test_str[100];

	for(i = 0;i<15;i++)
	{
		parm_str[i] = 0;
	}

	while(1)
	{
		switch(parm_idx)
		{

			case 0:	// Process Start bytes
				if(			(rx_str[0] == '$')
						&& 	(rx_str[1] == 'P')
						&& 	(rx_str[2] == 'G')
						&& 	(rx_str[3] == 'R')
						&& 	(rx_str[4] == 'M')
						&& 	(rx_str[5] == 'F')
						&& 	(rx_str[6] == ','))
				{

					parm_idx++;
					idx = 7;

				}
				else
				{
					return 0;
				}
				break;

			case 1:	// GPS week number (Not used)

				parm_resp = GetParameterString(rx_str,&idx,parm_str,4,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}



				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.week_num = 0;
				}
				else
				{
					gps.week_num = atoi(parm_str);
				}
				parm_idx++;
				break;

			case 2: // GPS seconds
				parm_resp = GetParameterString(rx_str,&idx,parm_str,6,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.seconds = 0;
				}
				else
				{
					gps.seconds = atol(parm_str);
				}
				parm_idx++;
				break;

			case 3: // UTC Date
				parm_resp = GetParameterString(rx_str,&idx,parm_str,6,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}



				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.utc_day = 0;
					gps.utc_month = 0;
					gps.utc_year = 0;
				}
				else
				{
					if(isdigit(parm_str[0]) && isdigit(parm_str[1]))
					{
						gps.utc_day = (parm_str[0]-48)*10 + (parm_str[1] - 48);
					}
					else
					{
						gps.utc_day = 0;
					}

					if(isdigit(parm_str[2]) && isdigit(parm_str[3]))
					{
						gps.utc_month = (parm_str[2]-48)*10 + (parm_str[3] - 48);
					}
					else
					{
						gps.utc_month = 0;
					}

					if(isdigit(parm_str[4]) && isdigit(parm_str[5]))
					{
						gps.utc_year = (parm_str[4]-48)*10 + (parm_str[5] - 48);
					}
					else
					{
						gps.utc_year = 0;
					}
				}
				parm_idx++;
				break;

			case 4: // UTC Time
				parm_resp = GetParameterString(rx_str,&idx,parm_str,8,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.utc_hour = 0;
					gps.utc_min = 0;
					gps.utc_sec = 0;
					gps.utc_tenths = 0;
				}
				else
				{
					if(isdigit(parm_str[0]) && isdigit(parm_str[1]))
					{
						gps.utc_hour = (parm_str[0]-48)*10 + (parm_str[1] - 48);
					}
					else
					{
						gps.utc_hour = 0;
					}

					if(isdigit(parm_str[2]) && isdigit(parm_str[3]))
					{
						gps.utc_min = (parm_str[2]-48)*10 + (parm_str[3] - 48);
					}
					else
					{
						gps.utc_min = 0;
					}

					if(isdigit(parm_str[4]) && isdigit(parm_str[5]))
					{
						gps.utc_sec = (parm_str[4]-48)*10 + (parm_str[5] - 48);
					}
					else
					{
						gps.utc_sec = 0;
					}

					if((parm_str[6] == '.') && (isdigit(parm_str[7])))
					{
						gps.utc_tenths = parm_str[7]-48;
					}
					else
					{
						gps.utc_tenths = 0;
					}
				}
				parm_idx++;
				break;

			case 5:	// GPS leap second count
				parm_resp = GetParameterString(rx_str,&idx,parm_str,10,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.leap_sec = 0;
				}
				else
				{
					gps.leap_sec = atol(parm_str);
				}
				parm_idx++;
				break;

			case 6:	// Latitude
				parm_resp = GetParameterString(rx_str,&idx,parm_str,10,10);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.lat_deg = 0;
					gps.lat_min = 0;
					gps.lat_min_frac = 0;
				}
				else
				{
					if(isdigit(parm_str[0]) && isdigit(parm_str[1]))
					{
						gps.lat_deg = (parm_str[0]-48)*10 + (parm_str[1] - 48);
					}
					else
					{
						gps.lat_deg = 0;
					}

					if(isdigit(parm_str[2]) && isdigit(parm_str[3]))
					{
						gps.lat_min = (parm_str[2]-48)*10 + (parm_str[3] - 48);
					}
					else
					{
						gps.lat_min = 0;
					}

					if(			parm_str[4] == '.'
							&&	isdigit(parm_str[5])
							&& 	isdigit(parm_str[6])
							&&	isdigit(parm_str[7])
							&&	isdigit(parm_str[8])
							&&	isdigit(parm_str[9]))
					{

						gps.lat_min_frac =	((uint32_t)(parm_str[5]-48))*10000;
						gps.lat_min_frac +=	((uint32_t)(parm_str[6]-48))*1000;
						gps.lat_min_frac +=	((uint32_t)(parm_str[7]-48))*100;
						gps.lat_min_frac +=	((uint32_t)(parm_str[8]-48))*10;
						gps.lat_min_frac +=	((uint32_t)(parm_str[9]-48));
					}
					else
					{
						gps.lat_min_frac = 0;
					}
				}
				parm_idx++;
				break;

			case 7: // Latitude Hemisphere
				parm_resp = GetParameterString(rx_str,&idx,parm_str,1,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				if((parm_str[0] == 'N')||(parm_str[0] == 'S'))
				{
					gps.lat_hemi = parm_str[0];
				}
				else
				{
					gps.lat_hemi = 0;
				}

				parm_idx++;
				break;

			case 8:	// Longitude

				parm_resp = GetParameterString(rx_str,&idx,parm_str,11,11);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.lng_deg = 0;
					gps.lng_min = 0;
					gps.lng_min_frac = 0;
				}
				else
				{
					if(isdigit(parm_str[0]) && isdigit(parm_str[1]) && isdigit(parm_str[2]))
					{
						gps.lng_deg = (parm_str[0]-48)*100 + (parm_str[1] - 48)*10 + (parm_str[2] - 48);
					}
					else
					{
						gps.lng_deg = 0;
					}

					if(isdigit(parm_str[3]) && isdigit(parm_str[4]))
					{
						gps.lng_min = (parm_str[3]-48)*10 + (parm_str[4] - 48);
					}
					else
					{
						gps.lng_min = 0;
					}

					if(			parm_str[5] == '.'
							&&	isdigit(parm_str[6])
							&& 	isdigit(parm_str[7])
							&&	isdigit(parm_str[8])
							&&	isdigit(parm_str[9])
							&&	isdigit(parm_str[10]))
					{
						gps.lng_min_frac =	((uint32_t)(parm_str[6]-48))*10000;
						gps.lng_min_frac +=	((uint32_t)(parm_str[7]-48))*1000;
						gps.lng_min_frac +=	((uint32_t)(parm_str[8]-48))*100;
						gps.lng_min_frac +=	((uint32_t)(parm_str[9]-48))*10;
						gps.lng_min_frac +=	((uint32_t)(parm_str[10]-48));
					}
					else
					{
						gps.lng_min_frac = 0;
					}
				}
				parm_idx++;
				break;

			case 9: // Longitude Hemisphere
				parm_resp = GetParameterString(rx_str,&idx,parm_str,1,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				if((parm_str[0] == 'E')||(parm_str[0] == 'W'))
				{
					gps.lng_hemi = parm_str[0];
				}
				else
				{
					gps.lng_hemi = 0;
				}

				parm_idx++;
				break;

			case 10: // Mode
				parm_resp = GetParameterString(rx_str,&idx,parm_str,1,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				if((parm_str[0] == 'A')||(parm_str[0] == 'M'))
				{
					gps.mode = parm_str[0];
				}
				else
				{
					gps.mode = 0;
				}

				parm_idx++;
				break;

			case 11:	// Fix type

				parm_resp = GetParameterString(rx_str,&idx,parm_str,1,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.fix_type = 0;
				}
				else
				{
					gps.fix_type = atoi(parm_str);
				}
				parm_idx++;
				break;

			case 12:	// Speed

				parm_resp = GetParameterString(rx_str,&idx,parm_str,4,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

//				// Process Parameter String
//				if((parm_resp == 0)||1)
//				{
//					gps.speed = 0;
//				}
//				else
//				{
//					if(isdigit(parm_str[0]) && isdigit(parm_str[1]) && isdigit(parm_str[2] && isdigit(parm_str[3])))
//					{
//						gps.speed = (uint16_t)(parm_str[0]-48)*1000 +(uint16_t)(parm_str[1]-48)*100 + (uint16_t)(parm_str[2] - 48)*10 + (uint16_t)(parm_str[3] - 48);
//					}
//					else
//					{
//						gps.speed = 0;
//					}
//				}
				parm_idx++;
				break;

			case 13:	// Heading

				parm_resp = GetParameterString(rx_str,&idx,parm_str,3,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

//				// Process Parameter String
//				if((parm_resp == 0)||1)
//				{
//					gps.heading = 0;
//				}
//				else
//				{
//					if(isdigit(parm_str[0]) && isdigit(parm_str[1]) && isdigit(parm_str[2]))
//					{
//						gps.heading = (uint16_t)(parm_str[0]-48)*100 +(uint16_t)(parm_str[1]-48)*10 + (uint16_t)(parm_str[2] - 48);
//					}
//					else
//					{
//						gps.heading = 0;
//					}
//				}
				parm_idx++;
				break;

			case 14:	// Position Dilution

				parm_resp = GetParameterString(rx_str,&idx,parm_str,1,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.pos_dil = 0;
				}
				else
				{
					gps.pos_dil = atoi(parm_str);
				}
				parm_idx++;
				break;

			case 15:	// Time Dilution

				/*
				parm_resp = GetParameterString(rx_str,&idx,parm_str,1,0);

				if(parm_resp == 0xFF)
				{
					return 0;
				}

				// Process Parameter String
				if(parm_resp == 0)
				{
					gps.pos_dil = 0;
				}
				else
				{
					gps.pos_dil = atoi(parm_str);
				}
				*/
				parm_idx++;
				break;
			case 16:
				return 1;
			default :
				return 0;
		}

	}
	return 1;
}

void handle_10ms_tick(void)
{
	CAN_packet pkt;
	glb_tmr += 10;
	sys.data.chan.curr_tick = TCNT3;

	/*----------------Handle Interrupt Priority------------------*/
	CANGIT = 0;
	TIMSK1 = 0;
	sei();
	/*-----------------------------------------------------------*/

	if(log_status == LOGGING)
	{
		if((log_timer % 10) == 0)
		{
			log(sys.log.f100);
		}

		if((log_timer % 100) == 0)
		{

		    log(sys.log.f10);
		}

		if((log_timer % 200) == 0)
		{

		    log(sys.log.f5);
		}

		if((log_timer %1000) == 0)
		{
		    log(sys.log.f1);
		}

		log_timer+=10;
	}

	if((glb_tmr % 1000)== 0)
	{

		pkt.id = CAN_DATA_DLG1;
		pkt.id = pkt.id << 8;
		pkt.id_type = CAN_EXT_MSG;
		pkt.length = 6;
		pkt.data[0] = log_status;
		pkt.data[1] = 0;
		pkt.data[2] = 0;
		pkt.data[3] = 0;
		pkt.data[4] = 0;
		pkt.data[5] = 0;

		can_tx(get_open_mob(),&pkt);

	}
	/*-------------Device Specific-----------------*/
	cli();
	CANGIE=(1<<ENIT) | (1<< ENRX) | (1 << ENTX);
	TIMSK1 = _BV(OCIE1A);
	/*---------------------------------------------*/
}

//! CAN interrupt service routine
ISR( CANIT_vect)
{


	unsigned char i;
	CAN_packet packet;
	unsigned char save_canpage;
	unsigned char mob;
	unsigned char length;

	/*----------------Handle Interrupt Priority------------------*/
	/*
	CANGIT = 0;
	TIMSK1 = 0;
	sei();

	*/
	/*-----------------------------------------------------------*/
	PORTD |= _BV(PD7);

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
	/*
	cli();
	uart_puts("Here\n");

	CANGIE=(1<<ENIT) | (1<< ENRX) | (1 << ENTX);
	TIMSK1 = _BV(OCIE1A);
	*/
	/*---------------------------------------------*/
}

#define ch2word(msb,lsb)	((((uint16_t)(msb) & 0x00FF) << 8) + lsb)
#define ch2dblword(b3,b2,b1,b0)	((((uint32_t)(b3) & 0x000000FF) << 24) + (((uint32_t)(b2) & 0x000000FF) << 16) + (((uint32_t)(b1) & 0x000000FF) << 8) + b0)

volatile uint8_t data_chg = 0;

uint8_t ProcessCanMsg(CAN_packet *pkt)
{
	uint8_t msg_type;

	unsigned char i;

	// Get the Message Type
	msg_type = pkt->msg_type;
	// Clear the message type in the lookup table

	uint16_t id = (uint16_t)((uint32_t)(pkt->id)>>8);

	switch(msg_type)
	{
		case CAN_MSGTYPE_NONE:
			return(0);
			return msg_type;
		case CAN_DATABRDCST:
			data_chg = 1;

			switch(id)
			{
				case CAN_DATA_DLGR_TGL:
					if(log_status == NOT_LOGGING)
					{
						open_log("test");
					}
					else if(log_status == LOGGING)
					{
						close_log();
					}
					return msg_type;

				case CAN_DATA_PE1:
					//led_ctrl(LED1,LED_ON,LED_NO_TMR);
					sys.data.chan.rpm = ch2word(pkt->data[1],pkt->data[0]);
					sys.data.chan.tps = ch2word(pkt->data[3],pkt->data[2]);
					sys.data.chan.fuel_open_time = ch2word(pkt->data[5],pkt->data[4]);
					sys.data.chan.ign_angle = ch2word(pkt->data[7],pkt->data[6]);
					return(msg_type);
				case CAN_DATA_PE2:
					//sys.data.chan.barometer = ch2word(pkt->data[1],pkt->data[0]);
					sys.data.chan.map = ch2word(pkt->data[3],pkt->data[2]);
					sys.data.chan.lambda = ch2word(pkt->data[5],pkt->data[4]);
					return(msg_type);
				case CAN_DATA_PE3:
					sys.data.chan.fuel_comp = ch2word(pkt->data[1],pkt->data[0]);
					sys.data.chan.timing_comp = ch2word(pkt->data[3],pkt->data[2]);
					sys.data.chan.fbrk_pres = ch2word(pkt->data[5],pkt->data[4]);
					sys.data.chan.rbrk_pres = ch2word(pkt->data[7],pkt->data[6]);
					return(msg_type);
				case CAN_DATA_PE4:
					//sys.data.chan.pe3_a5 = ch2word(pkt->data[1],pkt->data[0]);
					sys.data.chan.oil_pres = ch2word(pkt->data[3],pkt->data[2]);
					//sys.data.chan.pe3_a7 = ch2word(pkt->data[5],pkt->data[4]);
					//sys.data.chan.pe3_a8 = ch2word(pkt->data[7],pkt->data[6]);
					return(msg_type);
				case CAN_DATA_PE5:
					sys.data.chan.fl_wspd = ch2word(pkt->data[1],pkt->data[0]);
					sys.data.chan.fr_wspd = ch2word(pkt->data[3],pkt->data[2]);
					sys.data.chan.rl_wspd = ch2word(pkt->data[5],pkt->data[4]);
					sys.data.chan.rr_wspd = ch2word(pkt->data[7],pkt->data[6]);
					return(msg_type);
				case CAN_DATA_PE6:
					sys.data.chan.batt_volt = ch2word(pkt->data[1],pkt->data[0]);
					sys.data.chan.air_temp = ch2word(pkt->data[3],pkt->data[2]);
					sys.data.chan.wtr_temp = ch2word(pkt->data[5],pkt->data[4]);
					return(msg_type);
				case CAN_DATA_PE7:
					//sys.data.chan.therm5 = ch2word(pkt->data[1],pkt->data[0]);
					sys.data.chan.oil_temp = ch2word(pkt->data[3],pkt->data[2]);
					sys.data.prm.ver_mjr = pkt->data[4];
					sys.data.prm.ver_min = pkt->data[5];
					sys.data.prm.ver_bld = pkt->data[6];
					return(msg_type);
				case CAN_DATA_TCS1:
					sys.data.status.TCS_status = pkt->data[0];
					//sys.data.status.trac_status = pkt->data[1];
					//sys.data.chan.lnc_status = pkt->data[2];
					sys.data.prm.lnc_dsb_spd = ch2word(pkt->data[3],pkt->data[4]);
					return(msg_type);
				case CAN_DATA_TCS2:
					sys.data.chan.fuel_slip_err = ch2word(pkt->data[0],pkt->data[1]);
					sys.data.chan.fuel_comp = ch2word(pkt->data[2],pkt->data[3]);
					sys.data.chan.timing_err = ch2word(pkt->data[4],pkt->data[5]);
					sys.data.chan.timing_comp = ch2word(pkt->data[6],pkt->data[7]);
					return(msg_type);
				case CAN_DATA_TCS3:
					sys.data.chan.sw_angle = ch2word(pkt->data[0],pkt->data[1]);
					//sys.data.chan.tcs_a1 = ch2word(pkt->data[2],pkt->data[3]);
					//sys.data.chan.tcs_a2 = ch2word(pkt->data[4],pkt->data[5]);
					//sys.data.chan.tcs_a3 = ch2word(pkt->data[6],pkt->data[7]);
					return(msg_type);
				case CAN_DATA_TCS4:
					sys.data.prm.fuel_kp = pkt->data[0];
					sys.data.prm.fuel_kd = pkt->data[1];
					sys.data.prm.fuel_ki = pkt->data[2];
					sys.data.prm.timing_kp = pkt->data[3];
					sys.data.prm.timing_kd = pkt->data[4];
					sys.data.prm.timing_ki = pkt->data[5];
					return(msg_type);
				case CAN_DATA_IMU1:
					sys.data.status.IMU_status = pkt->data[0];
					sys.data.prm.x_acl_comp = ch2word(pkt->data[1],pkt->data[2]);
					sys.data.prm.y_acl_comp = ch2word(pkt->data[3],pkt->data[4]);
					sys.data.prm.z_acl_comp = ch2word(pkt->data[5],pkt->data[6]);
					return(msg_type);
				case CAN_DATA_IMU2:
					sys.data.chan.acl_x = ch2word(pkt->data[0],pkt->data[1]);
					sys.data.chan.acl_y = ch2word(pkt->data[2],pkt->data[3]);
					sys.data.chan.acl_z = ch2word(pkt->data[4],pkt->data[5]);
					return(msg_type);
				case CAN_DATA_IMU3:
					sys.data.chan.gyro_x = ch2word(pkt->data[0],pkt->data[1]);
					sys.data.chan.gyro_y = ch2word(pkt->data[2],pkt->data[3]);
					sys.data.chan.gyro_z = ch2word(pkt->data[4],pkt->data[5]);
					return(msg_type);
				case CAN_DATA_IMU4:
					//sys.data.chan.mag_x = ch2word(pkt->data[0],pkt->data[1]);
					//sys.data.chan.mag_y = ch2word(pkt->data[2],pkt->data[3]);
					//sys.data.chan.mag_z = ch2word(pkt->data[4],pkt->data[5]);
					//sys.data.chan.mag_heading = ch2word(pkt->data[6],pkt->data[7]);
					return(msg_type);
				case CAN_DATA_STS1:
					sys.data.status.STS_status = ch2word(pkt->data[0],pkt->data[1]);
					return(msg_type);
				case CAN_DATA_STS2:
					sys.data.chan.fl_susp_trvl = ch2word(pkt->data[0],pkt->data[1]);
					sys.data.chan.fr_susp_trvl = ch2word(pkt->data[2],pkt->data[3]);
					sys.data.chan.rl_susp_trvl = ch2word(pkt->data[4],pkt->data[5]);
					sys.data.chan.rr_susp_trvl = ch2word(pkt->data[6],pkt->data[7]);
					return(msg_type);
				case CAN_DATA_STS3:
					sys.data.chan.ttemp_fli = ch2word(pkt->data[0],pkt->data[1]);
					sys.data.chan.ttemp_fri = ch2word(pkt->data[2],pkt->data[3]);
					sys.data.chan.ttemp_rli = ch2word(pkt->data[4],pkt->data[5]);
					sys.data.chan.ttemp_rri = ch2word(pkt->data[6],pkt->data[7]);
					return(msg_type);
				case CAN_DATA_STS4:
					sys.data.chan.ttemp_flm = ch2word(pkt->data[0],pkt->data[1]);
					sys.data.chan.ttemp_frm = ch2word(pkt->data[2],pkt->data[3]);
					sys.data.chan.ttemp_rlm = ch2word(pkt->data[4],pkt->data[5]);
					sys.data.chan.ttemp_rrm = ch2word(pkt->data[6],pkt->data[7]);
					return(msg_type);
				case CAN_DATA_STS5:
					sys.data.chan.ttemp_flo = ch2word(pkt->data[0],pkt->data[1]);
					sys.data.chan.ttemp_fro = ch2word(pkt->data[2],pkt->data[3]);
					sys.data.chan.ttemp_rlo = ch2word(pkt->data[4],pkt->data[5]);
					sys.data.chan.ttemp_rro = ch2word(pkt->data[6],pkt->data[7]);
					return(msg_type);
				case CAN_DATA_DLG1:
					sys.data.status.DLG_status = pkt->data[0];
					sys.data.prm.hmsphr = pkt->data[1];
					sys.data.prm.date = ch2word(pkt->data[4],pkt->data[5]);
					return(msg_type);
				case CAN_DATA_DLG2:
					sys.data.chan.lat = ch2dblword(pkt->data[0],pkt->data[1],pkt->data[2],pkt->data[3]);
					sys.data.chan.lng = ch2dblword(pkt->data[4],pkt->data[5],pkt->data[6],pkt->data[7]);
					return(msg_type);
				case CAN_DATA_DLG3:
					sys.data.chan.heading = ch2word(pkt->data[0],pkt->data[1]);
					sys.data.chan.speed = pkt->data[2];
					//sys.data.chan.time = ch2dblword(pkt->data[3],pkt->data[4],pkt->data[5],pkt->data[6]);
					return(msg_type);
					/*
				case CAN_DATA_DLG4:
					system.dont_log.log_status = pkt->data[0];
					for(i = 0;i<7;i++)
					{
						system.dont_log.log_name[i] = pkt->data[i+1];
					}
					system.dont_log.log_name[7] = 0;
					return(msg_type);
					*/
				case CAN_DATA_STW1:
					sys.data.status.STW_status = pkt->data[0];
					return(msg_type);
				case CAN_DATA_WRL1:
					sys.data.status.WRL_status = pkt->data[0];
					return(msg_type);
				default:
					return(0);

			}
			break;

		default:
			return(0);
	}
	return(0);
}

uint8_t print_disk_info(const struct fat_fs_struct* fs)
{
    if(!fs)
        return 0;

    struct sd_raw_info disk_info;
    if(!sd_raw_get_info(&disk_info))
        return 0;

    uart_puts_p(PSTR("manuf:  0x")); uart_putc_hex(disk_info.manufacturer); uart_puts("\n\r");uart_putc('\r');
    uart_puts_p(PSTR("oem:    ")); uart_puts((char*) disk_info.oem); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("prod:   ")); uart_puts((char*) disk_info.product); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("rev:    ")); uart_putc_hex(disk_info.revision); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("serial: 0x")); uart_putdw_hex(disk_info.serial); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("date:   ")); uart_putw_dec(disk_info.manufacturing_month); uart_putc('/');
                                   uart_putw_dec(disk_info.manufacturing_year); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("size:   ")); uart_putdw_dec(disk_info.capacity / 1024 / 1024); uart_puts_p(PSTR("MB\n"));
    uart_puts_p(PSTR("copy:   ")); uart_putw_dec(disk_info.flag_copy); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("wr.pr.: ")); uart_putw_dec(disk_info.flag_write_protect_temp); uart_putc('/');
                                   uart_putw_dec(disk_info.flag_write_protect); uart_putc('\n');uart_putc('\r');
    uart_puts_p(PSTR("format: ")); uart_putw_dec(disk_info.format); uart_putc('\n');uart_putc('\r');
    //uart_puts_p(PSTR("free:   ")); uart_putdw_dec(fat_get_fs_free(fs)); uart_putc('/');
      //                             uart_putdw_dec(fat_get_fs_size(fs)); uart_putc('\n');uart_putc('\r');

    return 1;
}
