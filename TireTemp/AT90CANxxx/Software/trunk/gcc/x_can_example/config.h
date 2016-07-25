//
//***** config.h ***************************************************************

#include <avr/io.h>
#include "compiler.h"

// MCU Configuration
#define FOSC           8000        		// 8.000 MHz external crystal
#ifndef F_CPU
	#define F_CPU          (FOSC*1000) 	// Need for AVR GCC
#endif  // F_CPU

// CAN // MCU Configuration
#define CAN_BAUDRATE   250         		// in kBit


