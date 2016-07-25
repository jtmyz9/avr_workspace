/*
 * IMCP.h
 *
 *  Created on: Jan 9, 2012
 *      Author: Administrator
 */

#ifndef IMCP_H_
#define IMCP_H_

// Inter-Module Communication protocol

#define CAN_MSGTYPE_NONE 0
#define CAN_MSGTYPE_DATABRDCST 1

// Standard Message (11-bit id)

// Addresses
#define CAN_ADDR_RSVD_00	0x00
#define CAN_ADDR_RSVD_FF	0xFF
#define CAN_ADDR_DLG		0x01
#define CAN_ADDR_STW		0x02
#define CAN_ADDR_TCS		0x03
#define CAN_ADDR_IMU		0x04
#define CAN_ADDR_WRL		0x05
#define CAN_ADDR_STS		0x06

// Message Types
#define CAN_GEN		0x00	// General (Broadcast) Message
#define CAN_ERR 	0x01	// Error Message
#define CAN_DAT 	0x02	// Data Message
#define CAN_DRSP 	0x03	// Data Response
#define CAN_CMD 	0x04	// Command Message
#define CAN_CRSP	0x05	// Command Response
#define CAN_RSVD_06	0x06	// Reserved for later use (Bootloader, etc)
#define CAN_RSVD_07	0x07	// Reserved for later use (Bootloader, etc)

// General Messages
#define CAN_GEN_HB		0x01
#define CAN_GEN_HB_RSP 	0x02

// Commands



// Extended Message (29-Bit)
// data broadcast ids (J1939)

//
#define CAN_DATA_PE1	0x0CFFF048
#define CAN_DATA_PE2	0x0CFFF148
#define CAN_DATA_PE3	0x0CFFF248
#define CAN_DATA_PE4	0x0CFFF348
#define CAN_DATA_PE5	0x0CFFF448
#define CAN_DATA_PE6	0x0CFFF548
#define CAN_DATA_PE7	0x0CFFF648

#define CAN_DATA_DLG1

#define CAN_DATA_STW1

#define CAN_DATA_TCS1
// 1[TCS_status][
#define CAN_DATA_TCS2
#define CAN_DATA_TCS3
#define CAN_DATA_TCS4

#define CAN_DATA_WRL1

#define CAN_DATA_IMU1

#define CAN_DATA_STS1




#endif /* IMCP_H_ */
