//***************************************************************************
//! @file $RCSfile: uart_noit_lib.h,v $
//!
//! Copyright (c) 2005 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief UART without interrupt library include file.
//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//***************************************************************************

#ifndef _UART_NOIT_LIB_H_
#define _UART_NOIT_LIB_H_

//_____ I N C L U D E S ____________________________________________________

#include "config.h"

//_____ D E F I N I T I O N S ______________________________________________

#ifndef FOSC
        #error You must define FOSC in config.h
#endif

#ifndef USE_UART
        #error You must define USE_UART to 0 or 1
    #elif USE_UART == 0
        #define UBRRH_N   (UBRR0H)
        #define UBRRL_N   (UBRR0L)                   
        #define UCSRA_N   (UCSR0A)
          #define UDRE_N  (UDRE0)
          #define RXC_N   (RXC0)
        #define UCSRB_N   (UCSR0B)
          #define USBS_N  (USBS0)
          #define UCSZ1_N (UCSZ01)
          #define UCSZ0_N (UCSZ00)
        #define UCSRC_N   (UCSR0C)
          #define RXEN_N  (RXEN0)
          #define TXEN_N  (TXEN0)
        #define UDR_N     (UDR0)
    #elif USE_UART == 1
        #define UBRRH_N   (UBRR1H)
        #define UBRRL_N   (UBRR1L)                   
        #define UCSRA_N   (UCSR1A)
          #define UDRE_N  (UDRE1)
        #define UCSRB_N   (UCSR1B)
          #define USBS_N  (USBS1)
          #define UCSZ1_N (UCSZ11)
          #define UCSZ0_N (UCSZ10)
        #define UCSRC_N   (UCSR1C)
          #define RXEN_N  (RXEN1)
          #define TXEN_N  (TXEN1)
        #define UDR_N     (UDR1)
    #else
        #error Highest USE_UART definition
#endif

//_____ M A C R O S ________________________________________________________

//***************************************************************************
//! Set UART baudrate.
//!
//! @param bdr   (c.f. Examples of baud rate setting in device data sheet)
//***************************************************************************
#define Uart_set_baudrate(bdr)  ( UBRRH_N = (U8)((((U32)FOSC*1000)/((U32)bdr*16)-1)>>8),\
                                  UBRRL_N = (U8)(( (U32)FOSC*1000)/((U32)bdr*16)-1)    )

//_____ D E C L A R A T I O N S ____________________________________________

/*_____ P R O T O T Y P E S - D E C L A R A T I O N __________________________*/

void uart_init(void);
U8 get_char_uart(void);
void put_char_uart(U8 char_2_send);
void c_printf_uart(U8 *c_string);
void d_printf_uart(U8 *d_string);

//***************************************************************************

#endif  /* _UART_NOIT_LIB_H_ */
