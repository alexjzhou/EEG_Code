/*
 * USART.h
 *
 * Created: 4/4/2017 7:41:49 PM
 *  Author: Daniel
 */ 


#ifndef USART_H_
#define USART_H_

void init_USART(unsigned int ubrr) {

	//Set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable RX interupt, receiver, and transmitter
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	//Set frame format: 8data, 1stop bit 
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);

}

void USART_Transmit(unsigned char data) {

	// Wait for empty transmit buffer 
	while ( !( UCSR0A & (1<<UDRE0)) )
		;
	// Put data into buffer, sends the data
	UDR0 = data;
}

unsigned char USART_Receive() {

	// Wait for data to be received
//		while ( !(UCSRA & (1<<RXC)) )
		;
	// Get and return received data from buffer
	return UDR0;
}




#endif /* USART_H_ */