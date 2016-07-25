/*
 * ThermopileConfig
 * connects to a single mlx90614 via TWI and can set address & emissivity
 * Written by Matthew Nevels
 * matthew@enevels.com
 * 2/27/10
 */

#include <util/delay.h> //for time delays
#include "uart.h"		//for uart
#include "uart_extended.h" //for extended uart functions (uart_getstring, uart getfloat, etc...)
#include "i2cmaster.h"
#include "mlx90614.h"	//for thermopile functions

#define SlaveAddress      0x00  //All sensors listen to this address as well as their own. 
#define UART1_BAUD	9600UL

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void printMenu(void){
  //print menu
  printf_P(PSTR("\n-------------------------\n"));
  printf_P(PSTR(  "1. Read Temperature      \n"));
  printf_P(PSTR(  "2. Read Emissivity       \n"));
  printf_P(PSTR(  "3. Set Emissivity        \n"));
  printf_P(PSTR(  "4. Read SMBus Address    \n"));
  printf_P(PSTR(  "5. Set SMBus Address     \n"));
  printf_P(PSTR(  "-------------------------\n"));
}

int main(void)
{

uart_init(UART1_BAUD);
stdout = stdin = &uart_str;

//Initialise the i2c bus
i2c_init();
//enable pullups
PORTC = (1 << PORTC4) | (1 << PORTC5);

printf_P(PSTR("\r------------------------------------------\r"));
printf_P(PSTR(  "           Thermopile Config              \r"));
printf_P(PSTR(  "------------------------------------------\r"));
printMenu();
int i = 0;
while(1)
	{
    printf_P(PSTR(  "Select an item (enter for menu): "));
  switch (uart_getint(&uart_str)) {
  case 1:
	while(1){
	
      //infinitely Print Temperature
      printf_P(PSTR("%d,%f,%f\n"),i,mlx_getAmbTempF(SlaveAddress),mlx_getObjTempF(SlaveAddress));
      _delay_ms(500); 
	  i++;
   }
    break;
  case 2:
    //Print Emissivity
    printf_P(PSTR("Emissivity: %f\n"),mlx_getEmissivity(SlaveAddress));
    break;
  case 3:
	//Set Emissivity
    printf_P(PSTR("Current Emissivity: %f\n"),mlx_getEmissivity(SlaveAddress));  
	printf_P(PSTR("Enter new emissivity value between (0 to 1): "));
    printf_P(PSTR("New emissivity: %f\n"),mlx_setEmissivity(SlaveAddress,uart_getfloat(&uart_str)));
    break;
  case 4:
    //Print Address
    printf_P(PSTR("Current Address: %d\n"),mlx_getAddress());
    break;
  case 5:
    //Set Address
	printf_P(PSTR("Current Address: %d\n"),mlx_getAddress());
    printf_P(PSTR("Enter a new address (0 to 127):"));
    printf_P(PSTR("New Address = %d"),mlx_setAddress(SlaveAddress,(unsigned char)uart_getint(&uart_str)));
    printf_P(PSTR("\n******* YOU MUST CYCLE POWER BEFORE NEW ADDRESS IS EFFECTIVE *******\n"));
    break;
  default: 
	printMenu();
	break;
  }

	}

}//end main

