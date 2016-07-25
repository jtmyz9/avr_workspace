//
//***** main_it_can_test.c **************************************************

#include "config.h"
#include "can_it_lib.h"

int main (void)
{

st_cmd_t      x_can_message;        // CAN message descriptor
st_cmd_t      y_can_message;        // CAN message descriptor
st_cmd_t      z_can_message;        // CAN message descriptor

unsigned char x_can_buffer[8] = {0,0,0,0,0,0,0,0};      // CAN message buffer
unsigned char y_can_buffer[8] = {0,0,0,0,0,0,0,0};      // CAN message buffer
unsigned char z_can_buffer[8] = {0,0,0,0,0,0,0,0};      // CAN message buffer

  // Initialization
    can_init(0);
    x_can_message.pt_data = &x_can_buffer[0];
    y_can_message.pt_data = &y_can_buffer[0];
    z_can_message.pt_data = &z_can_buffer[0];

    while(1)
    {
    // Init MOB's for reception
        // For x_...
		x_can_message.cmd = CMD_RX;    // Enable Rx
        while(can_cmd(&x_can_message) != CAN_CMD_ACCEPTED);
        // For y_...
		y_can_message.cmd = CMD_RX;    // Enable Rx
        while(can_cmd(&y_can_message) != CAN_CMD_ACCEPTED);
        // For z_...
		z_can_message.cmd = CMD_RX;    // Enable Rx
        while(can_cmd(&z_can_message) != CAN_CMD_ACCEPTED);


    // Wait for Rx completed on x_...
        while(x_can_message.status == MOB_PENDING);
        // Then TxCAN
        if((x_can_message.status == MOB_RX_COMPLETED)||(x_can_message.status == MOB_RX_COMPLETED_DLCW))
		{
			x_can_message.id.std++;        // ID Incrementation
        	x_can_message.cmd = CMD_TX;    // Enable Tx            
        	while(can_cmd(&x_can_message) != CAN_CMD_ACCEPTED);
        }

    // Wait for Rx completed on y_...
        while(y_can_message.status == MOB_PENDING);
        // Then TxCAN
        if((y_can_message.status == MOB_RX_COMPLETED)||(y_can_message.status == MOB_RX_COMPLETED_DLCW))
		{
			y_can_message.id.std++;        // ID Incrementation
        	y_can_message.cmd = CMD_TX;    // Enable Tx            
        	while(can_cmd(&y_can_message) != CAN_CMD_ACCEPTED);
		}

    // Wait for Rx completed on z_...
        while(z_can_message.status == MOB_PENDING);
        // Then TxCAN
        if((z_can_message.status == MOB_RX_COMPLETED)||(z_can_message.status == MOB_RX_COMPLETED_DLCW))
		{
			z_can_message.id.std++;        // ID Incrementation
        	z_can_message.cmd = CMD_TX;    // Enable Tx            
        	while(can_cmd(&z_can_message) != CAN_CMD_ACCEPTED);
		}


     // Wait for ALL Tx completed        
        while(x_can_message.status == MOB_PENDING);
        while(y_can_message.status == MOB_PENDING);
        while(z_can_message.status == MOB_PENDING);
    }
    return 0;
}
