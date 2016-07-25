//******************************************************************************
//! @file $RCSfile: uart_it_drv.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level functions (drivers) of:
//!             - Both UARTs interrupt handling
//!             - for AT90CAN128/64/32
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
#include "uart_it_drv.h"
#include "timer16_drv.h"

//_____ D E F I N I T I O N S __________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________
#if (USE_UART == BOTH_UART)
    U8 uart_selected;
#endif

#if ((UART_BAUDRATE == VARIABLE_UART_BAUDRATE) || (UART_BAUDRATE == UART_AUTOBAUD))
    #ifndef USE_TIMER16
        #error You must define USE_TIMER16 to TIMER16_1 or TIMER16_3 or BOTH_TIMER16 in "config.h" file
    #elif (USE_TIMER16 == BOTH_TIMER16)
        U8 timer16_selected;
    #endif
#endif


    //----- Global variables
#if ((USE_UART == UART_0)||(USE_UART == BOTH_UART))
volatile U8 uart0_tx_buf[UART_TX_BUFFER_SIZE];
volatile U8 uart0_rx_buf[UART_RX_BUFFER_SIZE];
volatile U8 uart0_tx_head;
volatile U8 uart0_tx_tail;
volatile U8 uart0_rx_head;
volatile U8 uart0_rx_tail;
volatile U8 uart0_tx_buff_full;
volatile U8 uart0_rx_last_error;
#endif

#if ((USE_UART == UART_1)||(USE_UART == BOTH_UART))
volatile U8 uart1_tx_buf[UART_TX_BUFFER_SIZE];
volatile U8 uart1_rx_buf[UART_RX_BUFFER_SIZE];
volatile U8 uart1_tx_head;
volatile U8 uart1_tx_tail;
volatile U8 uart1_rx_head;
volatile U8 uart1_rx_tail;
volatile U8 uart1_tx_buff_full;
volatile U8 uart1_rx_last_error;
#endif

//_____ F U N C T I O N S ______________________________________________________

#if ((USE_UART == UART_0)||(USE_UART == BOTH_UART))
//------------------------------------------------------------------------------
//  @fn UART_0 TX data register empty interrupt service routine
//!
//! This function is called when the UART_0 is ready to transmit
//! the next byte.
//!
//------------------------------------------------------------------------------
ISR(USART0_UDRE_vect)
{
    
   if ( (uart0_tx_head != uart0_tx_tail) || (uart0_tx_buff_full == 1) )
   {
        // Get one byte from buffer and write it to UART, start the transmission
        Uart0_send_byte(uart0_tx_buf[uart0_tx_tail]);      
        // Calculate and store new buffer index
        uart0_tx_tail = (uart0_tx_tail + 1) & UART_TX_BUFFER_MASK;
        // Anyway clear "uart0_tx_buff_full"
        uart0_tx_buff_full = 0;
    }
    else
    {
        // Tx buffer empty, disable UDRE interrupt
        Uart0_tx_int_disable();
    }
}

//------------------------------------------------------------------------------
//  @fn UART_0 receive complete interrupt service routine
//!
//! This function is called when the UART has received a character.
//!
//------------------------------------------------------------------------------
ISR(USART0_RX_vect)
{

    U8 tmp_data, tmp_status;
     
    // Note:
    //======
    // Framing Error by UART           #define UART_FRAME_ERROR       0x1000    
    // Overrun condition by UART       #define UART_OVERRUN_ERROR     0x0800    
    // Parity error by UART            #define UART_PARITY_ERROR      0x0400    
    // Full circular Rx buffer         #define UART_BUFFER_FULL       0x2000    
    // Overflow on circular Rx buffer  #define UART_BUFFER_OVERFLOW  (0x4000 & UART_BUFFER_FULL)   
    // No receive data available       #define UART_NO_DATA           0x8000    
    // Error linked to the read data   #define UART_LINKED_ERROR      0x0100

    // Read UART status and data register
    tmp_status = Uart0_get_status();   // DO NOT CHANGE the order of these 2 lines !
    tmp_data   = Uart0_get_byte()  ;   // DO NOT CHANGE the order of these 2 lines !
    
    // ERROR: Receive buffer already FULL (then the new status will be OVERFLOW)
    //        The new incoming UART byte will be lost ! 
    if (     (uart0_rx_head == uart0_rx_tail) \
         && ((uart0_rx_last_error & ((U8)(UART_BUFFER_FULL >> 8))) == ((U8)(UART_BUFFER_FULL >> 8))) )
    {
        uart0_rx_last_error = (U8)(UART_BUFFER_OVERFLOW >> 8);
        return ;
    }
    // ... Standard progression ...
    else
    {
        // Store received data in buffer
        uart0_rx_buf[uart0_rx_head] = tmp_data;
        // Calculate & store the next receive buffer index 
        uart0_rx_head = ( uart0_rx_head + 1) & UART_RX_BUFFER_MASK;
        // Receive buffer FULL detection
        if ( uart0_rx_head == uart0_rx_tail )
        {
            tmp_status |= (U8)(UART_BUFFER_FULL >> 8);
        }
        // Store last received error
        uart0_rx_last_error = tmp_status;
    }
}

