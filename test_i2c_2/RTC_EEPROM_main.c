//********************************************************
// ******** PROGRAM FOR ATE RTC COMMUNICATION ***********
//********************************************************
//Controller:	ATmega128 (Crystal: 16Mhz)
//Compiler:		winAVR (AVR-GCC)
//Author:		CC Dharmani, Chennai (India)
//				www.dharmanitech.com
//Date:			April 2009
//********************************************************

//link to post: http://www.dharmanitech.com/2008/08/interfacing-rtc-serial-eeprom-using-i2c.html

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "UART_routines.h"
#include "i2c_routines.h"
#include "EEPROM_routines.h"
#include "RTC_routines.h"

		  
void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00;
 DDRC  = 0xff;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x00;
}

//TWI initialize
// bit rate:18 (freq: 100Khz)
void twi_init(void)
{
 TWCR= 0X00; //disable twi
 TWBR= 0x12; //set bit rate
 TWSR= 0x01; //set prescale
 TWCR= 0x44; //enable twi
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 //XDIV  = 0x00; //xtal divider
 XMCRA = 0x00; //external memory
 port_init();
 uart0_init();
 twi_init();
 
 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK2 = 0x00; //timer interrupt sources
 //ETIMSK = 0x00; //extended timer interrupt sources
 //SEI(); //re-enable interrupts
} 

//******************* MAIN FUNCTION *******************

 int main(void)
{
   unsigned char errorStatus, data, option, totalChar;
   unsigned int pageNumber;
   
   init_devices();
   
   TX_NEWLINE;
   TX_NEWLINE;
   TX_NEWLINE;
   transmitString_F(PSTR(">- RTC & SERIAL EEPROM Testing!---"));
   TX_NEWLINE;
   transmitString_F(PSTR(">-------- By CC Dharmani ---------"));
   TX_NEWLINE;
   transmitString_F(PSTR(">----------------------------------"));
   TX_NEWLINE;
   transmitString_F(PSTR(">Communication Started..."));

   TX_NEWLINE
   transmitString_F(PSTR("Enabling Clock\n"));
   i2c_start();
   i2c_sendAddress(DS1307_W);
   i2c_sendData(0);
   i2c_sendData(0);
   i2c_stop();
  
   
   while(1)
   {
   TX_NEWLINE;
   TX_NEWLINE;
   transmitString_F(PSTR("Press any key.."));
   option = receiveByte();
      
    
   TX_NEWLINE;
   transmitString_F(PSTR("> 0 : Erase EEPROM"));
   TX_NEWLINE;
   transmitString_F(PSTR("> 1 : Write EEPROM"));
   TX_NEWLINE;
   transmitString_F(PSTR("> 2 : Read EEPROM"));
   TX_NEWLINE;
   transmitString_F(PSTR("> 3 : Write EEPROM Page"));
   TX_NEWLINE;
   transmitString_F(PSTR("> 4 : Read EEPROM Page"));
   TX_NEWLINE;
   transmitString_F(PSTR("> 5 : Display RTC Date"));
   TX_NEWLINE;
   transmitString_F(PSTR("> 6 : Display RTC Time"));
   TX_NEWLINE;
   transmitString_F(PSTR("> 7 : Update RTC Date"));
   TX_NEWLINE;
   transmitString_F(PSTR("> 8 : Update RTC Time"));
   TX_NEWLINE;
   TX_NEWLINE;
   transmitString_F(PSTR("> Select Option (0-8): "));
   
   option = receiveByte();
   transmitByte(option);
   
   errorStatus = 0;
   totalChar = 0;
   pageNumber = 0;
   
   switch (option)
   {
    
	case '0': errorStatus = EEPROM_erase();
   			  TX_NEWLINE;
			  if(errorStatus == 1)
     		     transmitString_F(PSTR(" EEPROM erase failed.."));
			  else
			     transmitString_F(PSTR(" EEPROM erased successfully.."));
   			  break;
	case '1': TX_NEWLINE;
   		 	  transmitString_F(PSTR(" Enter text: "));
			  errorStatus = EEPROM_write(0x00,0x00);
   			  break;
	case '2': TX_NEWLINE;
   		 	  transmitString_F(PSTR(" How many bytes?(00-64): "));
			  data = receiveByte(); transmitByte(data);
			  totalChar = (data & 0x0f) * 10;
			  data = receiveByte(); transmitByte(data);
			  totalChar += (data & 0x0f);
			  TX_NEWLINE;
			  errorStatus = EEPROM_read(0x00,0x00,totalChar);
   			  break;
	case '3': 
	case '4': TX_NEWLINE;
   		 	  transmitString_F(PSTR(" Enter Page Number (000-511): "));
			  data = receiveByte(); transmitByte(data);
			  pageNumber = (data & 0x0f) * 100;
			  data = receiveByte(); transmitByte(data);
			  pageNumber += (data & 0x0f) * 10;
			  data = receiveByte(); transmitByte(data);
			  pageNumber += (data & 0x0f);
			  if(pageNumber > 511)
			  {
			    TX_NEWLINE;
   		 	  	transmitString_F(PSTR(" Address out of range.. "));
				break;
			  }	
			  TX_NEWLINE;
			  if(option == '3')
			  {
				 TX_NEWLINE;
   		 	  	 transmitString_F(PSTR(" Enter text: "));
				 errorStatus = EEPROM_writePage(pageNumber);
			  } 
			  else
			     errorStatus = EEPROM_readPage(pageNumber);
   			  break;
	case '5': RTC_displayDate();
		 	  break;
	case '6': RTC_displayTime();
		 	  break;
	case '7': RTC_updateDate();
		 	  break;
	case '8': RTC_updateTime();
		 	  break;
	default:  TX_NEWLINE;
   		 	  transmitString_F(PSTR(" Invalid option!"));
			  TX_NEWLINE;
	}		  
   
    if(errorStatus == 1)
   {
     transmitString_F(PSTR(" EEPROM operation failed.."));
	 TX_NEWLINE;
   }
   
   TX_NEWLINE;
   transmitString_F(PSTR(" Finished.."));
   TX_NEWLINE;
   }
   
   return 0;
}

