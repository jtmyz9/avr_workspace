//
//***** config.h ***************************************************************

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "at90can_drv.h"
#include "compiler.h"

// MCU Configuration
#define FOSC           8000        // 8.000 MHz external crystal
#define F_CPU          (FOSC*1000) // Need for AVR GCC

// UART Configuration
#define USE_TIMER16    TIMER16_1
#define USE_UART       BOTH_UART
//#define UART_BAUDRATE  38400       // in bauds
#define UART_BAUDRATE  UART_AUTOBAUD


#define UART_TX_BUFFER_SIZE  32
#define UART_RX_BUFFER_SIZE  128