//------------------------------------------------------------------------------
//  @fn uart0_getchar
//!
//! Get a character from the circular buffer associated to UART_0.
//!
//! @warning none
//!
//! @param  none
//!
//! @return  lower byte:  received byte from circular buffer
//!          higher byte: last receive error
//!
//------------------------------------------------------------------------------
U16 uart0_getchar(void)
{    
    U16 tmp_data;

    // "CLI" to prevent problems due to sharing variables with ISR
    Disable_interrupt();
    
    if (     (uart0_rx_head == uart0_rx_tail) \
         && ((uart0_rx_last_error & ((U8)(UART_BUFFER_FULL >> 8))) != ((U8)(UART_BUFFER_FULL >> 8))) )
    {
        Enable_interrupt();
        return (UART_NO_DATA);   // No data available
    }
    else
    {
        // Get data from receive buffer
        tmp_data = (U16) (uart0_rx_buf[uart0_rx_tail]);
        // Calculate/store buffer index
        uart0_rx_tail = (uart0_rx_tail + 1) & UART_RX_BUFFER_MASK;
        // Test if the last received error points at data that will be returned
        if (uart0_rx_head == uart0_rx_tail)
        {
            uart0_rx_last_error |= (U8)(UART_LINKED_ERROR >> 8);
        }
        // Prepare return data from receive buffer
        tmp_data = (((U16)(uart0_rx_last_error)) << 8) + tmp_data;
        // Anyway, , clean "uart0_rx_last_error"
        uart0_rx_last_error &= ~((U8)(UART_BUFFER_OVERFLOW >> 8));

        // "SEI", return to standard operation
        Enable_interrupt();

        // Return data
        return (tmp_data);
    }
}

//------------------------------------------------------------------------------
//  @fn uart0_putchar
//!
//! Write a character in the circular buffer associated to UART_0.
//!
//! @warning Possible waiting loop: "while (uart0_tx_buff_full != 0 );"
//!
//! @param  byte to be transmitted
//!
//! @return  none
//!
//------------------------------------------------------------------------------
void uart0_putchar(U8 tmp_data)
{    
    // Wait for free space in buffer
	// Note: Place this code line always before "Disable_interrupt()"
    while (uart0_tx_buff_full != 0 );

    // "CLI" to prevent problems due to sharing variables with ISR
	// Note: Place this code line always after "while (uart0_tx_buff_full != 0 )"
    Disable_interrupt();
	
	// Store the character in buffer
    uart0_tx_buf[uart0_tx_head] = tmp_data;
    // Calculate/store buffer index
    uart0_tx_head = (uart0_tx_head + 1) & UART_TX_BUFFER_MASK;
    // Test if the transmit buffer is full
    if (uart0_tx_head == uart0_tx_tail)
    {
        uart0_tx_buff_full = 1;
    }
    // Anyway, enable UDRE interrupt
    Uart0_tx_int_enable();

    // "SEI", return to standard operation
    Enable_interrupt();
}    
#endif

