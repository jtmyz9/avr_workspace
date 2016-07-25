//***************************************************************************
//! @file $RCSfile: uart_noit_lib.c,v $
//!
//! Copyright (c) 2005 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief Low level routines for UARTs running without interrupt.
//!
//! These routines need some #define's of "board.h" file.
//!
//! @version $Revision: 319852 $ $Name: jtellier $ 
//!
//! @todo
//! @bug
//***************************************************************************
//-----------------------------------------------------------------------
// Exemple of "x_printf_uart" using
// ================================
//
// #define FOSC          8000        // in KHz
// #define BAUDRATE      9600        // in K bit/s
// #define USE_UART         0        // only UART-0 will be used !
//
// U8 s_printf[80];  // 80 elements max for UART buffer
//
// uart_init(); // Setup UART at BAUDRATE (9600) Baud
//
// c_printf_uart("Counting from 0 to 9: ");
//
// for (i=0; i<10; i++) s_printf[i]=0x30+i;    // Init UART buffer
// s_printf[i++]=0x0D;            // "\r" - carriage return
// s_printf[i++]=0x0A;            // "\n" - new line or line feed
// s_printf[i++]=0x00;            // Null - End of string
// d_printf_uart(s_printf);
// 
//------------------------------------------------------------------------

//_____  I N C L U D E S ___________________________________________________

#include "config.h"
#include "uart_noit_lib.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________


//***************************************************************************
//  @fn uart_init
//!
//! UART initialization. Initialize the UART refered by USE_UART definition
//! to BAUDRATE speed. 
//! Initialization details: 8-bit, no parity, 2 stop-bit & no interrupt.     
//!
//! @warning Set definition for BAUDRATE
//!
//! @param (none)
//!
//! @return (none)
//!
//***************************************************************************
void uart_init(void)
    {
    UCSRB_N = 0 ;                     // Disable UART
    Uart_set_baudrate(UART_BAUDRATE); // Macro in "uart_no_int_lib.h"
    UCSRA_N = (1<<UDRE_N);
    UCSRC_N = (1<<USBS_N) | (1<<UCSZ1_N) | (1<<UCSZ0_N);
    UCSRB_N = (1<<RXEN_N) | (1<<TXEN_N);  // then, (re)enable UART
    }

//***************************************************************************
//  @fn get_char_uart
//!
//! Get byte from RX UART. If UART has received a character, this character
//! is returned, if no character has been received, 0x00 is returned.
//!
//! @warning "uart_init()" must be performed before
//!
//! @param (none)
//!
//! @return Character from RX UART or 0x00 if nothing received
//!
//***************************************************************************
U8 get_char_uart (void)
    {
    U8  temp;
		
    if (!(UCSRA_N & (1<<RXC_N)))
        {
        temp = FALSE;
        }
        else
            {
            temp = UDR_N;
            }
    return temp;
    }

//***************************************************************************
//  @fn put_char_uart
//!
//! Put character on TX UART. If TxReady, the character to send on TX UART.
//!
//! @warning "uart_init()" must be performed before
//!
//! @param Character to send
//!
//! @return (none)
//!
//***************************************************************************
void put_char_uart (U8 char_2_send)
    {
    while ( !(UCSRA_N & (1<<UDRE_N)) ); // wait for empty transmit buffer 
    UDR_N = char_2_send;
    }

//***************************************************************************
//  @fn c_printf_uart
//!
//! Put a code-string on TX UART. The code-string is send up to null
//! character is found.
//!
//! @warning "uart_init()" must be performed before
//!
//! @param Pointer on U8 code-string
//!
//! @return (none)
//***************************************************************************
//void c_printf_uart (const U8 *c_string)
void c_printf_uart (U8 *c_string)
    {
    while(*c_string) put_char_uart (*c_string++);
    }

//***************************************************************************
//  @fn d_printf_uart
//!
//! Put a data-string on TX UART. The data-string is send up to null
//! character is found.
//!
//! @warning "uart_init()" must be performed before
//!
//! @param Pointer on U8 data-string
//!
//! @return (none)
//!
//***************************************************************************
void d_printf_uart (U8 *d_string)
    {
    while(*d_string) put_char_uart (*d_string++);
    }
