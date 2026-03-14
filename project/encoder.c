#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "project.h"

#define MASK ((1<<PC5) | (1<<PC4)) //start and stop sensor

ISR(PCINT2_vect) //encoder
{
	// Read the input bits and determine A and B.
	char x = PIND;
	a = ((x & 1<<PD2)==(1<<PD2));
	b = ((x & 1<<PD3)==(1<<PD3));

	if (old_state == 0) {
		// Handle A and B inputs for state 0
		if(a){
			new_state=1;
			count++;
		}
		if(b){
			new_state=3;
			count--;
		}
	}
	else if (old_state == 1) {
		// Handle A and B inputs for state 1
		if(!a){
			new_state=0;
			count--;
		}
		if(b){
			new_state=2;
			count++;
		}
	}
	else if (old_state == 2) {
		// Handle A and B inputs for state 2
		if(!a){
			new_state=3;
			count++;
		}
		if(!b){
			new_state=1;
			count--;
		}
	}
	else {   // old_state = 3
		// Handle A and B inputs for state 3
		if(a){
			new_state=2;
			count--;
		}
		if(!b){
			new_state=0;
			count++;
		}
	}

	// Limit range
	if(count<1){
		count=1;
	} 
	if(count>99){
		count=99;
	}

	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
	if (new_state != old_state) {
		changed = 1;
		old_state = new_state;
	}
}


ISR(PCINT1_vect) //photosensors
{	
	if(!start && ((PINC & MASK) == (1<<PC5))){ // only start activated
		TCCR1B |= (1<<CS10) | (1<<CS12); //start timer, prescalar=1024
		TCNT1 = 0;
		start = 1;
	}

	if(start && ((PINC & MASK) == (1<<PC4))){ // only stop activated
		TCCR1B &= ~((1<<CS10) | (1<<CS12)); //stop timer
		time = TCNT1;
		stop = 1;
	}
}




