//**************************************************************
//******** FUNCTIONS FOR SERIAL COMMUNICATION USING UART *******
//**************************************************************
//Controller:	ATmega128 (Crystal: 16Mhz)
//Compiler:		winAVR (AVR-GCC)
//Author:		CC Dharmani, Chennai (India)
//				www.dharmanitech.com
//Date:			April 2009
//********************************************************

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "UART_routines.h"


//**************************************************
//Function initialize UART0
//desired baud rate: 19200
//actual: baud rate:19231 (0.2%)
//char size: 8 bit
//parity: Disabled
//**************************************************
void uart0_init(void)
{
 UCSR1B = 0x00; //disable while setting baud rate
 UCSR1A = 0x00;
 UCSR1C = 0x06;
 UBRR1L = 0x33; //set baud rate lo
 UBRR1H = 0x00; //set baud rate hi
 UCSR1B = 0x98;
}
//**************************************************
//Function to receive a single byte
//*************************************************
unsigned char receiveByte( void )
{
	unsigned char data, status;
	
	while(!(UCSR1A & (1<<RXC1))); 	// Wait for incomming data
	
	status = UCSR1A;
	data = UDR1;
		   
	return(data);
}

//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte( unsigned char data )
{
	while ( !(UCSR1A & (1<<UDRE1)) )
		; 			                /* Wait for empty transmit buffer */
	UDR1 = data; 			        /* Start transmition */
}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(char* string)
{
  while (pgm_read_byte(&(*string)))
   transmitByte(pgm_read_byte(&(*string++)));
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(char* string)
{
    while (*string)
		transmitByte(*string++);
}
