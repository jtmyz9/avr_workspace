//******************************************************************************
//! @file $RCSfile: main_dvk90can1_test_v2.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief Manufactory Test for DVK90CAN1 board & Demo prog.
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
#include "led_drv.h"
#include "key_drv.h"
#include "audio_drv.h"
#include "sensor_drv.h"
#include "adc_drv.h"
#include "uart_lib.h"
#include "rtc_drv.h"
#include "eeprom_lib.h"
#include "can_lib.h"

//_____ D E F I N I T I O N S __________________________________________________
#define RXCAN   'R'
#define TXCAN   'T'

//_____ D E C L A R A T I O N S ________________________________________________

U32     hex_to_deci_ascii(U16 hex_to_conv);
extern  U8 txt_small_logo_atmel[];

extern  U8 txt_logo_atmel[];
void    spy_printing(U8 rxtx, st_cmd_t* msg);
void    can_error_mode(void);

//------------------------------------------------------------------------------
//  @fn main
//!
//! Core of "main_dvk90can1_test_v2.c".
//! It is made of two parts, the DVK90CAN test and if the test passed, only
//!   the demo program (equ. can_spy_echo_example) is running.
//!
//! A- Test suite for DVK90CAN1 board:
//!    1 - Power-on Microcontroller
//!    2 - UART-1
//!    3 - LED's
//!    4 - PORT's
//!    5 - KEY's
//!    6 - Sensors
//!    7 - Audio
//!    8 - RTC
//!    9 - CAN
//!
//! @warning UART's at 9600 bauds
//!
//! B- Demo part:
//!    - CAN communications are listen, captured and a resume is sent on UART_0 .
//!    - An CAN Echo also is sent: ID(sent)=ID(received)+1
//!                             DLC = 8
//!                             RTR = 0
//!                             Data[6:7]=CANSTMH:CANSTML
//!    Note: If some key is pushed while RESET, only Spy mode is enabled.
//!    Note: Exit in UART ECHO MODE if CAN ID == 0 as CAN error simulation
//!
//! @warning Care on "#define"'s in "config.h"
//!          UART's at 38400 bauds
//!          CAN in auto-bitrate
//!
//! @param none.
//!
//! @return Integer 0
//!
//------------------------------------------------------------------------------
int main (void)
{
    //! Variables for test only
    //!-- 32-bit
    U32 temp, lum_value, vin_value;
    //!-- 16-bit
    S16 ii, temp_index;
    U16 temp_value, vin_index;
    //!-- 8-bit
    U8  key_index, lumi_index, temp1, temp2, ch_in, MOb, hyper_term;
    //!-- Boolean
    Bool  mute, run_demo;

    //! Variables for demo only
    //!-- 8-bit
    U8 mode;    // mode==0: spy+echo, mode!=0: only spy
    U8 txt_can_ana_1[]="======= UART-0 in CAN ANALYZER =======";
    U8 txt_can_ana_2[]="======= Echo + UART-0 in CAN ANALYZER =======";

    U8 i, u8_temp, led_out;
    U8 buffer[8];
    st_cmd_t message;


   
    //! Clock prescaler Reset
    CLKPR = 0x80;  CLKPR = 0x00;

    //! Test needed ?
    run_demo = ON;
    if (eeprom_rd_byte(6)  != '9') run_demo = OFF;
    if (eeprom_rd_byte(7)  != '0') run_demo = OFF;
    if (eeprom_rd_byte(30) != '3') run_demo = OFF;
    if (eeprom_rd_byte(32) != '2') run_demo = OFF;
    if (eeprom_rd_byte(33) != '0') run_demo = OFF;
    if (eeprom_rd_byte(40) !=  0 ) run_demo = OFF;
    
    if (run_demo == OFF)
    //! ........................................................................
    //! ....... Test part ......................................................
    //! ........................................................................
    {
        //! Init UART
        uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE_TEST);
    
        //! Init internal RTC as defined in "config.h" & "dvk90can1_board.h"
        rtc_int_init();
        
        //! Send TITLE
        uart_mini_printf ("\r\n\n§ .... DVK90CAN1 Board - Final Tests (v2).... §\r\n");
        uart_mini_printf       ("===============================================\r\n\n");
    
    //!*************************************************************************
    //! 1st TEST:
    //! ............ Power-on Microcontroller Test
    //! -
    //!*************************************************************************
        led_init();         // LED's = 0x00
        toggle_led();       // LED's = 0xFF    
        uart_mini_printf ("1st TEST - Power-on Test: LED's = 0xFF (Automatic output)\r\n");
        if (PINA != 0xFF) while(1);
        uart_mini_printf ("         - Test %cseems%c OK\r\n", '"', '"');
        
    
    //!*************************************************************************
    //! 2nd TEST:
    //! ............ UART-0 Test
    //! -
    //!*************************************************************************
        hyper_term = TRUE;
        //! Send test text
        uart_mini_printf ("2nd TEST - UART-0 Test: hit <space> PC-key as test\r\n");
            
        //! Wait for receive UART
        while ((temp1 = uart_test_hit ()) == 0);
        
        //! Check received character
        temp1 = uart_getchar();
        if (temp1 == ' ')
        {
                uart_mini_printf ("         - Test OK\r\n");
        }
        else hyper_term = FALSE;
        
        
    //!*************************************************************************
    //! 3rd TEST:
    //! ............ LED's Test
    //! -
    //!*************************************************************************
        uart_mini_printf ("3rd TEST - LED's Test: ");
    
    #ifdef _LED_TEST_
    
        uart_mini_printf ("LED's = 0x00, 0xFF, 0xAA & 0x55 (Automatic output)\r\n");
    
        //! Endless if false
        led_init();        wait_for(400); if (PINA != 0x00) while(1); // LEDs = 0x00
        toggle_led();      wait_for(400); if (PINA != 0xFF) while(1); // LEDs = 0xFF
        write_led(0xAA);   wait_for(400); if (PINA != 0xAA) while(1); // LEDs = 0xAA
        toggle_led();      wait_for(400); if (PINA != 0x55) while(1); // LEDs = 0x55
    
        uart_mini_printf ("         - Test OK\r\n");
    
    #else
        uart_mini_printf ("Skipped !!!\r\n");
    
    #endif  // _LED_TEST_ 
    
    //!*************************************************************************
    //! 4th TEST:
    //! ............ PORT's Test
    //! -
    //!*************************************************************************
    
        uart_mini_printf ("4th TEST - PORT's Self Test: ");
    
    #ifdef _PORT_TEST_
    
        uart_mini_printf ("PORT-B, ");
        DDRB = temp2 = 0x7F;
        // Endless if false
        PORTB=0x00; for(ii=0;ii<1000;ii++) {temp1=PINB & temp2; if(temp1==0x00) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-B = 0x00
        PORTB=0x7F; for(ii=0;ii<1000;ii++) {temp1=PINB & temp2; if(temp1==0x7F) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-B = 0x7F
        PORTB=0x0F; for(ii=0;ii<1000;ii++) {temp1=PINB & temp2; if(temp1==0x0F) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-B = 0x0F
        PORTB=0x70; for(ii=0;ii<1000;ii++) {temp1=PINB & temp2; if(temp1==0x70) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-B = 0x70
        PORTB=0x55; for(ii=0;ii<1000;ii++) {temp1=PINB & temp2; if(temp1==0x55) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-B = 0x55
        PORTB=0x2A; for(ii=0;ii<1000;ii++) {temp1=PINB & temp2; if(temp1==0x2A) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-B = 0x2A
        PORTB = DDRB = 0x00;
    
        uart_mini_printf ("C, ");
        DDRC = temp2 = 0xFF;
        // Endless if false
        PORTC=0x00; for(ii=0;ii<1000;ii++) {temp1=PINC & temp2; if(temp1==0x00) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-C = 0x00
        PORTC=0xFF; for(ii=0;ii<1000;ii++) {temp1=PINC & temp2; if(temp1==0xFF) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-C = 0xFF
        PORTC=0x0F; for(ii=0;ii<1000;ii++) {temp1=PINC & temp2; if(temp1==0x0F) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-C = 0x0F
        PORTC=0xF0; for(ii=0;ii<1000;ii++) {temp1=PINC & temp2; if(temp1==0xF0) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-C = 0xF0
        PORTC=0x55; for(ii=0;ii<1000;ii++) {temp1=PINC & temp2; if(temp1==0x55) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-C = 0x55
        PORTC=0xAA; for(ii=0;ii<1000;ii++) {temp1=PINC & temp2; if(temp1==0xAA) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-C = 0xAA
        PORTC = DDRC = 0x00;
    
        uart_mini_printf ("D, ");
        DDRD = temp2 = 0x9A;
        // Endless if false
        PORTD=0x00; for(ii=0;ii<1000;ii++) {temp1=PIND & temp2; if(temp1==0x00) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-D = 0x00
        PORTD=0x9A; for(ii=0;ii<1000;ii++) {temp1=PIND & temp2; if(temp1==0x9A) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-D = 0x9A
        PORTD=0x0A; for(ii=0;ii<1000;ii++) {temp1=PIND & temp2; if(temp1==0x0A) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-D = 0x0C
        PORTD=0x90; for(ii=0;ii<1000;ii++) {temp1=PIND & temp2; if(temp1==0x90) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-D = 0x90
        PORTD=0x18; for(ii=0;ii<1000;ii++) {temp1=PIND & temp2; if(temp1==0x18) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-D = 0x18
        PORTD=0x8A; for(ii=0;ii<1000;ii++) {temp1=PIND & temp2; if(temp1==0x8A) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-D = 0x8A
        PORTD = DDRD = 0x00;
    
        uart_mini_printf ("E, ");
        DDRE = temp2 = 0xFC;
        // Endless if false
        PORTE=0x00; for(ii=0;ii<1000;ii++) {temp1=PINE & temp2; if(temp1==0x00) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-E = 0x00
        PORTE=0xFC; for(ii=0;ii<1000;ii++) {temp1=PINE & temp2; if(temp1==0xFC) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-E = 0xFC
        PORTE=0x0C; for(ii=0;ii<1000;ii++) {temp1=PINE & temp2; if(temp1==0x0C) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-E = 0x0C
        PORTE=0xF0; for(ii=0;ii<1000;ii++) {temp1=PINE & temp2; if(temp1==0xF0) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-E = 0xF0
        PORTE=0x54; for(ii=0;ii<1000;ii++) {temp1=PINE & temp2; if(temp1==0x54) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-E = 0x54
        PORTE=0xA8; for(ii=0;ii<1000;ii++) {temp1=PINE & temp2; if(temp1==0xA8) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-E = 0xA8
        PORTE = DDRE = 0x00;
        
        uart_mini_printf ("F and ");
        DDRF = temp2 = 0x08;
        // Endless if false
        PORTF=0x00; for(ii=0;ii<1000;ii++) {temp1=PINF & temp2; if(temp1==0x00) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-F = 0x00
        PORTF=0x08; for(ii=0;ii<1000;ii++) {temp1=PINF & temp2; if(temp1==0x08) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-F = 0x08
        PORTF=DDRF = 0x00;
        
        uart_mini_printf ("G (Automatic output)\r\n");
        DDRG = temp2 = 0x07;
        // Endless if false
        PORTG=0x00; for(ii=0;ii<1000;ii++) {temp1=PING & temp2; if(temp1==0x00) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-G = 0x00
        PORTG=0x07; for(ii=0;ii<1000;ii++) {temp1=PING & temp2; if(temp1==0x07) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-G = 0x07
        PORTG=0x05; for(ii=0;ii<1000;ii++) {temp1=PING & temp2; if(temp1==0x05) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-G = 0x05
        PORTG=0x02; for(ii=0;ii<1000;ii++) {temp1=PING & temp2; if(temp1==0x02) break;} if(ii>=999){write_led(temp1); while(1);} // PORT-G = 0x02
        PORTG = DDRG = 0x00;
        
        uart_mini_printf ("         - Test OK\r\n");
        
    #else
        uart_mini_printf ("Skipped !!!\r\n");
    
    #endif  // _PORT_TEST_
    
    
    //!*************************************************************************
    //! 5th TEST:
    //! ............ KEY's Test
    //! -
    //!*************************************************************************
        uart_mini_printf ("5th TEST - KEY's Test: ");
    
    #ifdef _KEY_TEST_
    
        uart_mini_printf ("Push-on all keys of keyboard and INT key\r\n");
    
        led_init(); key_init();
        DDRD = 0; PORTD = 0x01;  // Pull-up on PORTD.0
        temp1 = NO_BUTTON;
        key_index = 0x00;
        
        uart_mini_printf ("           KEY's =");
        while (temp1!=(ALL_BUTTONS | (1<<PORTD0)))
        {
            temp2 = ((~PIND) & (1<<PORTD0)); temp1 |= temp2;
            if (temp2 == (1<<PORTD0))             // INT key
            {
                if ((key_index & 0x01) != 0x01)
                {
                    if (key_index != 0) uart_mini_printf (" ,");
                    uart_mini_printf (" INT.0");
                }
                key_index |= 0x01; Set_led(0);    // LED number 0 "ON"
            }
            temp2 = get_key(); temp1 |= temp2;
            if (temp2 == BUTTON_N)
            {
                if ((key_index & 0x10) != 0x10)
                {
                    if (key_index != 0) uart_mini_printf (" ,");
                    uart_mini_printf (" North");
                }
                key_index |= 0x10; Set_led(4);    // LED number 4 "ON"
            }
            if (temp2 == BUTTON_S)
            {
                if ((key_index & 0x80) != 0x80)
                {
                    if (key_index != 0) uart_mini_printf (" ,");
                    uart_mini_printf (" South");
                }
                key_index |= 0x80; Set_led(7);    // LED number 7 "ON"
            }
            if (temp2 == BUTTON_W)
            {
                if ((key_index & 0x40) != 0x40)
                {
                    if (key_index != 0) uart_mini_printf (" ,");
                    uart_mini_printf (" West");
                }
                key_index |= 0x40; Set_led(6);    // LED number 6 "ON"
            }
            if (temp2 == BUTTON_E)
            {
                if ((key_index & 0x20) != 0x20)
                {
                    if (key_index != 0) uart_mini_printf (" ,");
                    uart_mini_printf (" East");
                }
                key_index |= 0x20; Set_led(5);    // LED number 5 "ON"
            }
            if (temp2 == BUTTON_C)
            {
                if ((key_index & 0x04) != 0x04)
                {
                    if (key_index != 0) uart_mini_printf (" ,");
                    uart_mini_printf (" Center");
                }
                key_index |= 0x04; Set_led(2);    // LED number 2 "ON"
            }
        }
        uart_mini_printf ("\r\n");
        
        uart_mini_printf ("         - Test OK\r\n");
        wait_for(100); while (get_key() != 0); wait_for(100);
        
    #else
        uart_mini_printf ("Skipped !!!\r\n");
    
    #endif  // _KEY_TEST_
    
    
    //!*************************************************************************
    //! 6th TEST:
    //! ............ Sensor Tests
    //! -
    //!*************************************************************************
        uart_mini_printf ("6th TEST - Sensor Tests:\r\n");
    
    
    //!*************************************************************************
    //! ........................ Get Temperature Test
    //! -
    #ifdef _GET_TEMP_TEST_
    
        uart_mini_printf ("           . Get Temperature Test: (hit PC-key to output)\r\n");
    
        led_init(); key_init();
    
        while(1)
        {
            // Get temperature sensor
            temp_index = get_temperature ();
            uart_mini_printf ("             = ");
    
            if (temp_index < 0)                 // Negative
            {
                //uart_mini_printf ("-");
                temp_index = ~temp_index;
                temp1=0x80;
            }
            else                                // Positive
            {
                //uart_mini_printf ("+");
                temp1=0x00;
            }
            temp_value = (U16)(hex_to_deci_ascii(temp_index));    
            write_led( temp1 | ((U8)((temp_value>>4) & 0xF0)|(temp_value & 0x0F)));
        
            uart_mini_printf ("%01d °C    \r", temp_index);
        
            if (hyper_term == TRUE)
            {
                if (uart_test_hit() != 0) 
                {
                    temp1 = uart_getchar();
                    break;
                }
            }
            if (get_key() != 0) break;
        }
        wait_for(100); while (get_key() != 0); wait_for(100);
    
        uart_mini_printf ("\r\n");
        
    #else
        uart_mini_printf ("           . Get Temperature Test: Skipped !!!\r\n");
    
    #endif  // _GET_TEMP_TEST_
    
    
    //!*************************************************************************
    //! ........................ Get Luminosity Test
    //! -
    #ifdef _GET_LUM_TEST_
    
        uart_mini_printf ("           . Get Luminosity  Test: (hit PC-key to output)\r\n");
        led_init(); key_init();
    
        while(1)
        {
            // Get luminosity sensor
            lumi_index = get_luminosity ();
            uart_mini_printf ("             = ");
            
            lum_value =hex_to_deci_ascii(lumi_index);
            write_led( temp1 | ((U8)((lum_value>>4) & 0xF0)|(lum_value & 0x0F)));
            
            uart_mini_printf ("%01d Lux     \r", lumi_index);
        
            if (hyper_term == TRUE)
            {
                if (uart_test_hit() != 0) 
                {
                    temp1 = uart_getchar();
                    break;
                }
            }
            if (get_key() != 0) break;
        }
        wait_for(100); while (get_key() != 0); wait_for(100);
    
        uart_mini_printf ("\r\n");
        
    #else
        uart_mini_printf ("           . Get Luminosity  Test: Skipped !!!\r\n");
    
    #endif  // _GET_LUM_TEST_
    
    //!*************************************************************************
    //! ........................ Get Vcc-In Test
    //! -
    #ifdef _GET_LUM_TEST_
    
        uart_mini_printf ("           . Get   Vcc-In    Test: (hit PC-key to output)\r\n");
    
        led_init(); key_init();
    
        while(1)
        {
            // Get Vin
            vin_index = get_vin ();
            uart_mini_printf ("             = ");
    
            vin_value = hex_to_deci_ascii(vin_index);
            write_led((U8)(((vin_value>>12) & 0xF0) | ((vin_value>>8) & 0x0F)));
            
            uart_putchar ((U8)(vin_value>>24));
            uart_putchar ((U8)(vin_value>>16));
            uart_putchar ('.');
            uart_putchar ((U8)(vin_value>>8 ));
            uart_putchar ((U8)(vin_value    ));
            uart_mini_printf (" V     \r");;
           
            if (hyper_term == TRUE)
            {
                if (uart_test_hit() != 0) 
                {
                    temp1 = uart_getchar();
                    break;
                }
            }
            if (get_key() != 0) break;
        }
        wait_for(100); while (get_key() != 0); wait_for(100);
    
        uart_mini_printf ("\r\n");
        
    #else
        uart_mini_printf ("           . Get   Vcc-In    Test: Skipped !!!\r\n");
    
    #endif  // _GET_VIN_TEST_
    
    #ifdef _GET_TEMP_TEST_
     #ifdef _GET_TEMP_TEST_
      #ifdef _GET_TEMP_TEST_
        uart_mini_printf ("         - Tests %cseem%c OK\r\n", '"', '"');
      #endif
     #endif
    #endif
    
        
    //!*************************************************************************
    //! 7th TEST:
    //! ............ Audio Test
    //! -
    //!*************************************************************************
        uart_mini_printf ("7th TEST - Audio Test: ");
    
    #ifdef _AUDIO_TEST_
    
        uart_mini_printf ("(hit <space> PC-key to output)\r\n");
        uart_mini_printf ("           Location of tones on PC keyboard:\r\n\n");
        uart_mini_printf ("           .----.----.    .----.----.    .----.----.----.    .----.----.\r\n");
        uart_mini_printf ("           |SOL#| LA#|    | DO#| RE#|    | FA#|SOL#| LA#|    | DO#| RE#|\r\n");
        uart_mini_printf ("           |::::|::::|    |::::|::::|    |::::|::::|::::|    |::::|::::|\r\n");
        uart_mini_printf ("           | a  | z  |    | r  | t  |    | u  | i  | o  |    | ^  | $  |\r\n");
        uart_mini_printf ("           '-.--'-.--'-.--'-.--'-.--'-.--'-.--'-.--'-.--'-.--'-.--'-.--'\r\n");
        uart_mini_printf ("             | q  | s  | d  | f  | g  | h  | j  | k  | l  | m  | ù  |\r\n");
        uart_mini_printf ("             |    |    |    |    | .. |    |    |    |    |    |    |\r\n");
        uart_mini_printf ("             | LA | SI | DO | RE | MI | FA |SOL | LA | SI | DO | RE |\r\n");
        uart_mini_printf ("             '----'----'----'----'----'----'----'----'----'----'----'\r\n");
    
    
        led_init();  write_led(0x0F);
        key_init();
        audio_init();
    
        temp1=TRUE;
        mute=OFF;   // Mute="OFF" after init
        
        while  (temp1)
        {
            if (mute == OFF)
            {
                write_led(0x0F);
                uart_mini_printf ("         ... Mute (!) = OFF    \r");
            }
            else
            {
                write_led(0x0A);
                uart_mini_printf ("         ... Mute (!) = ON     \r");
            }
    
            ch_in = 0x00;
            if (hyper_term == TRUE) ch_in=uart_test_hit();
            if (ch_in==0x00)
            {
                ch_in = get_key();
                ch_in = ((ch_in>>4) | (ch_in & 0x0F));
            }
            else ch_in = uart_getchar();
            
            switch (ch_in)
            {
                case 0x20:  // space
                    temp1=FALSE;
                    audio_stop();
                    break;
                case 0x04:  // Center key = East or Right key
                    temp1=FALSE;
                    audio_stop();
                    break;
                case 'a':   // Tone G4X (French Sol#  )
                    audio_play(G4X); wait_for(150);
                    break;
                case 0x08:   // Tone A5 on South key or Down key
                    audio_play(A5); wait_for(150);
                    break;
                case 'q':   // Tone A5  (French La  )
                    audio_play(A5); wait_for(150);
                    break;
                case 'z':   // Tone A5X (French La# )
                    audio_play(A5X); wait_for(150);
                    break;
                case 's':   // Tone B5  (French Si  )
                    audio_play(B5); wait_for(150);
                    break;
                case 'd':   // Tone C5  (French Do  )
                    audio_play(C5); wait_for(150);
                    break;
                case 'r':   // Tone C5X (French Do#  )
                    audio_play(C5X); wait_for(150);
                    break;
                case 'f':   // Tone D5  (French Ré  )
                    audio_play(D5); wait_for(150);
                    break;
                case 't':   // Tone D5X (French Ré# )
                    audio_play(D5X); wait_for(150);
                    break;
                case 'g':   // Tone E5  (French Mi  )
                    audio_play(E5); wait_for(150);
                    break;
                case 'h':   // Tone F5  (French Fa  )
                    audio_play(F5); wait_for(150);
                    break;
                case 'u':   // Tone F5X (French Fa# )
                    audio_play(F5X); wait_for(150);
                    break;
                case 'j':   // Tone G5  (French Sol )
                    audio_play(G5); wait_for(150);
                    break;
                case 'i':   // Tone G5X (French Sol#)
                    audio_play(G5X); wait_for(150);
                    break;
                case 'k':   // Tone A6  (French La  )
                    audio_play(A6); wait_for(150);
                    break;
                case 'o':   // Tone A6X (French La# )
                    audio_play(A6X); wait_for(150);
                    break;
                case 'l':   // Tone B6  (French Si  )
                    audio_play(B6); wait_for(150);
                    break;
                case 'm':   // Tone C6  (French Do  )
                    audio_play(C6); wait_for(150);
                    break;
                case '^':   // Tone C6X (French Do# )
                    audio_play(C6X); wait_for(150);
                    break;
                case 'ù':   // Tone D6  (French Ré  )
                    audio_play(D6); wait_for(150);
                    break;
                case '$':   // Tone D6X (French Ré# )
                    audio_play(D6X); wait_for(150);
                    break;
                case 0x01:   // Tone D6X on North key or Up key
                    audio_play(D6X); wait_for(150);
                    break;
                // End of tones
                case '!':   // Mute control
                    if (mute == OFF) mute=ON;
                        else mute=OFF;
                    mute_control(mute);
                    wait_for(100);
                    break;
                case 0x02:   // Mute control on East key or Left key
                    if (mute == OFF) mute=ON;
                        else mute=OFF;
                    mute_control(mute);
                    wait_for(200);
                    break;
                default:
                    audio_play(PAUSE);
                    break;
            }
        }
        wait_for(100); while (get_key() != 0); wait_for(100);
    
        uart_mini_printf ("\r\n         - Test %cseems%c OK\r\n", '"', '"');
    
    #else
        uart_mini_printf ("Skipped !!!\r\n");
    
    #endif  // _AUDIO_TEST_
    
    
    //!*************************************************************************
    //! 8th TEST:
    //! ............ RTC Test
    //! -
    //!*************************************************************************
        uart_mini_printf ("8th TEST - RTC Test: ");
    
    #ifdef _RTC_TEST_
    
        uart_mini_printf ("(hit PC-key to output)\r\n");
    
        led_init(); key_init();
    
        temp1=TRUE;
        temp_value=0;
    
    
        while(1)
        {
            temp =hex_to_deci_ascii(temp_value);
            uart_mini_printf ("           Time = %01d s     \r", temp_value);
    
            write_led((U8)(temp_value+1)); // Count sends to LEDs every 1 seconde
            
            temp2 = 0;
            // Count divided in 10 to be able to get a break
            for(temp1=0; temp1<10; temp1++)
            {
                wait_for(100);          // Call for RTC
                if (hyper_term == TRUE)
                {
                    if (uart_test_hit() != 0)
                    {
                        ch_in = uart_getchar();
                        temp2=1;
                    }
                }
                if (get_key() != 0) temp2=1;
            }
            if (temp2 == 1) break;
            temp_value++;
        }
        wait_for(100); while (get_key() != 0); wait_for(100);
        uart_mini_printf ("\r\n         - Test %cseems%c OK\r\n", '"', '"');
    
    #else
        uart_mini_printf ("Skipped !!!\r\n");
    
    #endif  // _RTC_TEST_
    
    
    //!*************************************************************************
    //! 9th TEST:
    //! ............ CAN Test
    //! -
    //!*************************************************************************
        uart_mini_printf ("9th TEST - CAN Test: ");
    
    #ifdef _CAN_TEST_
    
        uart_mini_printf ("(Automatic output)\r\n");
    
        led_init(); key_init();
    
        // Reset de CAN controller
        CANGCON = 0x01;
        
        // Init Bit Timing: 1 MBs at 8MHz
        CANBT1 = 0x00;
        CANBT2 = 0x04;
        CANBT3 = 0x12;
        
        // Reset DPRAM of the CAN controller
        for (MOb=0; MOb <15; MOb++)
        {
            CANPAGE  = (MOb << 4); // Select the message object
            CANCDMOB = 0x00;
            CANSTMOB = 0x00;
            CANIDT1  = 0x00;
            CANIDT2  = 0x00;
            CANIDT3  = 0x00;
            CANIDT4  = 0x00;
            CANIDM1  = 0x00;
            CANIDM2  = 0x00;
            CANIDM3  = 0x00;
            CANIDM4  = 0x00;
            for (temp1 = 0; temp1 <8; temp1++)
            {
                CANMSG = 0x00; // This register is auto_incremented
            }
        }
        MOb = 3; CANPAGE = (MOb << 4);    // Select the message object nb3
        CANCDMOB = 0x41;                  // Tx MOb, CAN2A, 1 byte
        CANIDT1 = 0x22; CANIDT2 = 0x20;   // ID=0x111
        CANGCON = 0x02;                   // Enable CAN macro
        CANEN2 = MOb;                     // Enable Tx on MOB 3
        
        while (CANSTMOB != 0x01) // Wait for ACK error
        {
            write_led(0x18); wait_for(100);
            write_led(0x24); wait_for(100);
        }
        
        // CAN controller Reset
        CANGCON = 0x01;
    
        uart_mini_printf ("         - Test OK\r\n");
        
    #else
        uart_mini_printf ("Skipped !!!\r\n");
    
    #endif  // _CAN_TEST_
    
            
    //!*************************************************************************
    //! End of Tests:
    //! ............ Exit
    //! -
    //!*************************************************************************
        if (hyper_term == TRUE)
        {
            write_led(0xED);    // LEDs = 0xED like "EnD" if Hyper-Terminal
    
            //! Send test text
            uart_mini_printf ("\nEnd of Tests - If RST key is hit, the tests will restart.\r\n");
            while (1);
        }
        else
        {
            eeprom_wr_block(txt_small_logo_atmel, 0, 41);
			 
            led_init();
            while(1)    // LEDs = Ledchaser if NO Hyper-Terminal
            {
				// Ledchaser 0=>7 with LED functions
                for (temp2=0; temp2<8;temp2++)      // Endless if false
                {
                    write_led_number(temp2,1);
                    wait_for(40);
                    write_led_number(temp2,0);
                }
				// Ledchaser 7=>0 with LED macros
				wait_for(40);
//				Set_led(7); wait_for(40); Clr_led(7);
				Set_led(6); wait_for(40); Clr_led(6);
//				Set_led(5); wait_for(40); Clr_led(5);
				Set_led(4); wait_for(40); Clr_led(4);
//				Set_led(3); wait_for(40); Clr_led(3);
				Set_led(2); wait_for(40); Clr_led(2);
//				Set_led(1); wait_for(40); Clr_led(1);
				Set_led(0); wait_for(40); Clr_led(0); 

            }
        }

    } //! ....... End of test part .............................................
    
    else
     //! ........................................................................
    //! ....... Demo part ......................................................
    //! ........................................................................
   {

        //!--- I N I T
        //! Clock prescaler Reset
        CLKPR = 0x80;  CLKPR = 0x00;
        DDRA =0xFF;
        
        //! Pull-up on TxCAN & RxCAN one by one to use bit-addressing
        CAN_PORT_DIR &= ~(1<<CAN_INPUT_PIN );
        CAN_PORT_DIR &= ~(1<<CAN_OUTPUT_PIN);
        CAN_PORT_OUT |=  (1<<CAN_INPUT_PIN );
        CAN_PORT_OUT |=  (1<<CAN_OUTPUT_PIN);
        
        //! --- Init an get Keys configuration:
        key_init(); mode = get_key();
        
        //! --- Init LED's configuration:
        led_out = 0;
        led_init(); write_led(led_out);
        
        //! Init UART-0 at 38400 bauds
        uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE_DEMO);
            
        uart_mini_printf ("\r\n\n======= CAN SPY ECHO EXAMPLE =======\r\n");
        uart_mini_printf ("%s",txt_logo_atmel);
        if(mode==0) uart_mini_printf ("%s\r\n\n",txt_can_ana_2);
        else uart_mini_printf ("%s\r\n\n",txt_can_ana_1);
        uart_mini_printf (" ... No Activity on CAN Bus ! ...\r");
               
        //! --- Init CAN (special AUTOBAUD)  
            //- Wait until activity on RxCAN
        while ((CAN_PORT_IN & (1<<CAN_INPUT_PIN)) != 0);
            //- Reset CAN peripheral
        Can_reset();
            //- Set CAN Bit-timming
        can_init((U16)CAN_BAUDRATE);        // c.f. macro in "can_drv.h"
            //- Set CAN Timer Prescaler
        CANTCON = CANBT1;                   // Why not !
        
        //! --- C O R E   O F   C A N - E C H O
        while(1)
        {
        //! CAN ECHO: WAIT FOR RECEIVED
            //! --- Init Rx data
            message.pt_data = &buffer[0];
            for(i=0; i<8; i++) buffer[i]=0;
        
            //! --- Rx Command
            message.cmd = CMD_RX;
            
            //! --- Enable Rx
            while(can_cmd(&message) != CAN_CMD_ACCEPTED);
            //! --- Wait for Rx completed
            while(1)
            {
                u8_temp = can_get_status(&message);
                if (u8_temp != CAN_STATUS_NOT_COMPLETED) break; // Out of while
            }
            if (u8_temp == CAN_STATUS_ERROR) break; // Out of the function
        
        //! CAN ECHO: PRINT-UART OF CAN FRAME RECEIVED
            write_led(led_out); led_out++;
            spy_printing(RXCAN, &message);
        
            //! ---- Exit if CAN ID == 0
            if (message.id.ext == 0) break; // Out of function (CAN ERROR simulation)
            if (message.id.std == 0) break; // Out of function (CAN ERROR simulation)
            
            
        //! CAN ECHO: RESPONSE
            
            if(mode==0)
            {
                //! --- Init Tx data
                buffer[6] = CANSTMH;
                buffer[7] = CANSTML;
                
                //! --- Tx Command
                message.id.ext++;   // Incrementation of ID to revome possible clashes
                message.dlc = 8;
                message.cmd = CMD_TX_DATA;
                
                //! --- Enable Tx
                while(can_cmd(&message) != CAN_CMD_ACCEPTED);
                //! --- Wait for Tx completed        
                while(1)
                {
                    u8_temp = can_get_status(&message);
                    if (u8_temp != CAN_STATUS_NOT_COMPLETED) break; // Out of while
                }
                //! CAN ECHO: PRINT-UART OF CAN FRAME TRANSMITTED
                spy_printing(TXCAN, &message);
        
                            //! ---- Exit if CAN error(s)
                if (u8_temp == CAN_STATUS_ERROR) break; // Out of function
            }
        }
        can_error_mode();
    }
    return 0;
}



//_____ F U N C T I O N S ______________________________________________________


//------------------------------------------------------------------------------
//  @fn spy_printing
//!
//! CAN communications are resumed on UART_0 and UART_1 (The UART number is at
//!   the end & at the beginning of each generated
//!
//! Note: If some key is pushed while RESET, only Spy mode is enabled.
//!
//! @warning UART's at 38400 bauds
//!
//! @param  - 'R' for receive frame, 'T' for transmit frame.
//!         - CAN message 
//!
//! @return  none
//!
//------------------------------------------------------------------------------
void spy_printing(U8 rxtx, st_cmd_t* msg)
{
    U8 indx;

    if (msg->ctrl.ide) 
    {
        uart_mini_printf ("%cxCAN @ %02X%02X: 0x%08lX(Ext.), L=%d, ", rxtx, CANSTMH, CANSTML, msg->id.ext, msg->dlc);
    }
    else
    {
        uart_mini_printf ("%cxCAN @ %02X%02X:      0x%03X(Std.), L=%d, ", rxtx, CANSTMH, CANSTML, msg->id.std, msg->dlc);
    }
    if (msg->ctrl.rtr)
    {
        uart_mini_printf ("Remote\r\n"); 
    } 
    else
    {
        if (msg->dlc == 0)
        {
            uart_mini_printf ("No data\r\n");
        }
        else
        {
            for(indx=0; indx< (msg->dlc - 1); indx++)
            {
                uart_mini_printf ("%02X-", *(msg->pt_data + indx)); 
            }
            uart_mini_printf ("%02X\r\n", *(msg->pt_data + indx));
        }
    }                  

}

//------------------------------------------------------------------------------
//  @fn can_error_mode
//!
//! Exit in UART ECHO MODE if CAN ID == 0 as CAN error simulation.
//!
//! @warning UART's at 38400 bauds
//!
//! @param  none.
//!
//! @return  none
//!
//------------------------------------------------------------------------------
void can_error_mode(void)
{
    U8 i;
    U8 txt_ech_mod[]="======= UART in ECHO MODE =======";

    uart_mini_printf ("\r\n======= CAN LINE ERROR ");
    
    for(i=0; i<6; i++)
    {
        wait_for(1000); //- Wait for 1000 milli-seconds (1 s)
        uart_mini_printf ("*");
    }

    wait_for(1000); //- Wait for 1000 milli seconds

    uart_mini_printf (" Exit From Program! =======\r\n\n");
    
//- UART ECHO
    uart_mini_printf ("%s\r\n",txt_ech_mod);
    while(1)
    {
        if (uart_test_hit())
        {
            uart_putchar(uart_getchar());   // Echo on UART-0
        }
    }
}

//------------------------------------------------------------------------------
//  @fn hex_to_deci_ascii
//!
//! Convert an hexadecimal U16 to x4 ASCII characters in an U32.
//!
//! @warning Maximum value of input paramater = 0x270F or 9999.
//!
//! @param hex_to_conv   hexa U16 to convert (up to 0x270F)
//!
//! @return x4 ASCII characters in an U32.
//!          Ex: 0x008A will be convert in 0x30313338 = '0138'
//!              0x1A5D will be convert in 0x36373439 = '6749'
//!
//------------------------------------------------------------------------------
U32 hex_to_deci_ascii(U16 hex_to_conv)
{
    U32 ascii_result = 0x30303030;
    U32 temp_a=0;
    U32 temp_b=0;
    U32 temp_c=0;

    while (hex_to_conv > 9)
        {
        temp_a ++;
        hex_to_conv -= 10;
        }
    while (temp_a > 9)
        {
        temp_b ++;
        temp_a -= 10;
        }
    while (temp_b > 9)
        {
        temp_c ++;
        temp_b -= 10;
        }
    ascii_result |= temp_c <<24;
    ascii_result |= temp_b <<16;
    ascii_result |= temp_a << 8;
    ascii_result |= hex_to_conv;

    return ascii_result;
}





