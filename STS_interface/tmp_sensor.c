/*
 * tmp_sensor.c
 *
 *  Created on: Mar 24, 2012
 *      Author: Administrator
 */

#include <util/atomic.h>
#include "tmp_sensor.h"
#include <util/delay.h>

volatile uint8_t temps[12] = {255,255,255,255,255,255,255,255,255,255,255,255};

uint8_t temp_init(uint8_t addr)
{
	if(addr == 0) return 1;

	if(i2c_start(addr+I2C_READ))
	{
		i2c_stop();
		return 1;
	}
	i2c_read(ACK);
	i2c_read(ACK);
	i2c_read(ACK);
	i2c_stop();
	return 0;
}

//read two bytes from address using command
unsigned int mlx_read(unsigned int address, unsigned int command){
  unsigned char low,high,pec;
  //int dev = address<<1; //shift 1 so we can + read/write bit on next line
	if(i2c_start(address+I2C_WRITE))
	{
		i2c_stop();
		return 0;
	}
  i2c_write(command);			//command (mlx90614 command + address)
  i2c_rep_start(address+I2C_READ);	//repeated start read from SMBus address
  low = i2c_readAck(); 			//Read low byte and then send ack
  high = i2c_readAck(); 		//Read high byte and then send ack
  pec = i2c_readAck();			//Read pec and then send nak
  i2c_stop();					//stop i2c
  return low + (high << 8);
}

//Write two bytes to address using command
void mlx_write(unsigned char address, unsigned char command, unsigned int newvalue){
  unsigned char high,low,pec,arr[6];
  //Erase location at address
  high = 0x00;
  low = 0x00;
  //int dev = address<<1; //shift 1 so we can OR the read/write bit
  arr[5]=0x00;		    //must be 0 for pec calculation
  arr[4]=address+I2C_WRITE;	//address
  arr[3]=command;		//command (mlx90614 command + address)
  arr[2]=0x00;			//Low byte
  arr[1]=0x00;			//High byte
  arr[0]=0x00;			//must be 0 for pec calculation
  pec=PEC_calculation(arr);//Calculate CRC

  i2c_start_wait(address+I2C_WRITE); //start i2c write to SMBus address
  i2c_write(command);	//command (mlx90614 command + address)
  i2c_write(0x00);  	//Low byte
  i2c_write(0x00);  	//High byte
  i2c_write(pec); 		//pec
  i2c_stop();			//stop i2c
  _delay_ms(10);		//wait for sensor to write

  //Write newvalue to location at address
  high = (newvalue >> 8);                    //separate high byte
  low = (newvalue - ((newvalue >> 8) << 8)); //separate low byte

  arr[5]=0x00;		    //must be 0 for pec calculation
  arr[4]=address+I2C_WRITE;	//address
  arr[3]=command;		//command (mlx90614 command + address)
  arr[2]=low;			//Low byte
  arr[1]=high;			//High byte
  arr[0]=0x00;			//must be 0 for pec calcuation
  pec=PEC_calculation(arr);//Calculate CRC

  i2c_start_wait(address+I2C_WRITE); //start i2c write to SMBus address
  i2c_write(command);	//command (mlx90614 command + address)
  i2c_write(low);  		//Low byte
  i2c_write(high);  	//High byte
  i2c_write(pec); 		//packet error code
  i2c_stop();			//stop i2c
  _delay_ms(10);		//wait for sensor to write
}

uint8_t temp_get_value(uint8_t addr,uint8_t idx)
{
	uint8_t t0,t1,pec;

	int32_t t;

	if(addr == 0)	return(1);

	if(i2c_start(addr+I2C_WRITE))
	{
		i2c_stop();
		return(1);
	}

	i2c_write(TMPSNSR_VAL);

	if(i2c_rep_start(addr+I2C_READ))
	{
		i2c_stop();
		return 2;
	}

	t0 = i2c_read(ACK);
	t1 = i2c_read(ACK);
	pec = i2c_read(ACK);
	i2c_stop();

	if(t1 & 0x80) // Error bit set
	{
		//return 3;
	}
	t = (int16_t)(((uint16_t)(t1 & 0x7F) << 8) + (uint16_t)t0);

	t = (9*(t/50 - 273))/5 + 32;

	if(t < 0)	t = 0;
	if(t > 255) t = 255;

	// Guarantee atomic write
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		temps[idx] = (uint8_t) (t & 0x000000FF);
	}

	return 0;
}

//read current SMBus address
//reads from address 0, assuming there are no other mlx90614s on the bus
unsigned int mlx_getAddress(void){
  unsigned int address = mlx_read(0,0x2E); //0x2E is Melexis command to access EEPROM address 0x0E
  return (address - ((address >> 8) << 8)); //return low byte only
}

//read emissivity value
float mlx_getEmissivity(unsigned char address){
  float emiss = mlx_read(address,0x24);//0x24 is Melexis command to access EEPROM address 0x04
  return (emiss+1)/65536;
}

//set emissivity value
float mlx_setEmissivity(unsigned char address, float newvalue){
    if (newvalue < 0 || newvalue >1) return -1;
    newvalue = newvalue*65536.-1.; 	//Calculate Emissivity parameter
    mlx_write(address,0x24,(unsigned int)newvalue);//0x24 is Melexis command to access EEPROM address 0x04
	return mlx_getEmissivity(address);
}

int mlx_setAddress(unsigned char oldaddress, unsigned char newaddress){
	//if(newaddress > 127) return -1;//Address is 7 bits in MLX90614
	mlx_write(oldaddress,0x2E,newaddress);//0x2E is Melexis command to access EEPROM address 0x0E
	return mlx_getAddress();
}

//read object temperature in Kelvin
uint16_t mlx_getObjTempK(unsigned char address){
  return (2*mlx_read(address,0x07))/100; //0x07 is Melexis command to access RAM address 0x07
}

/*
//read object temperature in Kelvin
float mlx_getObjTempC(unsigned char address){
  return mlx_getObjTempK(address) - 273.15;
}

//read object temperature in Celsius
float mlx_getObjTempF(unsigned char address){
  return 9. * mlx_getObjTempC(address) / 5. + 32.;
}

//read ambient temperature in Farenheit
float mlx_getAmbTempK(unsigned char address){
  float tempFactor = 0.02; // 0.02 degrees per LSB
  return tempFactor * mlx_read(address,0x06); //0x06 is Melexis command to access RAM address 0x06
}

//read ambient temperature in Celsius
float mlx_getAmbTempC(unsigned char address){
  return mlx_getAmbTempK(address) - 273.15;
}

//read ambient temperature in Farenheit
float mlx_getAmbTempF(unsigned char address){
  return 9. * mlx_getAmbTempC(address) / 5. + 32.;
}
*/








