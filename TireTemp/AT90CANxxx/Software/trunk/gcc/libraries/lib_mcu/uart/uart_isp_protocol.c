//******************************************************************************
//! @file $RCSfile: uart_isp_protocol.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the library of functions of:
//!             - UART ISP PROTOCOL
//!             - AT90CAN128/64/32
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "uart_isp_protocol.h"
#include "isp_lib.h"
#include "uart_lib.h"
#include "uart_drv.h"

//_____ D E F I N I T I O N S __________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________

U8           hex_record_length;
U16          hex_offset;
U8           hex_record_type;
U8           hex_computed_chk;

U8           uart_isp_rx_buffer[4];
U8           uart_isp_tx_buffer[19];

Bool         uart_communication_opened;

//_____ F U N C T I O N S ______________________________________________________

//------------------------------------------------------------------------------
//  @fn uart_isp_protocol_task
//!
//! This function initializes the UART ISP PROTOCOL.
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!
//------------------------------------------------------------------------------
void uart_isp_protocol_init(void)
{
    isp_select_memory   = MEM_DEFAULT;
    isp_select_page     = PAGE_DEFAULT;
    isp_start_address   = ADD_DEFAULT;
    isp_number_of_bytes = N_DEFAULT;
    isp_prog_on_going   = FALSE;

    //- Init Rx data
//    loc_buf_index = 0;
//    hex_computed_chk = 0;
}

