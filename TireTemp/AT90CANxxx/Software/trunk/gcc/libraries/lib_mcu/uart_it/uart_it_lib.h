//******************************************************************************
//! @file $RCSfile: uart_it_lib.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the prototypes and the macros of the
//!        library of functions of:
//!             - Both UARTs interrupt handling
//!             - for AT90CAN128/64/32.
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070525.
//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

#ifndef _UART_IT_LIB_H_
#define _UART_IT_LIB_H_

//_____ I N C L U D E S ________________________________________________________
#include "uart_it_drv.h"
#include <stdarg.h>

//_____ D E F I N I T I O N S __________________________________________________
    // ---------- Used in "uart_mini_printf".
#ifndef DATA_BUF_LEN
#define DATA_BUF_LEN   12       // If the Max number is an U32 = 0xFFFFFFFF
                                // (8 digits) or = 4294967295 (10 digits) so
                                // 12 is enough if sign with (and) "0" prefixes
                                // are used.
#endif

//_____ M A C R O S ____________________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________

//------------------------------------------------------------------------------
//  @fn uart_init
//!
//! UART peripheral initialization. Reset the UART, initialize the uart
//! mode, initialize the baudrate and enable the UART peripheral.
//!
//! @warning If autobaud, only one character is useful. If autobaud, one
//!          16-bit Timer is necessary.
//!
//! @param  Mode (c.f. predefined modes in "uart_drv.h" file)
//!         Baudrate (for fixed baudrate this param is not used)
//!
//! @return Baudrate Status
//!         ==0: research of timing failed
//!         ==1: baudrate performed
//!
extern U8 uart_init (U8 mode, U32 baudrate);

//------------------------------------------------------------------------------
//  @fn uart_test_hit
//!
//! Check if something has been received on the UART peripheral.
//!
//! @warning none
//!
//! @param  none
//!
//! @return Status
//!         ==0: Nothing has been received
//!         ==1: A character has been received
//!
extern U8 uart_test_hit (void);

//------------------------------------------------------------------------------
//  @fn uart_put_string
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
void uart_put_string (U8 *data_string);

//------------------------------------------------------------------------------
//  @fn uart_mini_printf
//!
//! Minimal "PRINTF" with variable argument list. Write several variables
//! formatted by a format string to a file descriptor.
//! Example:
//! ========
//! { u8_toto = 0xAA;
//!   uart_mini_printf ("toto = %04d (0x%012X)\r\n", u8_toto, u8_toto);
//!   /*   Expected:     toto = 0170 (0x0000000000AA)   &  Cr+Lf       */ }
//!
//! @warning "uart_init()" must be performed before
//!
//! @param argument list
//!
//!     The format string is interpreted like this:
//!        ,---------------,---------------------------------------------------,
//!        | Any character | Output as is                                      |
//!        |---------------+---------------------------------------------------|
//!        |     %c:       | interpret argument as character                   |
//!        |     %s:       | interpret argument as pointer to string           |
//!        |     %d:       | interpret argument as decimal (signed) S16        |
//!        |     %ld:      | interpret argument as decimal (signed) S32        |
//!        |     %u:       | interpret argument as decimal (unsigned) U16      |
//!        |     %lu:      | interpret argument as decimal (unsigned) U32      |
//!        |     %x:       | interpret argument as hex U16 (lower case chars)  |
//!        |     %lx:      | interpret argument as hex U32 (lower case chars)  |
//!        |     %X:       | interpret argument as hex U16 (upper case chars)  |
//!        |     %lX:      | interpret argument as hex U32 (upper case chars)  |
//!        |     %%:       | print a percent ('%') character                   |
//!        '---------------'---------------------------------------------------'
//!
//!     Field width (in decimal) always starts with "0" and its maximum is
//!     given by "DATA_BUF_LEN" defined in "uart_lib.h".
//!        ,----------------------,-----------,--------------,-----------------,
//!        |       Variable       | Writting  |  Printing    |    Comment      |
//!        |----------------------+-----------+--------------|-----------------|
//!        |                      |   %x      | aa           |        -        |
//!        |  u8_xx = 0xAA        |   %04d    | 0170         |        -        |
//!        |                      |   %012X   | 0000000000AA |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        | u16_xx = -5678       |   %010d   | -0000005678  |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        | u32_xx = -4100000000 |   %011lu  | 00194967296  |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        |          -           |   %8x     | 8x           | Writting error! |
//!        |----------------------+-----------+--------------|-----------------|
//!        |          -           |   %0s     | 0s           | Writting error! |
//!        '----------------------'-----------'--------------'-----------------'
//!       
//! Return: 0 = O.K.
//!
U8 uart_mini_printf(char *format, ...);

//------------------------------------------------------------------------------
//  @fn uart_get_editedline
//!
//! This function is a simple line-editor that allows to delete and
//! re-edit the characters entered, until either (CR) or (NL) is entered.
//! Printable characters entered will be echoed using uart_putchar().
//! 
//! Editing characters:
//! - \b (BS) or \x7F (DEL) delete the previous character
//! - \t will be replaced by a single space
//! - ^z (or ^Z) kills the entire input buffer
//! 
//! All other control characters will be ignored. (CR) or (NL) used to stop
//! the line acquisition is not recorded.
//! 
//! The internal line buffer is [size] characters long, which includes
//! the terminating \0, the 'END OF STRING' flag.  If the buffer is full 
//! (i.e. at [size-1] characters in order to keep space for the
//! 'END OF STRING' flag), the function will be automatically finished.
//! 
//! The data-string can be re-sent up to null character ('END OF STRING' flag)
//! by uart_mini_printf() or uart_put_string () functions.
//!
//! @warning "uart_init()" must be performed before
//!
//! @param  Pointer on the line buffer
//!         Size of the buffer (0 not allowed)
//!
//! @return (none)
//!
void uart_get_editedline (U8 *data_string, U8 size);

//______________________________________________________________________________

#endif  // _UART_IT_LIB_H_
