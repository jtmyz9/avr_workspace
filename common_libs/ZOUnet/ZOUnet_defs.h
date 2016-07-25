/*
 * ZOUnet_defs.h
 *
 *  Created on: Jan 9, 2012
 *      Author: Administrator
 */

#ifndef ZOUNET_DEFS_H_
#define ZOUNET_DEFS_H_

// Can node types
#define CAN_NODETYPE_RGLR	0x00
#define CAN_NODETYPE_MSTR	0x01

// Inter-Module Communication protocol
#define CAN_MSGTYPE_NONE 0
#define CAN_DATABRDCST 1

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

//Standard Message Types
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
#define CAN_DATA_PE1	0xFFF0
#define CAN_DATA_PE2	0xFFF1
#define CAN_DATA_PE3	0xFFF2
#define CAN_DATA_PE4	0xFFF3
#define CAN_DATA_PE5	0xFFF4
#define CAN_DATA_PE6	0xFFF5
#define CAN_DATA_PE7	0xFFF6
#define CAN_DATA_TCS1	0xFFD0
#define CAN_DATA_TCS2	0xFFD1
#define CAN_DATA_TCS3	0xFFD2
#define CAN_DATA_TCS4	0xFFD3
#define CAN_DATA_IMU1	0xFFD4
#define CAN_DATA_IMU2	0xFFD5
#define CAN_DATA_IMU3	0xFFD6
#define CAN_DATA_IMU4	0xFFD7
#define CAN_DATA_STS1	0xFFD8
#define CAN_DATA_STS2	0xFFD9
#define CAN_DATA_STS3	0xFFDA
#define CAN_DATA_STS4	0xFFDB
#define CAN_DATA_STS5	0xFFDC
#define CAN_DATA_DLG1	0xFFDD
#define CAN_DATA_DLG2	0xFFDE
#define CAN_DATA_DLG3	0xFFDF
#define CAN_DATA_DLG4	0xFFE0
#define CAN_DATA_STW1	0xFFE1
#define CAN_DATA_WRL1	0xFFE2

#endif /* IMCP_H_ */
