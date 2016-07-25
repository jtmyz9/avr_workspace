//
//***** config.h ***************************************************************

#include <avr/io.h>
#include "compiler.h"

// MCU Configuration
#define FOSC           8000        // 8.000 MHz external crystal
#define F_CPU          (FOSC*1000) // Need for AVR GCC
// CAN // MCU Configuration
#define CAN_BAUDRATE   100         // in kBit


