/*
 * STS_interface.c
 *
 *  Created on: Mar 28, 2012
 *      Author: Administrator
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"
#include "i2cmaster.h"
#include "tmp_sensor.h"
#include "CRC8.h"

#define UART1_BAUD 57600UL
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void)
{
	DDRA |= _BV(PA0)|_BV(PA1);
	PORTD |= _BV(PD3);

	stdin = stdout = &uart_str;
	uart_init(UART1_BAUD);

	i2c_init();

	printf("|-----Start-----|\n");

	/*
	if(temp_init(0))
	{
		printf("Device Not Found\n");
		while(1);
	}
	*/

	//printf("Device Found\n");

	//printf("Setting Address\n");
	//mlx_setAddress(0x5A<<1,TMP_FRI_ADDR>>1);


	//printf("Device Address: 0x%02X\n",mlx_getAddress());

	while(1)
	{
		printf("FL:%u / %u / %u FR:%u / %u / %u RL:%u / %u / %u RR:%u / %u / %u\n",
				mlx_getObjTempK(TMP_FLO_ADDR),mlx_getObjTempK(TMP_FLM_ADDR),mlx_getObjTempK(TMP_FLI_ADDR),
				mlx_getObjTempK(TMP_FRO_ADDR),mlx_getObjTempK(TMP_FRM_ADDR),mlx_getObjTempK(TMP_FRI_ADDR),
				mlx_getObjTempK(TMP_RLO_ADDR),mlx_getObjTempK(TMP_RLM_ADDR),mlx_getObjTempK(TMP_RLI_ADDR),
				mlx_getObjTempK(TMP_RRO_ADDR),mlx_getObjTempK(TMP_RRM_ADDR),mlx_getObjTempK(TMP_RRI_ADDR));
		_delay_ms(100);
	}




}
