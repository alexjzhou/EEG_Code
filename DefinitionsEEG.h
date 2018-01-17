/*This is a test comment on the code for GitHub*/
 * DefinitionsEEG.h
 *
 * Created: 4/4/2017 5:46:27 PM
 *  Author: Daniel
 */ 


#ifndef DEFINITIONSEEG_H_
#define DEFINITIONSEEG_H_

#define DEVICE_ID		0x56
#define BUFFER_SIZE		256

#define START_PACKET	0x5A
#define END_PACKET		0xA5

//LED
#define LED_PIN			PC0
#define LED_PORT		PORTC
#define LED_DDR			DDRC

//ADS1299 Control lines
#define ADS_CTRL_DRDY_N		PD0
#define ADS_CTRL_RESET_N	PD6
#define ADS_CTRL_START		PD7
#define ADS_CTRL_PORT		PORTD
#define ADS_CTRL_DDR		DDRD

//SPI
#define SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define MOSI		PB3
#define MISO		PB4
#define SCK			PB5
#define CS_N		PB2

//UART
#define	RX			PD0
#define TX			PD1
#define MYUBRR		0 //500k Baud when not in double speed mode		

//Commands
#define CONFIG			0x01
#define START_STREAM	0x02
#define STOP_STREAM		0x03
#define TOGGLE_LED		0x04
#define GET_ID			0x05



#endif /* DEFINITIONSEEG_H_ */