//------------------------------------------------------------------------------
//  @fn uart_isp_protocol_task
//!
//! This function manages the UART ISP PROTOCOL.
//!
//! @warning none
//!
//! @param  none
//!
//! @return 0: Error (not used)
//!         1: Ok (for a new command)
//!
//------------------------------------------------------------------------------
Bool uart_isp_protocol_task(void)
{
U8      u8_temp;
U8      i, j;
U8      hex_switch_code;

    //- Wait for ':' character (HEX_RECORD_MARK)
    while (uart_getchar() != HEX_RECORD_MARK);    
    uart_putchar(HEX_RECORD_MARK);              // Echo
    
    //- Checksum initialization
    hex_computed_chk = 0;
    
    //- Get record lengh
    hex_record_length = uart_isp_getchar();     // Echo performed

    //- Get offset
    hex_offset = (U16)(uart_isp_getchar()) << 8;          // Echo performed
    hex_offset = hex_offset | (U16)(uart_isp_getchar());  // Echo performed
    
    //- Get record type
    hex_record_type = uart_isp_getchar();       // Echo performed

    //- Pre-construction of hex_switch_code
    if(hex_record_type == 0x00)
    {
        hex_switch_code = HEX_TYPE_PROG_DATA;
    }
    else
    {
        hex_switch_code = (U8)(hex_record_length + hex_record_type);
    }

//--- Switch following UART ISP resquest received
    switch (hex_switch_code)
    {

    //- DATA PROGRAMMING (Length = nn, Type = 0x00) ----------------------------
        case HEX_TYPE_PROG_DATA:
            //- Set pointer and size for block to program
            isp_start_address = hex_offset;
            isp_number_of_bytes = (U16)(hex_record_length);
    
            //- Save pointer and size for block programming using
            isp_start_address_copy = isp_start_address;
            isp_number_of_bytes_copy = isp_number_of_bytes;
                
            //- BLOCK PROGRAMMING ----------------------------------------------
                //- Init Rx data
                loc_buf_index = 0;

                //- Rx data
                for (i=0; i<hex_record_length; i++)
                {
                    u8_temp = uart_isp_getchar();       // Echo performed                   
                    local_buffer[loc_buf_index] = u8_temp;
                    loc_buf_index++;
                }

                //- Verify checksum
                u8_temp = uart_isp_getchar();       // Echo performed
                if (hex_computed_chk == 0)
                {
                    u8_temp = isp_prog_block(local_buffer, loc_buf_index);
                    // HEX prtocol defines max Length = 255.
                    // So no 0x00 response from "isp_prog_block()"
                    // only 0x01 (Ok) or 0xFF(error) as response.
                    if (u8_temp == 0xFF)
                    {
                        //- Acknowledge frame = programmation error (ex: SSB cause)
                        uart_isp_send_frame(UART_PROG_ERR);
                    }
                    else
                    {
                        //- Acknowledge frame = command done
                        uart_isp_send_frame(UART_CMD_DONE);                        
                    } 
                    //- Restore pointer and size
                    isp_start_address = isp_start_address_copy;
                    isp_number_of_bytes = isp_number_of_bytes_copy;
                    
                }
                else
                {
                    //- Acknowledge frame = wrong checksum
                    uart_isp_send_frame(UART_WRONG_CHKSUM);
                }

            break;


    //- START APPLICATION (Length = 0x00, Type = 0x01) -------------------------
        case HEX_TYPE_START_APPLI:
            //- Verify checksum
            uart_isp_getchar();       // Echo performed
            if (hex_computed_chk != 0)
            {
                //- Acknowledge frame = wrong ehecksum
                uart_isp_send_frame(UART_WRONG_CHKSUM);
            }
            else
            {
            //- JUMP or RESET --------------------------------------------------            
                //- Hard reset 
                if (hex_offset == 0xFFFF)
                {
                    //uart_isp_send_frame(UART_CMD_DONE);   // Optional                        
                    // Exit from Boot Loader and Hard Reset
                    Hard_reset();       // c.f. at90can_mcu.h included in config.h                
                }
                //- Jump to application @
                else
                {
                    //uart_isp_send_frame(UART_CMD_DONE);   // Optional                        
                    // Exit from Boot Loader and Jump
                    isp_jump_to(hex_offset);
                }
            }
            
            break;


    //- SELECT NEW PAGE (Length = 0x02, Type = 0x02) ---------------------------
        case HEX_TYPE_SEL_PAGE:
            // hex_offset unused !
            j = isp_select_page;    // Saving

            //- Get page
            u8_temp = uart_isp_getchar();       // Echo performed
            isp_select_page = u8_temp >> 4;

            //- PAGE SELECTION -------------------------------------------------            
                //- Get data[1]=0x00
                u8_temp = uart_isp_getchar();       // Echo performed
                if (u8_temp != UART_NEW_PAGE_1)
                {
                    //- Acknowledge frame = unknown command
                    uart_isp_send_frame(UART_UNKNOWN_CMD);
                    isp_select_page = j;    // Restoring
                }
                else
                {
                    //- Verify checksum
                    uart_isp_getchar();       // Echo performed
                    if (hex_computed_chk != 0)
                    {
                        //- Acknowledge frame = wrong checksum
                        uart_isp_send_frame(UART_WRONG_CHKSUM);
                        isp_select_page = j;    // Restoring
                    }
                    else
                    {
                        //- Acknowledge frame = command done
                        uart_isp_send_frame(UART_CMD_DONE);
                    }
                }
            
            break;


    //- SELECT MEMORY and PAGE (Length = 0x02, Type = 0x04) --------------------
        case HEX_TYPE_SEL_MEM_PAGE:
            // hex_offset test
            if (hex_offset != 0x0000)
            {
                //- Acknowledge frame = unknown command
                uart_isp_send_frame(UART_UNKNOWN_CMD);
                break;                  //- Exit from: case ...                    
            }

            i = isp_select_page;      // Saving
            j = isp_select_memory;    // Saving
                                
            //- MEMORY SELECTION -----------------------------------------------            
                //- Get data[0]
                u8_temp = uart_isp_getchar();       // Echo performed
                isp_select_memory = u8_temp;
                
            //- PAGE SELECTION -------------------------------------------------            
                //- Get data[1]
                u8_temp = uart_isp_getchar();       // Echo performed
                isp_select_page = u8_temp;
                
                //- Verify checksum
                uart_isp_getchar();       // Echo performed
                if (hex_computed_chk != 0)
                {
                    //- Acknowledge frame = wrong checksum
                    uart_isp_send_frame(UART_WRONG_CHKSUM);
                    isp_select_memory = i;    // Restoring
                    isp_select_page = j;      // Restoring
                }
                else
                {
                    //- Acknowledge frame = command done
                    uart_isp_send_frame(UART_CMD_DONE);
                }
            
            break;

            
    //- READ/BLANK-CHECK/ERASE MEMORY (Length = 0x05, Type = 0x04, Data[4] ?) --
        case HEX_TYPE_X_DATA:
            // hex_offset test
            if (hex_offset != UART_XD_RD_BC_FE_OFFSET )
            {
                //- Acknowledge frame = unknown command
                uart_isp_send_frame(UART_UNKNOWN_CMD);
                break;                  //- Exit from: case ...                    
            }
                                
            //- GET ALL INFORMATION ... ----------------------------------------            
                //- Rx data and saving in uart_isp_rx_buffer[]
                for (i=0; i<4; i++)
                {
                    u8_temp = uart_isp_getchar();       // Echo performed
                    uart_isp_rx_buffer[i] = u8_temp;
                }
                
                //- Get data[4]
                u8_temp = uart_isp_getchar();       // Echo performed
                if (u8_temp > UART_FULL_ERASE_4)
                {
                    //- Acknowledge frame = unknown command
                    uart_isp_send_frame(UART_UNKNOWN_CMD);
                    break;                  //- Exit from: case ...                    
                }
                hex_switch_code = hex_switch_code + u8_temp;

                 //- Verify checksum
                uart_isp_getchar();       // Echo performed
                if (hex_computed_chk != 0)
                {
                    //- Acknowledge frame = wrong checksum
                    uart_isp_send_frame(UART_WRONG_CHKSUM);
                    break;                  //- Exit from: case ...                    
                }
               
        //- SWITCH FOLLOWING DATA[4] -------------------------------------------
            switch (hex_switch_code)
            {

            //- READ DATA (Length=0x05, Type=0x04, Data[4]=0x00) ---------------
                case HEX_TYPE_READ_DATA:
                    //- Acknowledge command
                    uart_isp_send_frame(UART_CR_LF);
                                
                    //- Set pointer and size for block to program
                    isp_address_range(
                        ( (((U16)uart_isp_rx_buffer[0])<<8) | ((U16)uart_isp_rx_buffer[1]) )
                    ,   ( (((U16)uart_isp_rx_buffer[2])<<8) | ((U16)uart_isp_rx_buffer[3]) ) ) ;
                    
                    //- Save pointer and size
                    isp_start_address_copy = isp_start_address;
                    isp_number_of_bytes_copy = isp_number_of_bytes;

                    while (1)
                    {
                        //- Using "uart_isp_tx_buffer" to communicate with 'uart_isp_send_frame()'
                        uart_isp_tx_buffer[0] = 16;         // Data size
                        uart_isp_tx_buffer[1] = (U8)(isp_start_address >>8);    // msByte of address
                        uart_isp_tx_buffer[2] = (U8)(isp_start_address);        // lsByte of address
                        
                        //- Using "uart_isp_tx_buffer" to store the blocks
                        u8_temp = isp_read_block( &uart_isp_tx_buffer[3] , 16 );
                        
                        if (u8_temp == 0x00)            // Intermediate block
                        {
                            //- Acknowledge frame = read command done
                            uart_isp_send_frame(UART_READ_CMD_DONE);            
                        }
                        else if  (u8_temp == 0x01)      // Last block
                        {
                            uart_isp_tx_buffer[0] = isp_number_of_bytes;    // Data size of the last block
                            //- Acknowledge frame = read command done
                            uart_isp_send_frame(UART_READ_CMD_DONE);            
                            break;  //- Exit from: while(1...
                        }
                        else    //- u8_temp == 0xFF, error (ex: SSB cause)
                        {
                            //- Acknowledge frame = read error (ex: SSB cause)
                            uart_isp_send_frame(UART_READ_ERR);            
                            break;  //- Exit from: while(1...
                        }
                    }

                    //- Restore pointer and size
                    isp_start_address = isp_start_address_copy;
                    isp_number_of_bytes = isp_number_of_bytes_copy;
                    
                break;
                
            //- BLANK CHECK (Length=0x05, Type=0x04, Data[4]=0x01) -------------
                case HEX_TYPE_BLANK_CHECK:
                    //- Set pointer and size for block to program
                    isp_address_range   ( ( (((U16)uart_isp_rx_buffer[0])<<8) | ((U16)uart_isp_rx_buffer[1]) ) , \
                                          ( (((U16)uart_isp_rx_buffer[2])<<8) | ((U16)uart_isp_rx_buffer[3]) ) ) ;
                    
                    //- Save pointer and size
                    isp_start_address_copy = isp_start_address;
                    isp_number_of_bytes_copy = isp_number_of_bytes;

                    if ( isp_blank_check() == FALSE )
                    {
                        //- Using "uart_isp_tx_buffer" to communicate with 'uart_isp_send_frame()'
                        uart_isp_tx_buffer[0] = (U8)(isp_start_address >>8);    // 1st failed address msB
                        uart_isp_tx_buffer[1] = (U8)(isp_start_address);        // 1st failed address lsB
                        //- Acknowledge frame = blank check memory error
                        uart_isp_send_frame(UART_CHK_ERR);            
                    }
                    else
                    {
                        //- Acknowledge frame = command done
                        uart_isp_send_frame(UART_CMD_DONE);            
                    }
                    
                    //- Restore pointer and size
                    isp_start_address = isp_start_address_copy;
                    isp_number_of_bytes = isp_number_of_bytes_copy;

                break;
                
            //- FULL ERASE (Length=0x05, Type=0x04, Data[4]=0x02) --------------
                case HEX_TYPE_FULL_ERASE:
                    // Data[0..3] verification
                    if ( (uart_isp_rx_buffer[0] == UART_FULL_ERASE_0)
                    &&   (uart_isp_rx_buffer[1] == UART_FULL_ERASE_1)
                    &&   (uart_isp_rx_buffer[2] == UART_FULL_ERASE_2)
                    &&   (uart_isp_rx_buffer[3] == UART_FULL_ERASE_3) )
                    {                     
                        //- Erase memory
                        if (isp_memory_erase() == FALSE)
                        {
                            //- Acknowledge frame = erase error (ex: Read Only Memory cause)
                            uart_isp_send_frame(UART_PROG_ERR);
                        }
                        else
                        {
                            //- Acknowledge frame = command done
                            uart_isp_send_frame(UART_CMD_DONE);            
                        }
                    }
                    else
                    {
                        //- Acknowledge frame = unknown command
                        uart_isp_send_frame(UART_UNKNOWN_CMD);
                    }

                break;
                
            //- DEFAULT --------------------------------------------------------
                default:
                    //- Acknowledge frame = unknown command
                    uart_isp_send_frame(UART_UNKNOWN_CMD);
                break;
                                
            }   // 2nd switch (hex_switch_code...
            
        break; // case HEX_TYPE_X_DATA
        
    //- DEFAULT ----------------------------------------------------------------
        default:
            //- Acknowledge frame = unknown command
            uart_isp_send_frame(UART_UNKNOWN_CMD);
        break;

    }   // 1st switch (hex_switch_code...

    return(1);
    
}   // uart_isp_protocol_task...
        
