#ifndef CAN_H_
#define CAN_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "assert.h"
#include "IMCP.h"

#define NO_MOBS 15 		//!< number of message object buffers (MOBs) (AT90CAN128)
#define NOMOB 	0xff 	//!< return value for getmob()
#define RTR		0xff 	//!< special value for length ->RTR packet

#define CAN_EXT_MSG 1
#define CAN_STD_MSG 0

//! can packet structure

//! used to describe a CAN20a (standard 11 bit) packet.
//! a packet length of 0 indicates an RTR packet
typedef struct
{
	uint32_t id;
	uint8_t id_type;
	uint8_t msg_type;
	unsigned char length;
	unsigned char data[8];
}
CAN_packet;

extern uint8_t ProcessCanMsg(CAN_packet *pkt);
extern void ConfigureCanRx(void);
void can_rx(void);
uint8_t prepare_rx( uint8_t mob, uint32_t tag, uint32_t mask, uint8_t id_type,uint8_t msg_type);
uint8_t can_tx( char mob, 	CAN_packet *packet);
void can_init( void);

uint8_t msg_type_lu[NO_MOBS];

#endif