#if ((USE_UART == UART_1)||(USE_UART == BOTH_UART))
//------------------------------------------------------------------------------
//  @fn UART_1 TX data register empty interrupt service routine
//!
//! This function is called when the UART_1 is ready to transmit
//! the next byte.
//!
//------------------------------------------------------------------------------
ISR(USART1_UDRE_vect)
{
   if ( (uart1_tx_head != uart1_tx_tail) || (uart1_tx_buff_full == 1) )
   {
        // Get one byte from buffer and write it to UART, start the transmission
        Uart1_send_byte(uart1_tx_buf[uart1_tx_tail]);      
        // Calculate and store new buffer index
        uart1_tx_tail = (uart1_tx_tail + 1) & UART_TX_BUFFER_MASK;
        // Anyway clear "uart1_tx_buff_full"
        uart1_tx_buff_full = 0;
    }
    else
    {
        // Tx buffer empty, disable UDRE interrupt
        Uart1_tx_int_disable();
    }
}

//------------------------------------------------------------------------------
//  @fn UART_1 receive complete interrupt service routine
//!
//! This function is called when the UART has received a character.
//!
//------------------------------------------------------------------------------
ISR(USART1_RX_vect)
{
    U8 tmp_data, tmp_status;

    // Note:
    //======
    // Framing Error by UART           #define UART_FRAME_ERROR       0x1000    
    // Overrun condition by UART       #define UART_OVERRUN_ERROR     0x0800    
    // Parity error by UART            #define UART_PARITY_ERROR      0x0400    
    // Full circular Rx buffer         #define UART_BUFFER_FULL       0x2000    
    // Overflow on circular Rx buffer  #define UART_BUFFER_OVERFLOW  (0x4000 & UART_BUFFER_FULL)   
    // No receive data available       #define UART_NO_DATA           0x8000    
    // Error linked to the read data   #define UART_LINKED_ERROR      0x0100

    // Read UART status and data register
    tmp_status = Uart1_get_status();   // DO NOT CHANGE the order of these 2 lines !
    tmp_data   = Uart1_get_byte()  ;   // DO NOT CHANGE the order of these 2 lines !
    
    // ERROR: Receive buffer already FULL (then the new status will be OVERFLOW)
    //        The new incoming UART byte will be lost ! 
    if (     (uart1_rx_head == uart1_rx_tail) \
         && ((uart1_rx_last_error & ((U8)(UART_BUFFER_FULL >> 8))) == ((U8)(UART_BUFFER_FULL >> 8))) )
    {
        uart1_rx_last_error = (U8)(UART_BUFFER_OVERFLOW >> 8);
        return ;
    }
    // ... Standard progression ...
    else
    {
        // Store received data in buffer
        uart1_rx_buf[uart1_rx_head] = tmp_data;
        // Calculate & store the next receive buffer index 
        uart1_rx_head = ( uart1_rx_head + 1) & UART_RX_BUFFER_MASK;
        // Receive buffer FULL detection
        if ( uart1_rx_head == uart1_rx_tail )
        {
            tmp_status |= (U8)(UART_BUFFER_FULL >> 8);
        }
        // Store last received error
        uart1_rx_last_error = tmp_status;
    }
}

//------------------------------------------------------------------------------
//  @fn uart1_getchar
//!
//! Get a character from the circular buffer associated to UART_1.
//!
//! @warning none
//!
//! @param  none
//!
//! @return  lower byte:  received byte from circular buffer
//!          higher byte: last receive error
//!
//------------------------------------------------------------------------------
U16 uart1_getchar(void)
{    
    U16 tmp_data;

    // "CLI" to prevent problems due to sharing variables with ISR
    Disable_interrupt();
    
    if (     (uart1_rx_head == uart1_rx_tail) \
         && ((uart1_rx_last_error & ((U8)(UART_BUFFER_FULL >> 8))) != ((U8)(UART_BUFFER_FULL >> 8))) )
    {
        Enable_interrupt();
        return (UART_NO_DATA);   // No data available
    }
    else
    {
        // Get data from receive buffer
        tmp_data = (U16) (uart1_rx_buf[uart1_rx_tail]);
        // Calculate/store buffer index
        uart1_rx_tail = (uart1_rx_tail + 1) & UART_RX_BUFFER_MASK;
        // Test if the last received error points at data that will be returned
        if (uart1_rx_head == uart1_rx_tail)
        {
            uart1_rx_last_error |= (U8)(UART_LINKED_ERROR >> 8);
        }
        // Prepare return data from receive buffer
        tmp_data = (((U16)(uart1_rx_last_error)) << 8) + tmp_data;
        // Anyway, , clean "uart1_rx_last_error"
        uart1_rx_last_error &= ~((U8)(UART_BUFFER_OVERFLOW >> 8));

        // "SEI", return to standard operation
        Enable_interrupt();

        // Return data
        return (tmp_data);
    }
}

