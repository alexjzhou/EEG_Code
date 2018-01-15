/*
 * SPI.h
 *
 * Created: 4/4/2017 7:41:13 PM
 *  Author: Daniel
 */ 


#ifndef SPI_H_
#define SPI_H_

void init_SPI() {
/*
SPI2X SPR01 SPR00 SCK Frequency
0 0 0 fosc/4
0 0 1 fosc/16
0 1 0 fosc/64
0 1 1 fosc/128
1 0 0 fosc/2
1 0 1 fosc/8
1 1 0 fosc/32
1 1 1 fosc/64
*/

	// Set MOSI and SCK as output, MISO as input
	SPI_DDR |= (1<<MOSI)|(1<<SCK)|(1<<CS_N);
	//Enable SPI by SPE=1, set uC as Master
	//Sets Data order, 0 for MSB transmitted first
	//Other bits of SPCR set clock config
	//CPOL =1 sets the clock output to high when idle
	//CPHA = 1 sample on trailing edge of clock. This is dependent on CPOL
	//SPR1 and 0 affect clock freq. 
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<DORD)|(0<<CPOL)|(1<<CPHA)|(0<<SPR1)|(0<<SPR0);
	//Set clock rate, fck/2 (maximum)
	//More clock rates can be made using last 2 bits in SPCR
}

void SPI_Write(unsigned char dataout) {
	unsigned char dump;
  
  // Start transmission (MOSI)
  	SPDR = dataout;
  // Wait for trans to complete
  	while(!(SPSR & (1<<SPIF))) {
 // 		USART_Transmit(0x11);
  	}
	dump = SPDR;
}

unsigned char SPI_Read() {
	unsigned char word;
	//Start receiving (MISO)
	SPDR = 0x00;

	while(!(SPSR & (1<<SPIF))) {
//		USART_Transmit(0x22);
	}
//	_delay_us(10); //Not sure if this helps any
	word = SPDR;
	return word;

}




#endif /* SPI_H_ */