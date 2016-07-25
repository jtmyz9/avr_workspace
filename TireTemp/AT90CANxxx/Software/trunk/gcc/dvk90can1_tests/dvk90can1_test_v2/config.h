//******************************************************************************
//! @file $RCSfile: config.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief Configuration file for the following project:
//!             - dvk90can1_test_v2_gcc
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 319852 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

#ifndef _CONFIG_H_
#define _CONFIG_H_

//_____ I N C L U D E S ________________________________________________________
#include "compiler.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "at90can_drv.h"
#include "dvk90can1_board.h"

//_____ M A C R O S ____________________________________________________________

//_____ D E F I N I T I O N S __________________________________________________

    // -------------- MCU LIB CONFIGURATION
#define FOSC           8000        // 8.000 MHz External cristal
#define F_CPU          (FOSC*1000) // Need for AVR GCC

    // -------------- UART LIB CONFIGURATION -0:
    // UART-0 only with fixed Baudrate all application long
#define USE_TIMER16         TIMER16_1
#define USE_UART            UART_0
#define UART_BAUDRATE_TEST  9600
#define UART_BAUDRATE_DEMO  38400
    // -------------- UART LIB CONFIGURATION -1:
    // UART-0 or 1 if Baudrate changes during application
//#define USE_TIMER16   BOTH_TIMER16
//#define USE_UART      BOTH_UART
//#define UART_BAUDRATE VARIABLE_UART_BAUDRATE
    // -------------- UART LIB CONFIGURATION -2:
    // UART-0 or 1 with fixed Baudrate all application long
//#define USE_TIMER16   TIMER16_1
//#define USE_UART      BOTH_UART
//#define UART_BAUDRATE 9600
    // -------------- UART LIB CONFIGURATION -3:
    // UART-0 or 1 with AutoBaudrate detection
//#define USE_TIMER16   BOTH_TIMER16
//#define USE_UART      BOTH_UART
//#define UART_BAUDRATE UART_AUTOBAUD

    // -------------- CAN LIB CONFIGURATION
//#define CAN_BAUDRATE   500        // in kBit
#define CAN_BAUDRATE   CAN_AUTOBAUD

    // -------------- MISCELLANEOUS
    // Using TIMER_2 as RTC
#define USE_TIMER8       TIMER8_2
//#define RTC_TIMER        2     // Already defined in "dvk90can1_board.h"
//#define RTC_CLOCK        32    // Already defined in "dvk90can1_board.h"

//_____ D E C L A R A T I O N S ________________________________________________

#define _LED_TEST_
#define _PORT_TEST_
#define _KEY_TEST_
#define _GET_TEMP_TEST_
#define _GET_LUM_TEST_
#define _GET_VIN_TEST_
#define _AUDIO_TEST_
#define _RTC_TEST_
#define _CAN_TEST_

#define MUTE_ON_AUDIO_OUT   ON       

//______________________________________________________________________________

#endif  // _CONFIG_H

