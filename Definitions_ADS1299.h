/*
 * Definitions_ADS1299.h
 *
 * Created: 4/4/2017 5:46:11 PM
 *  Author: Daniel
 */ 


#ifndef DEFINITIONS_ADS1299_H_
#define DEFINITIONS_ADS1299_H_

//CHIP ID
#define ADS1299_ID	0b1110

//----------- registers -----------
//Global Settings
#define REG_ID		0x00
#define REG_CONFIG1	0x01
#define REG_CONFIG2	0x02
#define REG_CONFIG3	0x03
#define REG_LOFF	0x04
//Channel Settings
#define REG_CH1SET	0x05
#define REG_CH2SET	0x06
#define REG_CH3SET	0x07
#define REG_CH4SET	0x08
#define REG_CH5SET	0x09
#define REG_CH6SET	0x0A
#define REG_CH7SET	0x0B
#define REG_CH8SET	0x0C
#define REG_BIAS_SENSP	0x0D
#define REG_BIAS_SENSN	0x0E
#define REG_LOFF_SENSP	0x0F
#define REG_LOFF_SENSN	0x10
#define REG_LOFF_FLIP	0x11
//Lead-Off Status
#define REG_LOFF_STATP	0x12
#define REG_LOFF_STATN	0x13
//GPIO and Other
#define REG_GPIO		0x14
#define REG_MISC1		0x15
#define REG_MISC2		0x16
#define REG_CONFIG4		0x17
//---------------------------------

//---------- Commands -------------
//System Commands
#define WAKEUP			0x02
#define STANDBY			0x04
#define RESET			0x06
#define START			0x08
#define STOP			0x0A
//Data read commands
#define RDATAC			0x10
#define SDATAC			0x11
#define RDATA			0x12
//Register read commands
#define RREG			0b00100000
#define WREG			0b01000000


#endif /* DEFINITIONS_ADS1299_H_ */