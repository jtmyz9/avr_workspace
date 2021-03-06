

#include "can.h"

uint8_t can_tx( char mob, 	volatile CAN_packet *packet)
{
	unsigned cnt;

    CANPAGE = mob << 4;

	if((CANCDMOB & 0b11000000) && ((CANSTMOB & 0x40)==0))
	{
		return(0);
	}

	if(msg_type_lu[mob]!=0)
	{
		return(0);
	}

	CANSTMOB = 0x00;    	// cancel pending operation
	CANCDMOB = 0x00;

	if(packet->id_type == CAN_STD_MSG)
	{
		CANIDT1=packet->id >>3;
		CANIDT2=packet->id <<5;
		CANIDT3 = 0;
		CANIDT4 = 0;
	}
	else
	{
		CANIDT1 = 0x000000FF & (packet->id >> 21);
		CANIDT2 = 0x000000FF & packet->id >>13;
		CANIDT3 = 0x000000FF & packet ->id >> 5;
		CANIDT4 = 0x000000FF & packet->id << 3;
		CANCDMOB |= _BV(IDE);
	}

	for (cnt=0; cnt < packet->length; ++cnt)
	{
		CANMSG = packet->data[cnt];
	}

	CANCDMOB|=packet->length;

	msg_type_lu[mob] = CAN_TX_MSG;

	uint16_t canie = (1<<mob);

	CANIE2 |= (uint8_t)(0x00FF  & canie);
	CANIE1 |= (uint8_t)(0x00FF & (canie >> 8));

    CANCDMOB|=0x40; //enable TX
	return(1);
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
		CANIDT1 = (uint8_t)((tag >>21) & 0x000000FF);
		CANIDT2 = (uint8_t)((tag >>13) & 0x000000FF);
		CANIDT3 = (uint8_t)((tag >> 5) & 0x000000FF);
		CANIDT4 = (uint8_t)((tag << 3) & 0x000000FF);
		CANIDM1 = (uint8_t)((mask >> 21) & 0x000000FF);
		CANIDM2 = (uint8_t)((mask >> 13) & 0x000000FF);
		CANIDM3 = (uint8_t)((mask >> 5) & 0x000000FF);
		CANIDM4 = (uint8_t)((mask << 3) & 0x000000FF);
		//CANIDM4|= 0x01; // Set IDE mask
		CANCDMOB= 0x80|_BV(IDE);
	}
	mob_mask = 1<<mob;
	CANIE2 |= mob_mask;
	CANIE1 |= (mob_mask>>8);

	return(1);
}

void can_init( void)
{
	unsigned mob;

	if(F_CPU == 16000000)
	{
		// For 16MHz gives 250K Baud
		CANBT1 = 0x06;
		CANBT2 = 0x0C;
		CANBT3 = 0x37;
	}

	for (mob = 0; mob < NO_MOBS; mob++)
	{
		CANPAGE  = (mob << 4);
		CANSTMOB = 0;
		CANCDMOB = 0;
	}
	CANGCON |= 0x02;
	CANGIE=(1<<ENIT) | (1<< ENRX)|(1 << ENTX);

	while(!(CANGSTA & _BV(ENFG)));
}

uint8_t get_open_mob(void)
{
	uint8_t i = 0;
	for(i=0;i<NO_MOBS;i++)
	{
		if(msg_type_lu[i] == 0)
		{
			return(i);
		}
	}
	return(0xFF);
}


