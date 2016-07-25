/*!	\file
	generic CAN bus driver for AT90CAN128

	privides interrupt-driven reception and polled transmission of can packets

	\author Dr. Klaus Schaefer \n
	Hochschule Darmstadt * University of Applied Sciences \n
	schaefer@eit.h-da.de \n
	http://kschaefer.eit.h-da.de

	You can redistribute it and/or modify it
	under the terms of the GNU General Public License.\n
	It is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; \n
	without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n
	See http://www.gnu.org/copyleft/gpl.html for mor details.

*/

#include "can.h"

//! CAN interrupt service routine
ISR( CANIT_vect)
{
	unsigned char i;
	CAN_packet packet;
	unsigned char save_canpage;
	unsigned char mob;
	unsigned char length;

	PORTC &= ~_BV(PC7);

	mob=CANHPMOB; // get highest prio mob

	save_canpage = 0xF0 | mob;

    CANPAGE = mob & 0xf0;
	mob >>= 4; // -> mob number 0..15

	packet.msg_type = msg_type_lu[mob];

	if(CANCDMOB & _BV(IDE))
	{
		packet.id_type = CAN_EXT_MSG;
		packet.id = 	(((uint32_t)CANIDT1) << 21)
					| (((uint32_t)CANIDT2) << 13)
					| (((uint32_t)CANIDT3) << 5)
					| (((uint32_t)CANIDT4) >> 3);
	}

	//packet.id=(CANIDT2>>5) | (CANIDT1 <<3);
	length=CANCDMOB & 0x0f;
	packet.length=length;

	for (i = 0; i <length; ++i)
	{
		packet.data[i] = CANMSG;
	}

	ProcessCanMsg(&packet);

	CANSTMOB=0;		// reset INT reason
	CANCDMOB &= 0x80|_BV(IDE);	// re-enable RX on this channel

	CANPAGE=save_canpage;	// restore CANPAGE

	PORTC |= _BV(PC7);
}

uint8_t can_tx( char mob, 	CAN_packet *packet)
{
	unsigned cnt;
	ASSERT( packet->id <= 0x7ff);
	ASSERT( packet->length <=8);
	ASSERT( mob <=14);
    CANPAGE = mob << 4;

	if((CANCDMOB & 0b11000000) && ((CANSTMOB & 0x40)==0))
	{
		return(0);
	}

	CANSTMOB = 0x00;    	// cancel pending operation
	CANCDMOB = 0x00;

	CANIDT1=packet->id >>3;
	CANIDT2=packet->id <<5;
	CANIDT3 = 0;
	CANIDT4 = 0;

	for (cnt=0; cnt < packet->length; ++cnt)
	{
		CANMSG = packet->data[cnt];
	}

	CANCDMOB|=packet->length;

    CANCDMOB|=0x40; //enable TX
	return(1);
}

void can_rx(void)
{

	CANPAGE = 0;
	CANSTMOB = 0;
	CANCDMOB = 0;

	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = 0x00;
	CANIDT1 = 0x00;

	CANIDM4 = 0x00;
	CANIDM3 = 0x00;
	CANIDM2 = 0x00;
	CANIDM1 = 0x00;

	// Enable Reception
	CANCDMOB =0x80;

	while(!(CANSTMOB & _BV(RXOK)));
}


uint8_t prepare_rx( uint8_t mob, uint32_t tag, uint32_t mask, uint8_t id_type,uint8_t msg_type)
{
	uint16_t mob_mask = 0;

	if( mob >= 15) 				// illegal MOB number
	{
		return(0);
	}


    CANPAGE = mob << 4;

	if( CANCDMOB & 0b11000000) 	// if MOB already in use
	{
		return(0);			// no vacancy ...
	}

	msg_type_lu[mob] = msg_type;

	CANSTMOB = 0x00;    	// cancel pending operation
	CANCDMOB = 0x00;
	CANHPMOB = 0x00;		// enable direct mob indexing, see docu

	if(id_type == CAN_EXT_MSG)
	{
		CANIDT4 = tag >>21;
		CANIDT3 = tag >>13;
		CANIDT2=tag >> 5;
		CANIDT1=(tag << 3) & 0xF8;
		CANIDM4=mask >> 21;
		CANIDM3=mask >> 13;
		CANIDM2=mask >> 5;
		CANIDM1=(mask << 3) & 0xF8;
		CANIDM|= 0x01; // Set IDE mask
		CANCDMOB = 0x80|_BV(IDE);
	}
	mob_mask = 1<<mob;
	CANIE2 |= mob_mask;
	CANIE1 |= (mob_mask>>8);

	return(1);
}


void can_init( void)
{
	unsigned mob;

	// For 16MHz gives 250K Baud
	CANBT1 = 0x06;
	CANBT2 = 0x0C;
	CANBT3 = 0x37;

	for (mob = 0; mob < NO_MOBS; mob++)
	{
		CANPAGE  = (mob << 4);
		CANSTMOB = 0;
		CANCDMOB = 0;
	}
	CANGCON |= 0x02;
	CANGIE=(1<<ENIT) | (1<< ENRX);
}