//_____ L O W    L E V E L    F U N C T I O N S ________________________________

//------------------------------------------------------------------------------
//  @fn uart_isp_getchar
//!
//! This function gets from UART 2 hexa ASCII characters and return 1 hexa byte.
//!
//! @warning This function uses 'uart_getchar()'.
//!          This function also uses 'uart_putchar()' for echoing.
//!
//! @param  none 
//!
//! @return  1 hexa character
//!
//------------------------------------------------------------------------------
U8 uart_isp_getchar (void)
{
U8  u8_temp_h, u8_temp_l;
U8  return_byte;

    u8_temp_h = uart_getchar();
    uart_putchar(u8_temp_h);                    // Echo 1st ASCII char
    u8_temp_l = uart_getchar();
    uart_putchar(u8_temp_l);                    // Echo 2nd ASCII char
    
    if (u8_temp_h > '9') u8_temp_h = (u8_temp_h & 0x4F) - 0x07 ;
    return_byte = (u8_temp_h & 0x0F) << 4;
    
    if (u8_temp_l > '9') u8_temp_l = (u8_temp_l & 0x4F) - 0x07 ;
    return_byte = return_byte | (u8_temp_l & 0x0F);

    //- Compute checksum
    hex_computed_chk = (U8)(hex_computed_chk + return_byte);
    
    return(return_byte);
}

