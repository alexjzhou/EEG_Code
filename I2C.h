/*
 * I2C.h
 *
 * Created: 4/4/2017 7:44:46 PM
 *  Author: Daniel
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <compat/twi.h>

#define I2C_MAX_TRIES 10

#define I2C_START      0
#define I2C_DATA       1
#define I2C_BURST_DATA 2
#define I2C_STOP       3


void I2C_Init() {
  	/* Initial TWI Peripheral */
  	TWSR = 0x00;   // Select Prescaler of 1
  	// SCL frequency = 12e6 / (16 + 2 * 21 * 1) = 207 khz
	//TWBR = 40;
	// SCL frequency = 8e6 / (16 + 2 * 40 * 1) = 83.3 khz
	TWBR = 11;
	// SCL frequency = 3.6e6 / (16 + 2 * 11 * 1) = 95 khz
  	//TWBR = 0x15;   // 21 Decimal
	PORTC = (1<<PC4) | (1<<PC5);
}

unsigned char I2C_Transmit(unsigned char type) {
	switch(type) {
		case I2C_START:    // Send Start Condition
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
			break;
		case I2C_DATA:     // Send Data
			TWCR = (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_BURST_DATA:     // Send Data
			TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
			break;
		case I2C_STOP:     // Send Stop Condition
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
			return 0;
	}

  	// Wait for TWINT flag set in TWCR Register
  	while (!(TWCR & (1 << TWINT))) {}

  	// Return TWI Status Register, mask the prescaler bits (TWPS1,TWPS0)
  	return (TWSR & 0xF8);
}

char I2C_Write(unsigned char SLA, unsigned char RA, unsigned char data) {
  	unsigned char n = 0;
  	unsigned char twi_status;
  	char r_val = -1;

/**/i2c_retry:
	n++;
  	if (n >= I2C_MAX_TRIES) 
  		return r_val;
  	// Transmit Start Condition
  	twi_status = I2C_Transmit(I2C_START);

  	// Check the TWI Status
  	if (twi_status == TW_MT_ARB_LOST) 
		goto i2c_retry;
  	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) 
		goto i2c_quit;

  	// Send slave address (SLA_W)
  	TWDR = SLA | TW_WRITE;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);

  	// Check the TWSR status
  	if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) 
		goto i2c_retry;
  	if (twi_status != TW_MT_SLA_ACK) 
		goto i2c_quit;

  	// Send the Reg Address
  	TWDR = RA;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);

  	// Check the TWSR status
  	if (twi_status != TW_MT_DATA_ACK) 
		goto i2c_quit;

  	// Put data into data register and start transmission
  	TWDR = data;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);

  	// Check the TWSR status
  	if (twi_status != TW_MT_DATA_ACK) 
		goto i2c_quit;

  	// TWI Transmit Ok
  	r_val=1;

/**/i2c_quit:
  	// Transmit I2C Data
  	twi_status = I2C_Transmit(I2C_STOP);
  	return r_val;
}

char I2C_Read(unsigned char SLA, unsigned char RA, volatile unsigned char *data)
{
  	unsigned char n = 0;
  	unsigned char twi_status;
  	char r_val = -1;

/**/i2c_retry:
  	if (n++ >= I2C_MAX_TRIES) 
		return r_val;

  	// Transmit Start Condition
  	twi_status=I2C_Transmit(I2C_START);

  	// Check the TWSR status
  	if (twi_status == TW_MT_ARB_LOST) 
		goto i2c_retry;
  	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) 
		goto i2c_quit;
	
  	// Send slave address (SLA_W)
  	TWDR = SLA| TW_WRITE;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);
	
	//-----Not sure what this was doing here----------
	//USART_Transmit(twi_status);
	//------------------------------------------------
	
  	// Check the TWSR status
  	if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) 
		goto i2c_retry;
  	if (twi_status != TW_MT_SLA_ACK) 
		goto i2c_quit;
  	// Send the Low 8-bit of I2C Address
  	TWDR = RA;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);

  	// Check the TWSR status
  	if (twi_status != TW_MT_DATA_ACK) 
		goto i2c_quit;
  	// Send start Condition
  	twi_status=I2C_Transmit(I2C_START);

  	// Check the TWSR status
  	if (twi_status == TW_MT_ARB_LOST) 
		goto i2c_retry;
  	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) 
		goto i2c_quit;
  	// Send slave address (SLA_R)
  	TWDR = SLA | TW_READ;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);  

  	// Check the TWSR status
  	if ((twi_status == TW_MR_SLA_NACK) || (twi_status == TW_MR_ARB_LOST)) 
		goto i2c_retry;
  	if (twi_status != TW_MR_SLA_ACK) 
		goto i2c_quit;
  	// Read I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);
  	if (twi_status != TW_MR_DATA_NACK) 
		goto i2c_quit;
  	// Get the Data
  	*data=TWDR;
  	r_val=1;

