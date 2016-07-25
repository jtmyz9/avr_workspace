//******************************************************************************
//! @file $RCSfile: uart_isp_protocol.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the prototypes and the macros of the
//!        library of functions of:
//!             - UART ISP PROTOCOL
//!             - for AT90CAN128/64/32.
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

#ifndef _UART_ISP_PROTOCOL_H_
#define _UART_ISP_PROTOCOL_H_

//_____ I N C L U D E S ________________________________________________________

#include "uart_lib.h"

//_____ D E F I N I T I O N S __________________________________________________

//------ Protocol commands

// Intel Hex record mark
#define HEX_RECORD_MARK             ':'

// Codes for 'uart_isp_protocol_task()'
#define HEX_TYPE_PROG_DATA          0x00          // Type
#define HEX_TYPE_START_APPLI        0x01          // Length + Type
#define HEX_TYPE_SEL_PAGE           0x04          // Length + Type
#define HEX_TYPE_SEL_MEM_PAGE       0x06          // Length + Type
#define HEX_TYPE_X_DATA             0x09          // Length + Type
#define HEX_TYPE_READ_DATA          0x09          // Length + Type + Data[4] 
#define HEX_TYPE_BLANK_CHECK        0x0A          // Length + Type + Data[4]
#define HEX_TYPE_FULL_ERASE         0x0B          // Length + Type + Data[4]

// 'hex_offset' critical value
#define UART_XD_RD_BC_FE_OFFSET    ((U16)(0x0000))

// 'data[0..4]' critical value
#define UART_NEW_PAGE_1             0x00
#define UART_FULL_ERASE_0           0x00
#define UART_FULL_ERASE_1           0xFF
#define UART_FULL_ERASE_2           0x00
#define UART_FULL_ERASE_3           0x00
#define UART_FULL_ERASE_4           0x02

// Codes for 'uart_isp_send_frame()'
#define UART_CMD_DONE               0x00          // '.\r\n'
#define UART_UNKNOWN_CMD            0x01          // 'K\r\n'
#define UART_READ_CMD_DONE          0x02          // 'address=data(16-byte)\r\n'
#define UART_WRONG_CHKSUM           0x03          // 'X\r\n'
#define UART_CHK_ERR                0x04          // '1st_failling_add\r\n'
#define UART_READ_ERR               0x05          // 'L\r\n'
#define UART_PROG_ERR               0x06          // 'P\r\n'
#define UART_CR_LF                  0xFF          // '\r\n'

//_____ D E C L A R A T I O N __________________________________________________

extern void uart_isp_protocol_init(void);
extern Bool uart_isp_protocol_task(void);
extern void uart_isp_send_frame(U8 frame_type);
extern U8 uart_isp_getchar(void);
extern void uart_isp_putchar(U8 simple_byte);


extern U8           hex_record_length;
extern U16          hex_offset;
extern U8           hex_record_type;
extern U8           hex_computed_chk;

// TO REMOVE ?
// extern U8           uart_isp_tx_buffer[8];

extern U8           isp_select_memory;
extern U8           isp_select_page;
extern U16          isp_start_address;
extern U16          isp_number_of_bytes;
extern U16          isp_start_address_copy;
extern U16          isp_number_of_bytes_copy;

//______________________________________________________________________________

#endif  /* _UART_ISP_PROTOCOL_H_ */