//------------------------------------------------------------------------------
//  @fn uart1_putchar
//!
//! Write a character in the circular buffer associated to UART_1.
//!
//! @warning Possible waiting loop: "while (uart1_tx_buff_full != 0 );"
//!
//! @param  byte to be transmitted
//!
//! @return  none
//!
//------------------------------------------------------------------------------
void uart1_putchar(U8 tmp_data)
{    
    // Wait for free space in buffer
	// Note: Place this code line always before "Disable_interrupt()"
    while (uart1_tx_buff_full != 0 );

    // "CLI" to prevent problems due to sharing variables with ISR
	// Note: Place this code line always after "while (uart1_tx_buff_full != 0 )"
    Disable_interrupt();
	
    // Store the character in buffer
    uart1_tx_buf[uart1_tx_head] = tmp_data;
    // Calculate/store buffer index
    uart1_tx_head = (uart1_tx_head + 1) & UART_TX_BUFFER_MASK;
    // Test if the transmit buffer is full
    if (uart1_tx_head == uart1_tx_tail)
    {
        uart1_tx_buff_full = 1;
    }
    // Anyway, enable UDRE interrupt
    Uart1_tx_int_enable();

    // "SEI", return to standard operation
    Enable_interrupt();
}    
#endif

#if ((UART_BAUDRATE == VARIABLE_UART_BAUDRATE) || (UART_BAUDRATE == UART_AUTOBAUD))
//------------------------------------------------------------------------------
//  @fn uart_set_baudrate
//!
//! This function programs the UBRRH register of both the UARTs from the
//! value passed as parameter or from an automatic timing detection if
//! there is an 'U' character received on the serial link. This function
//! is called by "Uart_set_baudrate(baudrate)" macro.
//! The function rounds to nearest the division result (i.e. FOSC/BAUDRATE).
//!
//! @warning Lower limit is 300 Bauds per MHz (ex: at 8MHz, lower limit
//!          is 2400 Bauds).
//!          Writting UBBR_L triggers an immediate update of the baud
//!          rate prescaler.
//!     Note the role of SOF_DETECTION !
//!
//! @param  !=0: baudrate value
//!         ==0: automatic baudrate detection
//!
//! @return Baudrate Status
//!         ==0: baudrate configuration failed
//!         ==1: baudrate performed
//------------------------------------------------------------------------------
U8 uart_set_baudrate (U32 baudrate)
{
    U16 t_meas_0, t_meas_1, t_meas_2, t_meas_3, t_meas_4;
    U16 t_min, t_max;

    if (baudrate != 0)
    {
        // UBRRN Value (rounded to nearest division) with U2Xn=1
        t_meas_4 = (U16)(( (((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1);
    }
    else    // Wait for only one "U" ASCII Char (0x55)
            // Autobaud using "USE_TIMER16" definition
    {
        // Autobaud set-up
        Uart_rx_in_pu_on();     // RxD in input + pull up
        Uart_tx_in_pu_on();     // TxD in input + pull up
        Timer16_clear();        // Flush, Disable and Reset TIMER16

//
// SOF_DETECTED means that the Start_Of_Frame bit has been already detected and
// it is impossible to measure this bit length. So, because all measurement are
// processed on all falling edges,  the start of the whole measurement will be
// done from the start of bit 1 (failling edge detection) of the 'U' character.
//
#ifndef SOF_DETECTED        
        // Wait for falling edge of SOF.
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0

        // SOF Detected => Start Timer
        Timer16_set_clock(TIMER16_CLKIO_BY_1);     

        // SOF + bit[0] Measurement. Read 3 times to remove overshoots.
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0
        //t_meas_0 = TCNT3;
        t_meas_0  =  Timer16_get_counter_low();
        t_meas_0 |= (Timer16_get_counter_high() << 8 );

        // SOF + bit[0:2] Measurement. Read 3 times to remove overshoots.
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0
        t_meas_1  =  Timer16_get_counter_low();
        t_meas_1 |= (Timer16_get_counter_high() << 8 );

        // SOF + bit[0:4] Measurement. Read 3 times to remove overshoots.
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0
        t_meas_2  =  Timer16_get_counter_low();
        t_meas_2 |= (Timer16_get_counter_high() << 8 );

        // SOF + bit[0:6] Measurement. Read 3 times to remove overshoots.
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0
        t_meas_3  =  Timer16_get_counter_low();
        t_meas_3 |= (Timer16_get_counter_high() << 8 );

        // SOF + bit[0:7] Measurement. Only 1 read because looking for _/ edge.
        while (uart_rx_get_level()   == 0);  // Wait for RxD=1
        t_meas_4  =  Timer16_get_counter_low();
        t_meas_4 |= (Timer16_get_counter_high() << 8 );

        // End of Frame
        Timer16_off();
        Uart_rx_in_pu_off();     // RxD in input no pull-up
        Uart_tx_in_pu_off();     // TxD in input no pull-up

        // 1-bit (last bit) measurement
        t_meas_4 = t_meas_4 - t_meas_3;       // Bit[7]   length
        // 2-bit measurements
        t_meas_3 = t_meas_3 - t_meas_2;       // Bit[5:6] length
        t_meas_2 = t_meas_2 - t_meas_1;       // Bit[3:4] length
        t_meas_1 = t_meas_1 - t_meas_0;       // Bit[1:2] length
        // t_meas_0 is already 2-bit length, SOF + bit[0] length

        // Verification if each couple of bits has an length error
        // less than 1/4 (2*1/8) of Bit[7] length
        t_min =  (2* t_meas_4) - (t_meas_4 >> 2);
        t_max =  (2* t_meas_4) + (t_meas_4 >> 2) + (t_meas_4 >> 3);
        if ((t_meas_0 > t_max) || (t_meas_0 < t_min)) return 0;
        if ((t_meas_1 > t_max) || (t_meas_1 < t_min)) return 0;
        if ((t_meas_2 > t_max) || (t_meas_2 < t_min)) return 0;
        if ((t_meas_3 > t_max) || (t_meas_3 < t_min)) return 0;

        // UBRRN Value (rounded to nearest division) with U2Xn=1
        t_meas_4  = (((((t_meas_0 + t_meas_1 + t_meas_2 + t_meas_3 ) << 1) >> 6) + 1) >> 1) - 1;
        
#else        
        // Wait for falling edge between bit[0] and bit[1].
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0

        // Edge detected => Start Timer
        Timer16_set_clock(TIMER16_CLKIO_BY_1);     

        // Bit[1..2] Measurement. Read 3 times to remove overshoots.
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0
        //t_meas_0 = TCNT3;
        t_meas_0  =  Timer16_get_counter_low();
        t_meas_0 |= (Timer16_get_counter_high() << 8 );

        // Bit[1..4] Measurement. Read 3 times to remove overshoots.
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0
        t_meas_1  =  Timer16_get_counter_low();
        t_meas_1 |= (Timer16_get_counter_high() << 8 );

        // Bit[1..6] Measurement. Read 3 times to remove overshoots.
        while (uart_rx_get_3level() == 0);  // Wait for RxD=1
        while (uart_rx_get_level()   != 0);  // Wait for RxD=0
        t_meas_2  =  Timer16_get_counter_low();
        t_meas_2 |= (Timer16_get_counter_high() << 8 );

        // Bit[1:7] Measurement. Only 1 read because looking for rising edge.
        while (uart_rx_get_level()   == 0);  // Wait for RxD=1
        t_meas_3  =  Timer16_get_counter_low();
        t_meas_3 |= (Timer16_get_counter_high() << 8 );

        // End of Frame
        Timer16_off();
        Uart_rx_in_pu_off();     // RxD in input no pull-up
        Uart_tx_in_pu_off();     // TxD in input no pull-up

        // 1-bit (last bit) measurement
        t_meas_3 = t_meas_3 - t_meas_2;     // Bit[7]   length
        // 2-bit measurements
        t_meas_2 = t_meas_2 - t_meas_1;     // Bit[5:6] length
        t_meas_1 = t_meas_1 - t_meas_0;     // Bit[3:4] length
        // t_meas_0 is already 2-bit length -  Bit[1:2] length

        // Verification if each couple of bits has an length error
        // less than 1/4 (2*1/8) of Bit[7] length
        t_min =  (2* t_meas_3) - (t_meas_3 >> 2);
        t_max =  (2* t_meas_3) + (t_meas_3 >> 2) + (t_meas_3 >> 3);
        if ((t_meas_0 > t_max) || (t_meas_0 < t_min)) return 0;
        if ((t_meas_1 > t_max) || (t_meas_1 < t_min)) return 0;
        if ((t_meas_2 > t_max) || (t_meas_2 < t_min)) return 0;

        // UBRRN Value (rounded to nearest division) with U2Xn=1
        t_meas_4  = (((((t_meas_0 + t_meas_1 + t_meas_1 + t_meas_2 ) << 1) >> 6) + 1) >> 1) - 1;

#endif  // #endif of "SOF_DETECTED"

    }
    #if (USE_UART ==  UART_0)
        UBRR0H  = (U8)((t_meas_4)>>8);
        UBRR0L  = (U8) (t_meas_4)    ;
        UCSR0A |=  (1<<U2X0);
    #elif (USE_UART ==  UART_1)
        UBRR1H  = (U8)((t_meas_4)>>8);
        UBRR1L  = (U8) (t_meas_4)    ;
        UCSR1A |=  (1<<U2X1);
    #elif (USE_UART ==  BOTH_UART)
        if (uart_selected == UART_0)
        {
            UBRR0H  = (U8)((t_meas_4)>>8);
            UBRR0L  = (U8) (t_meas_4)    ;
            UCSR0A |=  (1<<U2X0);
        }
        if (uart_selected == UART_1)
        {
            UBRR1H  = (U8)((t_meas_4)>>8);
            UBRR1L  = (U8) (t_meas_4)    ;
            UCSR1A |=  (1<<U2X1);
        }
    #   else
    #       error USE_UART definition is not referenced in "uart_it_drv.h" file
    #endif
    
    return 1;
}

#endif  // #endif of '((UART_BAUDRATE == ...'


//------------------------------------------------------------------------------
//  @fn uart_rx_get_3level
//!
//! This function returns the UART pin value sampled 3 times.
//!
//! @warning none
//!
//! @param  none
//!
//! @return UART pin value sampled 3 times
//------------------------------------------------------------------------------
Bool uart_rx_get_3level (void)
{
U8 u8_temp = 0;

#if ((USE_UART == UART_0) || (USE_UART == UART_1))
    u8_temp = Uart_rx_get_3level();
    
#elif (USE_UART ==  BOTH_UART)
    if (uart_selected == UART_0)
    {
        u8_temp = (UART_0_PORT_IN  & UART_0_PORT_IN  & UART_0_PORT_IN  & (1<<UART_0_INPUT_PIN)) >> UART_0_INPUT_PIN;
    }
    if (uart_selected == UART_1)
    {
        u8_temp = (UART_1_PORT_IN  & UART_1_PORT_IN  & UART_1_PORT_IN  & (1<<UART_1_INPUT_PIN)) >> UART_1_INPUT_PIN;
    }
#   else
#       error USE_UART definition is not referenced in "uart_drv.h" file
#endif

    return u8_temp;
}


//------------------------------------------------------------------------------
//  @fn uart_rx_get_level
//!
//! This function returns the UART pin value.
//!
//! @warning none
//!
//! @param  none
//!
//! @return UART pin value
//------------------------------------------------------------------------------
Bool uart_rx_get_level (void)
{
U8 u8_temp = 0;

#if ((USE_UART == UART_0) || (USE_UART == UART_1))
    u8_temp = Uart_rx_get_level();
    
#elif (USE_UART ==  BOTH_UART)
    if (uart_selected == UART_0)
    {
        u8_temp = (UART_0_PORT_IN  & (1<<UART_0_INPUT_PIN)) >> UART_0_INPUT_PIN;
    }
    if (uart_selected == UART_1)
    {
        u8_temp = (UART_1_PORT_IN  & (1<<UART_1_INPUT_PIN)) >> UART_1_INPUT_PIN;
    }
#   else
#       error USE_UART definition is not referenced in "uart_drv.h" file
#endif

    return u8_temp;
}














