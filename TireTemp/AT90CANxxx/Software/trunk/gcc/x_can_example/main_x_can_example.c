//
//***** main_x_can_example.c ***************************************************

#include "config.h"
#include "can_lib.h"

int main (void)
{

st_cmd_t      x_can_message;        // CAN message descriptor
unsigned char x_can_buffer[8];      // CAN message buffer

  // Initialization
    can_init(0);
    x_can_message.pt_data = &x_can_buffer[0];

    while(1)
    {
      // RxCAN
        x_can_message.cmd = CMD_RX;    // Enable Rx
        while(can_cmd(&x_can_message) != CAN_CMD_ACCEPTED);
        // Wait for Rx completed
        while(can_get_status(&x_can_message) == CAN_STATUS_NOT_COMPLETED);
      // TxCAN
        x_can_message.id.std++;        // ID Incrementation
        x_can_message.cmd = CMD_TX;    // Enable Tx            
        while(can_cmd(&x_can_message) != CAN_CMD_ACCEPTED);
        // Wait for Tx completed        
        while(can_get_status(&x_can_message) == CAN_STATUS_NOT_COMPLETED);
    }
    return 0;
}
