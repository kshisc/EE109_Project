#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "timers.h"
#include "project.h"

void timer1_init() //sensors
{
	TCCR1B |= (1<<WGM12); //CTC
    TIMSK1 |= (1<<OCIE1A); //enable timer interrupt
	OCR1A = 62500; //modulus (4 sec)
}

ISR(TIMER1_COMPA_vect)
{
	TCCR1B &= ~((1<<CS10) | (1<<CS12)); //stop timer
	expired=1;
}


void timer0_init() //buzzer
{
	TCCR0B |= (1<<WGM02); //CTC
    TIMSK0 |= (1<<OCIE0A); //enable timer interrupt
}

ISR(TIMER0_COMPA_vect)
{
	PORTC ^= (1<<PC1); //toggle
	cycles++;

	if(cycles == max){
		TCCR0B &= ~(1<<CS02); //stop timer
		cycles=0;
	}
}

void play_note()
{	
	max = 262; //half a second, freq=262
	OCR0A = 119; //delay
	TCCR0B |= (1<<CS02); //start timer, prescalar=256
}


void timer2_init() //servo
{
	TCCR2A |= (1<<WGM21) | (1<<WGM20); // Fast PWM mode, modulus=256
	TCCR2A |=  (1<<COM2A1); // Turn on D11 at 0x00 and off at OCR2A
	TCCR2B |= (1<<CS20) | (1<<CS21) | (1<<CS22); //prescalar=1024
	
	//count = time x (16Mhz/prescalar)
	OCR2A=24; //1.5 sec
}


