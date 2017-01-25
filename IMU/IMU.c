/*
 * IMU.c
 *	Original Author : Mark Shaumburg
 *  	Created on: Feb 4, 2012
 *  Revision 2: Jason McLoud
 * 		1/2/2017
 * 		Notes: Removed Suspesion Travel(Potetionmeter), utilized Atmel Libraries(CAN, UART)
 *	TODO: Compass
 *
 *	 Send 3-Axis Accel and 3-Axis Gyro Data 
 *			 Data sent through CAN at predetermined frequency 
 *			 Datalogging System recieves and logs
 */


#include "IMU.h"

int main(void)
{	
	cli();
	//Set DDR for port D and E
	DDRD|=_BV(PD7);
	DDRE|=_BV(PE6);

	uint8_t i;

#ifdef DEBUG
	stdout = stdin = &uart_str;
	uart_init(UART1_BAUD);
	//uart only utilized for usb/serial debugging purposes
#endif
	//i2c_init(50000UL);
	//can_init();

	uint8_t resp;

	//CAN Init section
	can_reset();
	can_init(1);
	while ((CANGSTA & (1<<ENFG)) == 0); //! Wait for Enable OK
	CANGIT = 0xFF;                      //! Reset General errors and OVRTIM flag
	can_clear_all_mob();				//Clear all Mobs

	//InitCanMsgs();

	i2c_init();

#ifdef DEBUG
	printf("Start\n");

	if((resp = acl_init())!= 0)	printf("ACL initialization error: %u\n",resp);
	_delay_us(1000);
	if((resp = gyro_init()!= 0) printf("Gyro initialization error: %u\n", resp));
	_delay_us(1000);
#else
	acl_init();
	_delay_us(1000);
	gyro_init();
	_delay_us(1000);
#endif

	sts_init();

	sei();

	systimer_10ms_restart(10);

	while(1)
	{
			acl_get_values();
			_delay_us(100);
			gyro_get_values();
			_delay_us(100);
	}
}

/*
*	Called by timer compare match ISR
*	Compare Vector set to trigger on 10ms intervals
*	handle_10ms_tick: handle for sending messages to can controller
*	USAGE: set data output frequency, based upon current systimer val
*/
void handle_10ms_tick(void)
{	
	//TODO: send message for gyro and accel
	//		transition code to use Atmel can lib
	if(systimer_10ms %10 == 0)
	{
		imu2.data[0]=(gyro_x >> 8) & 0x00FF;
		imu2.data[1]=(gyro_x & 0x00FF);
		imu2.data[2]=(gyro_y >> 8) & 0x00FF;
		imu2.data[3]=(gyro_y & 0x00FF);
		imu2.data[4]=(gyro_z >> 8) & 0x00FF;
		imu2.data[5]=(gyro_z & 0x00FF);

		
	}

	if(systimer_10ms %20 == 0)
	{
		// Send 50Hz Data
		//led_ctrl(LED2,LED_ON,LED_NO_TMR);
	}

	if(systimer_10ms % 50 == 0)
	{
		// Send 20Hz Data
		//led_ctrl(LED3,LED_ON,LED_NO_TMR);
	}

	if(systimer_10ms % 100 == 0)
	{
		// Send 10Hz Data
		

	}

	if(systimer_10ms % 200 == 0)
	{
		// Send 5Hz Data
		//led_ctrl(LED5,LED_ON,LED_NO_TMR);
	}
	if(systimer_10ms % 500 == 0)
	{
		// Send 2Hz Data
		//led_ctrl(LED6,LED_ON,LED_NO_TMR);
	}
	if(systimer_10ms % 1000 == 0)
	{
		// Send 1Hz Data
		//led_ctrl(LED7,LED_ON,LED_NO_TMR);
	
	}
}

