#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "general.h"
#include "assert.h"
#include "can.h"
#include "uart.h"
#include <util/delay.h>
#include "IMCP.h"

#define CAN_MSG_TYPE_DATABRDCAST 0x01


volatile struct
{
	unsigned short rpm;
	signed int tps;
	signed int fuel_open_time;
	signed int ign_angle;
}data[2];

#define UART_BAUD 115200
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

uint8_t ProcessCanMsg(CAN_packet *pkt)
{

	uint8_t msg_type;

	// Get the Message Type
	msg_type = pkt->msg_type;
	// Clear the message type in the lookup table
	//can_msgtype_lu[mob] = CAN_MSGTYPE_NONE;

	printf("CAN ID: %04X%04X\n",(uint16_t)(pkt->id >> 16),(uint16_t)pkt->id);

	switch(msg_type)
	{
		case CAN_MSGTYPE_NONE:
			return(0);
		case CAN_MSGTYPE_DATABRDCST:

			switch(pkt->id)
			{
				case CAN_DATA_PE1:
					data[0].rpm = ((((uint16_t)(pkt->data[1])) & 0x00FF) << 8) + (uint16_t)pkt->data[0];
					data[0].tps = (int16_t)(((((uint16_t)(pkt->data[3])) & 0x00FF) << 8) + pkt->data[2]);
					data[0].fuel_open_time = (int16_t)(((((uint16_t)(pkt->data[5])) & 0x00FF) << 8) + pkt->data[4]);
					data[0].ign_angle = (int16_t)(((((uint16_t)(pkt->data[7])) & 0x00FF) << 8) + pkt->data[6]);
					return(msg_type);
					break;
				case CAN_DATA_PE2:
					return(0);
				default:
					return(0);
			}
			break;
		default:
			return(0);
	}


}

//! main program.

//! used to test and demonstrate the CAN rx+tx functionality \n
//! uncomment main() to use this version (without FreeRTOS)
int main (void)
//int main (void)
{
	stdout = stdin = &uart_str;

	//initialize UART1
	UCSR1A=_BV(U2X1);
	UCSR1B=0x98;
	UCSR1C=0x06;
	UBRR1 = (F_CPU / (8UL * UART_BAUD)) - 1;

	PORTC = 0xFF;
	DDRC = 0xFF;

	printf("\nStart\n-----\n");

	printf("Can Initializing...");
	can_init();
	printf("DONE\n\n");

	prepare_rx(0,0,0,CAN_EXT_MSG,CAN_MSGTYPE_DATABRDCST);

	sei();

	while(1)
	{
			printf("Rpm: %u Tps: %i FOT: %i IA: %i\n",data[0].rpm,data[0].tps,data[0].fuel_open_time,data[0].ign_angle);
			_delay_ms(500);
	}

}
