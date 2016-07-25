//
//***** config.h ***************************************************************

#include <stdio.h>
#include <avr/io.h>
#include "compiler.h"

// MCU Configuration
#define FOSC           8000        // 8.000 MHz external crystal
#define F_CPU          (FOSC*1000) // Need for AVR GCC

// UART Configuration
#define USE_TIMER16    TIMER16_1
#define USE_UART       UART_0
#define UART_BAUDRATE  38400       // in bauds



