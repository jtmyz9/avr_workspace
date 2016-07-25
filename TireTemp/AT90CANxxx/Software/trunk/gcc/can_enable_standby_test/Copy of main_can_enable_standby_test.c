//
//***** main_can_enable_standby_test.c *****************************************

#include "config.h"
#include "can_lib.h"

int main (void)
{

// CAN message descriptors
st_cmd_t  c001_can_message;
st_cmd_t  c020_can_message;
st_cmd_t  c300_can_message;
// CAN message buffer
U8        c001_can_buffer[4] = {0x00,0x00,0x00,0x00};
U8        c020_can_buffer[4] = {0x20,0x21,0x22,0x23};
U8        c300_can_buffer[4] = {0x00,0x00,0x00,0x00};
// Flags
U8      i, c001_flag = 0;
U8      j, c020_flag = 0;
U8         c300_flag = 0;
U8      ena_count = 0;
// Counter
U32     u32_count = 0;


  // Initialization
    DDRB |= 1<<PB0; PORTB &= ~(1<<PB0); // PB0 or /SS  or J23-2 used as Busy flag
    DDRB |= 1<<PB1; PORTB &= ~(1<<PB1); // PB1 or SCK  or J23-4 used as Tic  flag
    DDRB |= 1<<PB2; PORTB &= ~(1<<PB2); // PB2 or MOSI or J23-5 used as ENA  flag

    can_init(0);
    PORTB |= 1<<PB2;    // PB2-ENA flag = 1

    for (j=0;j<15;j++) // Clear MOb Data
    {
        CANPAGE = j<<4;
        for (i=0;i<8;i++) CANMSG = 0;
    }
    
  // CAN message ID=111
    c001_can_message.id.std = 0x001;
    c001_can_message.ctrl.ide = 0;
    c001_can_message.pt_data = &c001_can_buffer[0];
    c001_can_message.dlc = 4;
  // CAN message ID=222
    c020_can_message.id.std = 0x020;
    c020_can_message.ctrl.ide = 0;
    c020_can_message.pt_data = &c020_can_buffer[0];
    c020_can_message.dlc = 4;
  // CAN message ID=333
    c300_can_message.id.std = 0x300;
    c300_can_message.ctrl.ide = 0;
    c300_can_message.pt_data = &c300_can_buffer[0];
    c300_can_message.dlc = 4;

  // Prepare messages
    c001_can_message.cmd = CMD_RX_DATA_MASKED;
    c020_can_message.cmd = CMD_TX_DATA;
    c300_can_message.cmd = CMD_RX_DATA_MASKED;
  // Enable Rx CAN message ID=001
    while (can_cmd(&c001_can_message) != CAN_CMD_ACCEPTED);  c001_flag = 1;
  // Enable Rx CAN message ID=300
    while (can_cmd(&c300_can_message) != CAN_CMD_ACCEPTED);  c300_flag = 1;
        
  // Wait for Rx completed
    while(1)
    {
        if((CANGSTA & (1<<RXBSY)) != 0)
        {
            PORTB |= 1<<PB0;    // PB0-Busy flag = 1
            ena_count=1;
        }
        else PORTB &= ~(1<<PB0);                             // PB0-Busy flag = 0
        
        if(ena_count != 0)
        {
            u32_count++;
            PINB |= 1<<PB1;     // Toggle PB1-Tic flag
        }

        if(can_get_status(&c001_can_message) == CAN_STATUS_COMPLETED) c001_flag = 2;
        if(can_get_status(&c020_can_message) == CAN_STATUS_COMPLETED) c020_flag = 2;
        if(can_get_status(&c300_can_message) == CAN_STATUS_COMPLETED) c300_flag = 2;
        
        if(u32_count == 02)   // ID field                
//        if(u32_count == 06)   // DLC field                 
//        if(u32_count == 10)   // Data field                 
        {
            Can_disable();                  
            u32_count++;
            PINB |= 1<<PB2;     // Toggle PB2-ENA flag
            // Enable Tx CAN message ID=020
            while (can_cmd(&c020_can_message) != CAN_CMD_ACCEPTED);  c020_flag = 1;
        }
        if(u32_count == 20)
        {
            Can_enable();                  
            u32_count++;
            PINB |= 1<<PB2;     // Toggle PB2-ENA flag
        }
    }
    return 0;
}
