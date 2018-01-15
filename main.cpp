/*
 * EEG_ADS1299.cpp
 *
 * Created: 4/4/2017 8:02:59 PM
 * Author : Daniel
 */ 

#define F_CPU			8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "DefinitionsEEG.h"
#include "Definitions_ADS1299.h"

#include "USART.h"
#include "SPI.h"
//#include "I2C.h"

//Functions
void init_Timer();
void init_Board();
void checkRx();
void getDataOverSPI();
void configEEG();
void toggleLED();

//Variable
volatile unsigned char stream;
//========= For handling Rx USART words=========
volatile unsigned char Rx_Word;
volatile unsigned char Rx_Buffer[BUFFER_SIZE];
volatile unsigned char Rx_BufferWriteIndex;
volatile unsigned char Rx_BufferReadIndex;
volatile unsigned char Rx_BufferUnread;

//========= For handling Tx USART words=========
volatile unsigned char Tx_Buffer[BUFFER_SIZE];
volatile unsigned char Tx_BufferWriteIndex;
volatile unsigned char Tx_BufferReadIndex;
volatile unsigned char Tx_BufferUnread;

//Configure buffer
volatile unsigned char configBuffer[BUFFER_SIZE];

int main(void)
{
    init_Board();
	init_USART(MYUBRR);
	init_SPI();
	sei(); //Turns on interrupts
    while(1) {
        //if (Rx_Word != 0x00)
		//_delay_ms(100);
		//toggleLED();
		if (Rx_BufferUnread > 0)
			checkRx();
		if (Tx_BufferUnread > 0) {			
			USART_Transmit(Tx_Buffer[Tx_BufferReadIndex]);
			Tx_BufferUnread -= 1;
			Tx_BufferReadIndex = (Tx_BufferReadIndex+1);
		}
    }
}

void init_Board() {
	Rx_Word = 0x00;
	Rx_BufferReadIndex   = 0;
	Rx_BufferWriteIndex  = 0;
	Rx_BufferUnread      = 0;

	Tx_BufferReadIndex   = 0;
	Tx_BufferWriteIndex  = 0;
	Tx_BufferUnread      = 0;

	stream = 0;

	//Configure PORT and DDR
	LED_DDR |= (1<<LED_PIN);
	LED_PORT |= (1<<LED_PIN);

	ADS_CTRL_DDR |= (1<<ADS_CTRL_RESET_N)|(1<<ADS_CTRL_START); //outputs
	ADS_CTRL_DDR &= ~(1<<ADS_CTRL_DRDY_N); //DRDYn as input for trigger
	ADS_CTRL_PORT&= ~((1<<ADS_CTRL_RESET_N)|(1<<ADS_CTRL_START)); //reset and don't start
	_delay_us(100);
	ADS_CTRL_PORT|= (1<<ADS_CTRL_RESET_N);

	//Setup Int0 interrupt for nDRDY
	EICRA = (1<<ISC01)|(0<<ISC00); //Interrupt on falling edge

}

void checkRx() {
	switch (Rx_Buffer[Rx_BufferReadIndex]) {
		case (GET_ID):
			USART_Transmit(DEVICE_ID);
			Rx_BufferUnread -= 1;
			Rx_BufferReadIndex = (Rx_BufferReadIndex+1);
			Rx_Buffer[Rx_BufferReadIndex] = 0x00;
			break;
		case (CONFIG):
			Rx_BufferUnread -= 1;
			Rx_BufferReadIndex = (Rx_BufferReadIndex+1);
			while (Rx_BufferUnread < 23)
				;		
			for (unsigned char count=0;count<23;count++) {
				configBuffer[count] = Rx_Buffer[Rx_BufferReadIndex];
				Rx_BufferReadIndex = (Rx_BufferReadIndex+1);
				Rx_BufferUnread -= 1;
			}
			configEEG();
			Rx_Buffer[Rx_BufferReadIndex] = 0x00;
			break;
		case (TOGGLE_LED):
			toggleLED();
			Rx_BufferUnread -= 1;
			Rx_BufferReadIndex = (Rx_BufferReadIndex+1);
			Rx_Buffer[Rx_BufferReadIndex] = 0x00;
			break;
		case (START_STREAM):
			stream = 1;
			EIMSK = (1<<INT0); //Enables interrupt
			ADS_CTRL_PORT|= (1<<ADS_CTRL_START);

			Rx_BufferUnread -= 1;
			Rx_BufferReadIndex = (Rx_BufferReadIndex+1);
			Rx_Buffer[Rx_BufferReadIndex] = 0x00;
			break;
		case (STOP_STREAM):
			stream = 0;
			EIMSK = (0<<INT0); //Enables interrupt
			ADS_CTRL_PORT&= ~(1<<ADS_CTRL_START);

			Rx_BufferUnread -= 1;
			Rx_BufferReadIndex = (Rx_BufferReadIndex+1);
			Rx_Buffer[Rx_BufferReadIndex] = 0x00;
			break;
		default:
			Rx_BufferUnread -= 1;
			Rx_BufferReadIndex = (Rx_BufferReadIndex+1);
			Rx_Buffer[Rx_BufferReadIndex] = 0x00;
			break;
	}
}

void toggleLED() {
	if (LED_PORT&(1<<LED_PIN))
		LED_PORT &= ~(1<<LED_PIN);
	else
		LED_PORT |= (1<<LED_PIN);
}
void configEEG() {
	unsigned char startingReg = 0x01;
	unsigned char numReg = 23;

	SPI_PORT &= ~(1<<CS_N);
	SPI_Write(SDATAC);

	SPI_Write(WREG|startingReg);
	SPI_Write(numReg-1);

	for (unsigned char count=0; count<numReg;count++) {
		SPI_Write(configBuffer[count]);
	}
	SPI_Write(RDATAC);
	_delay_us(10);
	SPI_PORT |= (1<<CS_N);
}
void getDataOverSPI() {
	//Tx_Buffer[Tx_BufferWriteIndex] = START_PACKET;
	//Tx_BufferWriteIndex = (Tx_BufferWriteIndex+1);
	//Tx_BufferUnread += 1;

	SPI_PORT &= ~(1<<CS_N);
	for (unsigned char count=0; count<27;count++) {
		Tx_Buffer[Tx_BufferWriteIndex] = SPI_Read();
		Tx_BufferWriteIndex = (Tx_BufferWriteIndex+1);
		Tx_BufferUnread += 1;
	}
	_delay_us(10);
	SPI_PORT |= (1<<CS_N);
	//Tx_Buffer[Tx_BufferWriteIndex] = END_PACKET;
	//Tx_BufferWriteIndex = (Tx_BufferWriteIndex+1);
	//Tx_BufferUnread += 1;
}

//Interrupts
ISR(USART_RX_vect) {
	Rx_Word = USART_Receive();
	Rx_Buffer[Rx_BufferWriteIndex] = Rx_Word;
	Rx_BufferWriteIndex = (Rx_BufferWriteIndex+1);
	Rx_BufferUnread += 1;
}

ISR(INT0_vect) { //nDRDY falling edge
	if (stream == 1)
		getDataOverSPI();
}