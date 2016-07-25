//******************************************************************************
//! @file $RCSfile: can_it_lib.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the library of functions of:
//!             - CAN (Controller Array Network) interrupt handling
//!             - AT90CAN128/64/32
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070525.
//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "can_it_lib.h"
#include "can_it_drv.h"
#include <avr/interrupt.h>

//_____ D E F I N I T I O N S __________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________

st_cmd_t  *can_descript_list[14];  // CAN message descriptor list (vs Mailbox)
U16  can_bus_off_count = 0;        // Bus-Off Counter          
U16  can_general_error_count = 0;  // CAN general error counter
U16  can_manag_error_count = 0;    // Error counter on descriptor/MOb management 

//_____ F U N C T I O N S ______________________________________________________

//------------------------------------------------------------------------------
//  @fn can_init
//!
//! CAN macro initialization. Reset the CAN peripheral, initialize the bit
//! timing, initialize all the registers mapped in SRAM to put MObs in
//! inactive state and enable the CAN macro.
//!
//! @warning The CAN macro will be enable after seen on CAN bus a receceive
//!          level as long as of an inter frame (hardware feature).
//!
//! @param  Mode (for "can_fixed_baudrate" param not used)
//!         ==0: start CAN bit timing evaluation from faster baudrate
//!         ==1: start CAN bit timing evaluation with CANBTx registers
//!              contents
//!
//! @return Baudrate Status
//!         ==0: research of bit timing configuration failed
//!         ==1: baudrate performed 
//!
//------------------------------------------------------------------------------
U8 can_init(U8 mode)
{
    if ((Can_bit_timing(mode))==0) return (0);  // c.f. macro in "can_drv.h"
    can_clear_all_mob();                        // c.f. function in "can_drv.c"
	Can_enable_mob_int();                       // c.f. macro in "can_drv.h"
	Can_enable_global_int();                    // c.f. macro in "can_drv.h"
    Can_enable();                               // c.f. macro in "can_drv.h" 
	Enable_interrupt();							// c.f. macro in "at90can_drv.h"
	return (1);
}

//------------------------------------------------------------------------------
//  @fn can_cmd
//!
//! This function takes a CAN descriptor, analyses the action to do:
//! transmit, receive or abort.
//! This function returns a status (CAN_CMD_ACCEPTED or CAN_CMD_REFUSED) if
//! a MOb for Rx or Tx has been found. If no MOB has been found, the
//! application must be retry at a later date.
//! This function also updates the CAN descriptor status (MOB_PENDING or
//! MOB_NOT_REACHED) and the CAN descriptor list if a MOb for Rx or Tx has
//! been found. If aborting is performed, the CAN descriptor status will be
//! set to STATUS_CLEARED.
//!
//! @param  st_cmd_t* - Can_descriptor pointer on CAN descriptor structure
//!         to select the action to do.
//!
//! @return CAN_CMD_ACCEPTED - command is accepted
//!         CAN_CMD_REFUSED  - command is refused
//!
//------------------------------------------------------------------------------
U8 can_cmd(st_cmd_t* cmd)
{
  U8 mob_handle, cpt;
  U32 u32_temp;
  
  if (cmd->cmd == CMD_ABORT)
  {
    if (cmd->status == MOB_PENDING)
    {
      // Rx or Tx not yet performed
      Can_set_mob(cmd->handle);
      Can_mob_abort();
      Can_clear_status_mob();       // To be sure !
      cmd->handle = 0;
    }
    cmd->status = STATUS_CLEARED; 
  }
  else
  {
    mob_handle = can_get_mob_free();
    if (mob_handle!= NO_MOB)
    {
      // Update CAN descriptor list
	  can_descript_list[mob_handle] = cmd;
      // Update CAN descriptor structure
	  cmd->status = MOB_PENDING; 
      cmd->handle = mob_handle;
      // Clean MOb used
      Can_set_mob(mob_handle);
      Can_clear_mob();
          
      switch (cmd->cmd)
      {
        //------------      
        case CMD_TX:    
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          for (cpt=0;cpt<cmd->dlc;cpt++) CANMSG = *(cmd->pt_data + cpt);
          if (cmd->ctrl.rtr) Can_set_rtr(); 
            else Can_clear_rtr();    
          Can_set_dlc(cmd->dlc);
          Can_config_tx();
          break;
        //------------      
        case CMD_TX_DATA:    
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          for (cpt=0;cpt<cmd->dlc;cpt++) CANMSG = *(cmd->pt_data + cpt);
          cmd->ctrl.rtr=0; Can_clear_rtr();
          Can_set_dlc(cmd->dlc);
          Can_config_tx();
          break;
        //------------      
        case CMD_TX_REMOTE:       
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          cmd->ctrl.rtr=1; Can_set_rtr();
          Can_set_dlc(cmd->dlc);
          Can_config_tx();
          break;
        //------------      
        case CMD_RX:
          u32_temp=0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          Can_clear_rtrmsk();
          Can_clear_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_DATA:
          u32_temp=0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=0; Can_set_rtrmsk(); Can_clear_rtr();
          Can_clear_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_REMOTE:
          u32_temp=0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=1; Can_set_rtrmsk(); Can_set_rtr();
          Can_clear_rplv();
          Can_clear_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_MASKED:
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          u32_temp=~0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          Can_clear_rtrmsk();
          Can_set_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_DATA_MASKED:
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          u32_temp=~0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=0; Can_set_rtrmsk(); Can_clear_rtr();
          Can_set_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_REMOTE_MASKED:
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          u32_temp=~0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=1; Can_set_rtrmsk(); Can_set_rtr();
          Can_clear_rplv();
          Can_set_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_REPLY:
          for (cpt=0;cpt<cmd->dlc;cpt++) CANMSG = *(cmd->pt_data + cpt);
          u32_temp=0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=1; Can_set_rtrmsk(); Can_set_rtr();
          Can_set_rplv();
          Can_clear_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_REPLY_MASKED:
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          for (cpt=0;cpt<cmd->dlc;cpt++) CANMSG = *(cmd->pt_data + cpt);
          u32_temp=~0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=1; Can_set_rtrmsk(); Can_set_rtr();
          Can_set_rplv();
          Can_set_idemsk();
          Can_config_rx();       
          break;
        //------------      
        default:
          // case CMD_NONE or not implemented command
          cmd->status = STATUS_CLEARED; 
          break;
        //------------      
      } // switch (cmd ...
    } // if (mob_handle ...
    else
    {
      cmd->status = MOB_NOT_REACHED;
      return CAN_CMD_REFUSED;
    }
  } // else of no CMD_ABORT
  return CAN_CMD_ACCEPTED;
}