//------------------------------------------------------------------------------
//  @fn uart_isp_putchar
//!
//! This function puts on UART 2 hexa ASCII characters from 1 hexa byte.
//!
//! @warning This function uses 'uart_putchar()'.
//!
//! @param  1 hexa character 
//!
//! @return  none
//!
//------------------------------------------------------------------------------
void uart_isp_putchar (U8 simple_byte)
{
    U8  u8_temp;
    
    u8_temp = simple_byte >> 4;
    if (u8_temp > 9) u8_temp = u8_temp + 0x07 ;
    uart_putchar(u8_temp + 0x30);
    
    u8_temp = simple_byte & 0x0F;
    if (u8_temp > 9) u8_temp = u8_temp + 0x07 ;
    uart_putchar(u8_temp + 0x30);
}

//------------------------------------------------------------------------------
//  @fn uart_isp_send_frame
//!
//! This function send frames in 'uart_isp_protocol_task()' function.
//! - UART_CMD_DONE         0x00    // '.\r\n'
//! - UART_UNKNOWN_CMD      0x00    // 'K\r\n'
//! - UART_READ_CMD_DONE    0x01    // 'address=data(16-byte)\r\n'
//! - UART_WRONG_CHKSUM     0x02    // 'X\r\n'
//! - UART_CHK_ERR          0x03    // '1st_failling_add\r\n'
//! - UART_READ_ERR         0x04    // 'L\r\n'
//! - UART_PROG_ERR         0x05    // 'P\r\n'
//! - UART_CR_LF            0xFF    // '\r\n'
//!
//! @warning none
//!
//! @param From 0x00 to 0x05 & 0xFF (see above or in 'uart_isp_protocol_task.h')
//!
//! @return  none
//!
//------------------------------------------------------------------------------
void uart_isp_send_frame(U8 frame_type)
{
U8  i;

    switch (frame_type)
    {
    
    //- ACKNOWLEDGE FRAME = COMMAND DONE ---------------------------------------
        case UART_CMD_DONE:
            uart_putchar('.'); uart_isp_send_frame(UART_CR_LF);    
        break;

    //- ACKNOWLEDGE FRAME = UNKNOWN COMMAND ------------------------------------
        case UART_UNKNOWN_CMD:
            uart_putchar('K'); uart_isp_send_frame(UART_CR_LF);    
        break;

    //- ACKNOWLEDGE FRAME = READ COMMAND DONE ----------------------------------
        case UART_READ_CMD_DONE:
            uart_isp_putchar(uart_isp_tx_buffer[1]);    // msByte of address    
            uart_isp_putchar(uart_isp_tx_buffer[2]);    // lsByte of address    
      
            uart_putchar('=');
        
            for (i=0; i< uart_isp_tx_buffer[0]; i++)
            {
                uart_isp_putchar(uart_isp_tx_buffer[i+3]);
            }        
            uart_isp_send_frame(UART_CR_LF);    
        break;
        
    //- ACKNOWLEDGE FRAME = WRONG CHECKSUM -------------------------------------
        case UART_WRONG_CHKSUM:
            uart_putchar('X'); uart_isp_send_frame(UART_CR_LF);    
        break;

    //- ACKNOWLEDGE FRAME = BLANK CHECK MEMORY ERROR ---------------------------
        case UART_CHK_ERR:
            uart_isp_putchar(uart_isp_tx_buffer[0]);    // msByte of address    
            uart_isp_putchar(uart_isp_tx_buffer[1]);    // lsByte of address    
      
            uart_isp_send_frame(UART_CR_LF);    
        break;
        
    //- ACKNOWLEDGE FRAME = READ MEMORY ERROR (EX: SSB CAUSE) ------------------
        case UART_READ_ERR:
            uart_putchar('L'); uart_isp_send_frame(UART_CR_LF);    
        break;

    //- ACKNOWLEDGE FRAME = PROGRAM MEMORY ERROR (EX: SSB CAUSE) ---------------
        case UART_PROG_ERR:
            uart_putchar('P'); uart_isp_send_frame(UART_CR_LF);    
        break;
        
    //- PUTCHAR CR and LF ------------------------------------------------------
        case UART_CR_LF:
            uart_putchar('\r'); uart_putchar('\n');    
        break;

    //- DEFAULT ----------------------------------------------------------------
        default:
        break;
        
    } // switch (frame_type...
    
}