/**/i2c_quit:
  	// Send Stop Condition
  	twi_status=I2C_Transmit(I2C_STOP);
  	return r_val;
}

char I2C_Burst_Read(unsigned char SLA, unsigned char RA, volatile unsigned char *data, unsigned char numBytes)
{
  	unsigned char n = 0;
  	unsigned char twi_status;
  	char r_val = -1;

/**/i2c_retry:
  	if (n++ >= I2C_MAX_TRIES) 
		return r_val;

  	// Transmit Start Condition
  	twi_status=I2C_Transmit(I2C_START);

  	// Check the TWSR status
  	if (twi_status == TW_MT_ARB_LOST) 
		goto i2c_retry;
  	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) 
		goto i2c_quit;

  	// Send slave address (SLA_W)
  	TWDR = SLA| TW_WRITE;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);

  	// Check the TWSR status
  	if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MT_ARB_LOST)) 
		goto i2c_retry;
  	if (twi_status != TW_MT_SLA_ACK) 
		goto i2c_quit;

  	// Send the Low 8-bit of I2C Address
  	TWDR = RA;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);

  	// Check the TWSR status
  	if (twi_status != TW_MT_DATA_ACK) 
		goto i2c_quit;

  	// Send start Condition
  	twi_status=I2C_Transmit(I2C_START);

  	// Check the TWSR status
  	if (twi_status == TW_MT_ARB_LOST) 
		goto i2c_retry;
  	if ((twi_status != TW_START) && (twi_status != TW_REP_START)) 
		goto i2c_quit;

  	// Send slave address (SLA_R)
  	TWDR = SLA | TW_READ;

  	// Transmit I2C Data
  	twi_status=I2C_Transmit(I2C_DATA);  

  	// Check the TWSR status
  	if ((twi_status == TW_MR_SLA_NACK) || (twi_status == TW_MR_ARB_LOST)) 
		goto i2c_retry;
  	if (twi_status != TW_MR_SLA_ACK) 
		goto i2c_quit;

  	// Read I2C Burst Data
	for (unsigned char i=0; i<(numBytes-1); i++) {
  		twi_status=I2C_Transmit(I2C_BURST_DATA);
  		if (twi_status != TW_MR_DATA_ACK) 
			goto i2c_quit;
		// Get the Data
		data[i]=TWDR;
	}
	twi_status=I2C_Transmit(I2C_DATA);
  	if (twi_status != TW_MR_DATA_NACK) 
		goto i2c_quit;
	// Get the Data
	data[numBytes-1]=TWDR;
 

  	r_val=1;

/**/i2c_quit:
  	// Send Stop Condition
  	twi_status=I2C_Transmit(I2C_STOP);
  	return r_val;
}
char I2C_Write_Bit (unsigned char SLA, unsigned char RA, unsigned char data, unsigned char bitStart, unsigned char length) {
	unsigned char b;
	
	if (I2C_Read(SLA,RA,&b) == 1) {
//		USART_Transmit(b);
		unsigned char mask = (0xFF << (bitStart + length)) | (0xFF >> (8 - bitStart));
		data = (data << bitStart) & ~mask;
        b &= mask;
        b |= data;
//		USART_Transmit(b);
		return I2C_Write(SLA,RA,b);
	}
	else
		return 0;
};




#endif /* I2C_H_ */