//------------------------------------------------------------------------------
//  @fn CAN interrupt service routine
//!
//! This function updates the entire CAN descriptor corresponding to the MOB
//! interrupt. If error management exists between CAN descriptor and MOb, the
//! global variable - "can_manag_error_count" - is incremented.
//!
//! Else if an interrupt come from general CAN error, the global variable
//! - "can_general_error_count" - is incremented.
//!
//! Finally, if an interrupt come from BUS-OFF state reaching, the global 
//! variable - "can_general_error_count" - is incremented.
//!
//------------------------------------------------------------------------------
ISR(CANIT_vect)
{
    U8 a_status;
	st_cmd_t* cmd;

	// Scan general interrupts
	a_status = can_get_general_interrupt();
	if (a_status & BUS_OFF_MSK) can_bus_off_count++;
	if (a_status & ERR_GEN_MSK) can_general_error_count++;
	 
	// Is there always interrupts pending (i.e. some MOB interrupts are set ) ?
	if (can_get_general_interrupt() == CAN_INT_MSK)
	{	
	    // Is the right handling ?
	    cmd = can_descript_list[MOB_INT_NUM];
        if ((cmd->handle) != MOB_INT_NUM)
        {
            can_manag_error_count++; return;
        }
	    // Is the descriptor status OK ?
	    a_status = cmd->status;
        if ((a_status==STATUS_CLEARED)||(a_status==MOB_NOT_REACHED)||(a_status==MOB_DISABLE))
        {
            can_manag_error_count++; return;
        }
        
        // All is right now! Looking at MOb's
        Can_set_mob(cmd->handle);
        a_status = can_get_mob_status();
        
        // Message management
        switch (a_status)
        {
            case MOB_NOT_COMPLETED:
                // May be !
                can_manag_error_count++;
                break;
            //---------------      
            case MOB_RX_COMPLETED:     
            case MOB_RX_COMPLETED_DLCW:
                cmd->dlc = Can_get_dlc();
                can_get_data(cmd->pt_data);
                cmd->ctrl.rtr = Can_get_rtr();
                if (Can_get_ide()) // if extended frame
                {
                    cmd->ctrl.ide = 1; // extended frame
                    Can_get_ext_id(cmd->id.ext);
                }
                else // else standard frame
                    {
                        cmd->ctrl.ide = 0;
                        Can_get_std_id(cmd->id.std);
                    }
                // Status field of descriptor: 0x20 if Rx completed
                // Status field of descriptor: 0xA0 if Rx completed with DLCWarning    
                cmd->status = a_status;
                Can_mob_abort();        // Freed the MOB
                Can_clear_status_mob(); //   and reset MOb status
                break;
            //---------------      
            case MOB_TX_COMPLETED:     
                // Status field of descriptor: 0x40 if Tx completed
                cmd->status = a_status;
                Can_mob_abort();        // Freed the MOB
                Can_clear_status_mob(); //   and reset MOb status
                break;
            //---------------      
            default:
                // Status field of descriptor: (bin)000b.scfa if MOb error
                cmd->status = a_status;
                Can_mob_abort();        // Freed the MOB
                Can_clear_status_mob(); //   and reset MOb status
                break;
                 
        } // switch (a_status...
    }    
}
    
    
    
    
    
    
    
    
    
    
    
    
    

