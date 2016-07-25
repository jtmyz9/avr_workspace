/*
 * IMU.c
 *
 *  Created on: Feb 4, 2012
 *      Author: Administrator
 */

#include "IMU.h"

int main(void)
{
	DDRD|=_BV(PD7);
	DDRE|=_BV(PE6);

	uint8_t i;

	stdout = stdin = &uart_str;
	uart_init(UART1_BAUD);
	i2c_init(50000UL);
	can_init();

	uint8_t resp;

	InitCanMsgs();
	printf("Start\n");

	if((resp = acl_init())!= 0)	printf("ACL initialization error: %u\n",resp);

	sts_init();

	sei();

	systimer_10ms_restart(10);

	while(1)
	{
			acl_get_values();
			_delay_us(100);
			sts_get_values();
			_delay_us(100);
			//printf("%u %u %u %u\n",susp_trvl[0],susp_trvl[1],susp_trvl[2],susp_trvl[3]);

//			if(i == 0)	temp_get_value(TMP_FLI_ADDR,TMP_FLI);
//			if(i == 1)	temp_get_value(TMP_FLM_ADDR,TMP_FLM);
//			if(i == 2)	temp_get_value(TMP_FLO_ADDR,TMP_FLO);
//
//			if(i == 3)	temp_get_value(TMP_FRI_ADDR,TMP_FRI);
//			if(i == 4)	temp_get_value(TMP_FRM_ADDR,TMP_FRM);
//			if(i == 5)	temp_get_value(TMP_FRO_ADDR,TMP_FRO);
//
//			if(i == 6)	temp_get_value(TMP_RLI_ADDR,TMP_RLI);
//			if(i == 7)	temp_get_value(TMP_RLM_ADDR,TMP_RLM);
//			if(i == 8)	temp_get_value(TMP_RLO_ADDR,TMP_RLO);
//
//			if(i == 9)	temp_get_value(TMP_RRI_ADDR,TMP_RRI);
//			if(i == 10)	temp_get_value(TMP_RRM_ADDR,TMP_RRM);
//			if(i == 11)	temp_get_value(TMP_RRO_ADDR,TMP_RRO);

	}
}

void handle_10ms_tick(void)
{
	if(systimer_10ms %10 == 0)
	{
		imu2.data[0]=(acl_x >> 8) & 0x00FF;
		imu2.data[1]=(acl_x & 0x00FF);
		imu2.data[2]=(acl_y >> 8) & 0x00FF;
		imu2.data[3]=(acl_y & 0x00FF);
		imu2.data[4]=(acl_z >> 8) & 0x00FF;
		imu2.data[5]=(acl_z & 0x00FF);

		can_tx(get_open_mob(),&imu2);

		sts2.data[0]=(susp_trvl[FL_SUSP_TRVL-1] >> 8) & 0x00FF;
		sts2.data[1]=(susp_trvl[FL_SUSP_TRVL-1] & 0x00FF);
		sts2.data[2]=(susp_trvl[FR_SUSP_TRVL-1] >> 8) & 0x00FF;
		sts2.data[3]=(susp_trvl[FR_SUSP_TRVL-1] & 0x00FF);
		sts2.data[4]=(susp_trvl[RL_SUSP_TRVL-1] >> 8) & 0x00FF;
		sts2.data[5]=(susp_trvl[RL_SUSP_TRVL-1] & 0x00FF);
		sts2.data[6]=(susp_trvl[RR_SUSP_TRVL-1] >> 8) & 0x00FF;
		sts2.data[7]=(susp_trvl[RR_SUSP_TRVL-1] & 0x00FF);

		can_tx(get_open_mob(),&sts2);
	}

	if(systimer_10ms %20 == 0)
	{
		// Send 50Hz Data
		//led_ctrl(LED2,LED_ON,LED_NO_TMR);
	}

	if(systimer_10ms % 50 == 0)
	{
		// Send 20Hz Data
		//led_ctrl(LED3,LED_ON,LED_NO_TMR);
	}

	if(systimer_10ms % 100 == 0)
	{
		// Send 10Hz Data
		sts3.data[1] = temps[TMP_FLI];
		sts3.data[3] = temps[TMP_FRI];
		sts3.data[5] = temps[TMP_RLI];
		sts3.data[7] = temps[TMP_RRI];

		sts4.data[1] = temps[TMP_FLM];
		sts4.data[3] = temps[TMP_FRM];
		sts4.data[5] = temps[TMP_RLM];
		sts4.data[7] = temps[TMP_RRM];

		sts5.data[1] = temps[TMP_FLO];
		sts5.data[3] = temps[TMP_FRO];
		sts5.data[5] = temps[TMP_RLO];
		sts5.data[7] = temps[TMP_RRO];

		can_tx(get_open_mob(),&sts3);
		can_tx(get_open_mob(),&sts4);
		can_tx(get_open_mob(),&sts5);

	}

	if(systimer_10ms % 200 == 0)
	{
		// Send 5Hz Data
		//led_ctrl(LED5,LED_ON,LED_NO_TMR);
	}
	if(systimer_10ms % 500 == 0)
	{
		// Send 2Hz Data
		//led_ctrl(LED6,LED_ON,LED_NO_TMR);
	}
	if(systimer_10ms % 1000 == 0)
	{
		// Send 1Hz Data
		//led_ctrl(LED7,LED_ON,LED_NO_TMR);
		can_tx(get_open_mob(),&sts5);
	}
}

//CAN interrupt service routine
ISR(CANIT_vect)
{
	unsigned char i;
	CAN_packet packet;
	unsigned char save_canpage;
	unsigned char mob;
	unsigned char length;

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
	/*---------------------------------------------*/
}

void InitCanMsgs(void)
{
	uint8_t i;
	imu2.id_type = CAN_EXT_MSG;
	imu2.id = CAN_DATA_IMU2;
	imu2.id = (imu2.id << 8);
	imu2.length = 6;
	for(i = 0;i<8;i++)
	{
		imu2.data[i] = 0;
	}

	sts3.id_type = CAN_EXT_MSG;
	sts3.id = CAN_DATA_STS3;
	sts3.id = (sts3.id << 8);
	sts3.length = 8;
	for(i = 0;i<8;i++)
	{
		sts3.data[i] = 0;
	}

	sts4.id_type = CAN_EXT_MSG;
	sts4.id = CAN_DATA_STS4;
	sts4.id = (sts4.id << 8);
	sts4.length = 8;
	for(i = 0;i<8;i++)
	{
		sts4.data[i] = 0;
	}

	sts5.id_type = CAN_EXT_MSG;
	sts5.id = CAN_DATA_STS5;
	sts5.id = (sts5.id << 8);
	sts5.length = 8;
	sts5.msg_type = CAN_TX_MSG;
	for(i = 0;i<8;i++)
	{
		sts5.data[i] = 0;
	}

	sts2.id_type = CAN_EXT_MSG;
	sts2.id = CAN_DATA_STS2;
	sts2.id = (sts2.id << 8);
	sts2.length = 8;
	sts2.msg_type = CAN_TX_MSG;
	for(i = 0;i<8;i++)
	{
		sts2.data[i] = 0;
	}


}

uint8_t ProcessCanMsg(CAN_packet *pkt)
{
	return 0;
}
