#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "serial.h"
#include "project.h"

#define FOSC 16000000 // Clock frequency
#define BAUD 9600 // Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0

void serial_init(){
	UBRR0 = MYUBRR;
	UCSR0B |= (1 << TXEN0 | 1 << RXEN0); // Enable RX and TX
	UCSR0C = (3 << UCSZ00); // Async., no parity, 1 stop bit, 8 data bits
}

void tx_char(char ch)
{
	// Wait for transmitter data register empty
	while ((UCSR0A & (1<<UDRE0)) == 0) {}
	UDR0 = ch;
}

ISR(USART_RX_vect)
{
	char x = UDR0;

	if(x=='{'){ //start new
		open=1;
		dcount=0;
	}

	else if(open && dcount>0 && (x=='}')){ //done
		open=0;
		valid=1;
	} 

	else if(open && dcount<4){
		if(x<'0' || x>'9'){ //invalid char
			open=0;
		}
		else{
			rchar[dcount]=x;
			dcount++;
		}
	}

	else{ //invalid
		open=0;
		dcount=0;
	} 
}