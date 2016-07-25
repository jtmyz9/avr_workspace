/*
 * TireTemp.c
 *
 * Created: 10/29/2015 11:05:03 PM
 *  Author: SexPanther
 */ 

#include <avr/io.h>
#include "can_lib.h"

int main(){
	
	can_init(0);
	uint8_t buffer[4] = {11,22,33,44};
	can_ctrl_t ctrl;
	ctrl.ide = 1;
	ctrl.rtr = 1;
	st_cmd_t msg;
	msg.cmd = CMD_TX;
	msg.ctrl = ctrl;
	msg.dlc = 4;
	msg.pt_data = buffer;
	can_id_t id;
	id.ext = 0;
	id.std = 0x123;
	msg.id = id;
	
	while(1){
		//get shit done
		can_cmd(&msg);
	}
}