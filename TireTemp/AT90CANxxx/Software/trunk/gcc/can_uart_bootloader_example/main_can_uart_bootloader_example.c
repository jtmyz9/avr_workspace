//******************************************************************************
//! @file $RCSfile: main_can_uart_bootloader_example.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief: This application performs a CAN or UARTs boot loader for 
//!         AT90CAN128/64/32 with aut-bitrate, IAP (In Application Programming) 
//!         and API (Application Programming Interface).
//!
//! This boot loader is an example (application note like), the customer
//! needs to appropriate this code to fit exactly with its application and
//! its environment. Some specific calls (or jump) in customer application
//! are necessary to use correctly the IAP and API features.
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

//_____  I N C L U D E S _______________________________________________________
#include "config.h"
#include "can_isp_protocol.h"
#include "uart_isp_protocol.h"
#include "isp_lib.h"
#include "reduced_can_lib.h"
#include "uart_lib.h"
#include "can_drv.h"
#include "uart_drv.h"
#include "flash_boot_lib.h"
#include "flash_api_lib.h"

//_____ M A C R O S ____________________________________________________________

//_____ D E F I N I T I O N S __________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________

//_____ F U N C T I O N S ______________________________________________________

//------------------------------------------------------------------------------
//  @fn main
//!
//! Core of "main_can_uart_boot_loader.c".
//!
//! @brief: This application performs a CAN or UARTs boot loader for
//!         AT90CAN128/64/32 with auto-bitrate, IAP (In Application Programming)
//!         and API(Application Programming Interface).
//!
//! @warning: - Don't forget to full locate the boot loader in "Boot Loader
//!             Flash Section".
//!           - xxx_HWCB defined in "board.h" file
//!
//! @param none.
//!
//! @return Integer 0
//!
//------------------------------------------------------------------------------
int main (void)
{
    U8      u8_temp;
    U8      hwcb = FALSE;
    Bool    auto_b = 0;

//! --- First of all, disabling the Global Interrupt
    Disable_interrupt();

//! --- If comming from RESET then test of the HardWare Condition Bit
    if (MCUSR != 0)
    {
        //- Clear all reset flags
        MCUSR = 0;
        //- Get HWCB (HardWare Condition Bit)
        HWCB_PORT_DIR &= ~(1<<HWCB_INPUT_PIN);      // HWCB pin in input
        if (HWCB_PULLUP_ON == 1)
        {
            HWCB_PORT_OUT |= (1<<HWCB_INPUT_PIN);   // Pull-up on HWCB
        }
        hwcb = HWCB_PORT_IN;
        if (HWCB_ACTIVE_LEVEL == 0)
        {
            hwcb = ((~hwcb)&(1<<HWCB_INPUT_PIN));
        }

        //- Start application else start boot loader
        u8_temp = get_conf_byte(BSB);

        if ( ((u8_temp!=BSB_DEFAULT)&&(hwcb==0)) || ((u8_temp==BSB_DEFAULT)&&(hwcb!=0)) )
        {
            isp_jump_to ( (((U16)get_conf_byte(SA_H))<<8) | ((U16)get_conf_byte(SA_L)) );   // Start application
        }
        // else go to BOOT LOADER
    }

//! --- Open CAN communication

    can_communication_opened = 0;   // Not opened

    //- Pull-up on TxCAN & RxCAN one by one to use bit-addressing
    CAN_PORT_DIR &= ~(1<<CAN_OUTPUT_PIN); CAN_PORT_OUT |=  (1<<CAN_OUTPUT_PIN);
    CAN_PORT_DIR &= ~(1<<CAN_INPUT_PIN) ; CAN_PORT_OUT |=  (1<<CAN_INPUT_PIN) ;

#if (USE_UART == BOTH_UART)
    //- "uart_drv.h" macro-functions are not used to save some bytes !
             
    //- Pull-up on TxD & RxD / UART-0 one by one to use bit-addressing
    UART_0_PORT_DIR &= ~(1<<UART_0_OUTPUT_PIN); UART_0_PORT_OUT |= (1<<UART_0_OUTPUT_PIN);
    UART_0_PORT_DIR &= ~(1<<UART_0_INPUT_PIN) ; UART_0_PORT_OUT |= (1<<UART_0_INPUT_PIN) ;

    //- Pull-up on TxD & RxD / UART-1 one by one to use bit-addressing
    UART_1_PORT_DIR &= ~(1<<UART_1_OUTPUT_PIN); UART_1_PORT_OUT |= (1<<UART_1_OUTPUT_PIN);
    UART_1_PORT_DIR &= ~(1<<UART_1_INPUT_PIN) ; UART_1_PORT_OUT |= (1<<UART_1_INPUT_PIN) ;

    uart_selected = NO_UART;

#endif

#if ((USE_UART == UART_0) || (USE_UART == UART_1))

    //- Pull-up on TxD & RxD / UART
	Uart_tx_in_pu_on();
    Uart_rx_in_pu_on();

#endif

    while (1)
    {
        //- TEST OF ACTIVITY ON RXCAN ------------------------------------------
        if ((CAN_PORT_IN & (1<<CAN_INPUT_PIN)) == 0)
        {
            //- Reset CAN peripheral
    	    Can_reset();
    
    	    //- Set CAN Bit-timming
            if (get_conf_byte(EB) != 0xFF)
            {
                CANBT1 = get_conf_byte(BTC1);
                CANBT2 = get_conf_byte(BTC2);
                CANBT3 = get_conf_byte(BTC3);
            }
            else
            //- Loop on auto-bitrate until it is performed
            {
                while ((Can_bit_timing(auto_b))==0);        // c.f. macro in "can_drv.h"
            }
    
    	    //- Enable CAN peripheral
            can_clear_all_mob();                        // c.f. function in "can_drv.c"
            Can_enable();                               // c.f. macro in "can_drv.h"
    
            //- Initialization of CAN ISP PROTOCOL only if CAN communication NOT opened
            if(!can_communication_opened) can_isp_protocol_init();
    
            //- CAN ISP PROTOCOL execution
            while(can_isp_protocol_task());
            //- Exit with "0" only if CAN hardware error occurs
            auto_b = ((~auto_b)&0x01);      // Flip of "auto_b" for CAN
            
        } // if ((CAN_PORT_IN...

        //- TEST OF ACTIVITY ON RXD UARTs --------------------------------------
        
#if (USE_UART == BOTH_UART)

    	//- "uart_drv.h" macro-functions are not used to save some bytes !
        if (((UART_0_PORT_IN & UART_0_PORT_IN & UART_0_PORT_IN & (1<<UART_0_INPUT_PIN)) >> UART_0_INPUT_PIN ) == 0) uart_selected = UART_0;
        if (((UART_1_PORT_IN & UART_1_PORT_IN & UART_1_PORT_IN & (1<<UART_1_INPUT_PIN)) >> UART_1_INPUT_PIN ) == 0) uart_selected = UART_1;
        
        if (uart_selected != NO_UART)
        {                     
            //- Loop on auto-baudrate until it is performed
            while (uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE) == 0);
            //- UART acknowledge character
            uart_putchar('U');

            //- Initialization of UART ISP PROTOCOL
            uart_isp_protocol_init();
    
            //- UART ISP PROTOCOL execution
            while(uart_isp_protocol_task());
            //- CARE: no exit from 'uart_isp_protocol_task()'
            
        } // if (uart_selected...

#endif
        
#if ((USE_UART == UART_0) || (USE_UART == UART_1))

        if (Uart_rx_get_3_data() == 0)
        {            
            
			//- Loop on auto-baudrate until it is performed
            while (uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE) == 0);
            //- UART acknowledge character
            uart_putchar('U');

            //- Initialization of UART ISP PROTOCOL
            uart_isp_protocol_init();
    
            //- UART ISP PROTOCOL execution
            while(uart_isp_protocol_task());
            //- CARE: no exit from 'uart_isp_protocol_task()'

        } // if (Uart_rx_get_3_data...
		    
#endif
        
    } // while(1...
    
    return(0);

} // End of "main"

