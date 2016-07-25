//*************************************************************
//*** FUNCTIONS FOR EEPROM READ/WRITE ON I2C COMMUNICATION  ***
//*************************************************************
//Controller:	ATmega128 (Crystal: 16Mhz)
//Compiler:		winAVR (AVR-GCC)
//Author:		CC Dharmani, Chennai (India)
//				www.dharmanitech.com
//Date:			April 2009
//********************************************************

//link to post: http://www.dharmanitech.com/2008/08/interfacing-rtc-serial-eeprom-using-i2c.html

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "EEPROM_routines.h"
#include "UART_routines.h"
#include "i2c_routines.h"


//******************************************************************
//Function to read given number of bytes from EEPROM 
//******************************************************************
unsigned char EEPROM_read(unsigned char highAddress, unsigned char lowAddress, unsigned char totalChar)
{
   unsigned char errorStatus, i, data;
   
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("i2c start failed.."));
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendAddress(EEPROM_W);
   
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM sendAddress1 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendData(highAddress);
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM write-1 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
 
   errorStatus = i2c_sendData(lowAddress);
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM write-2 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_repeatStart();
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("i2c repeat-start failed.."));
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendAddress(EEPROM_R);
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM sendAddress2 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
   for(i=0;i<totalChar;i++)
   {
      if(i == (totalChar-1))  	 //no Acknowledge after receiving the last byte
	   	  data = i2c_receiveData_NACK();
	  else
	  	  data = i2c_receiveData_ACK();
		  
   	  if(data == ERROR_CODE)
   	  {
       		transmitString_F(PSTR("EEPROM receive failed.."));
			i2c_stop();
	   		return(1);
   	  }
	   
	   if((i%16) == 0) 
	     TX_NEWLINE;
	   transmitByte(data);
	   
   }
   
   TX_NEWLINE;
   i2c_stop();
   
   return(0);
}

//******************************************************************
//Function to a character string to EEPROM 
//******************************************************************
unsigned char EEPROM_write(unsigned char highAddress, unsigned char lowAddress)
{
   unsigned char errorStatus, data;
   
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("i2c start failed.."));
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendAddress(EEPROM_W);
   
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM sendAddress1 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendData(highAddress);
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM write-1 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
 
   errorStatus = i2c_sendData(lowAddress);
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM write-2 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
    while((data = receiveByte()) != 0x0d)
   {
      transmitByte(data);
	  if(data == 0x0d)
 	  transmitByte(0x0a);
	  errorStatus = i2c_sendData(data);
   	  if(errorStatus == 1)
   	  {
       		transmitString_F(PSTR("EEPROM write data failed.."));
			i2c_stop();
	   		return(1);
   	  }
   }
   
   i2c_stop();
   
   return(0);
}

//******************************************************************
//Function to read one page of EEPROM 
//******************************************************************
unsigned char EEPROM_readPage( unsigned int pageNumber )
{
  unsigned char highAddress, lowAddress, errorStatus;
  unsigned int pageAddress;
  
  pageAddress = pageNumber * 64;
  
  highAddress = (unsigned char)((pageAddress >> 8) & 0x00ff);
  lowAddress  = (unsigned char)( pageAddress & 0x00ff);
  
  errorStatus = EEPROM_read(highAddress, lowAddress, 64);
  
  return(errorStatus);
 } 
  
//******************************************************************
//Function to write one page of EEPROM 
//******************************************************************
unsigned char EEPROM_writePage( unsigned int pageNumber )
{
  unsigned char highAddress, lowAddress, errorStatus;
  unsigned int pageAddress;
  
  pageAddress = pageNumber * 64;
  
  highAddress = (unsigned char)((pageAddress >> 8) & 0x00ff);
  lowAddress  = (unsigned char)( pageAddress & 0x00ff);
  
  errorStatus = EEPROM_write(highAddress, lowAddress);
  
  return(errorStatus);  
} 
 
//******************************************************************
//Erasing EEPROM (filling it with databyte 0xff)
//******************************************************************
unsigned char EEPROM_erase(void)
{
  
  unsigned char errorStatus;
  unsigned int i;
  
  TX_NEWLINE;
  TX_NEWLINE;
  transmitString_F(PSTR("Wait..."));
  TX_NEWLINE;
   
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("i2c start failed.."));
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendAddress(EEPROM_W);
   
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM sendAddress1 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendData(0x00);
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM write-1 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
 
   errorStatus = i2c_sendData(0x00);
   if(errorStatus == 1)
   {
     transmitString_F(PSTR("EEPROM write-2 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
    for(i=0;i<0x8000;i++)
   {
	  errorStatus = i2c_sendData(0xff);
   	  if(errorStatus == 1)
   	  {
       		transmitString_F(PSTR("EEPROM write data failed.."));
			i2c_stop();
	   		return(1);
   	  }
   }
   
   i2c_stop();
   
   return(0);

}   	  
