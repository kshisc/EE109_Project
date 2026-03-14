/********************************************
 *
 *  Name: Kyra Shi
 *  Email: kyrashi@usc.edu
 *  Section: Wed 3:30
 *  Assignment: Project
 *
 ********************************************/

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "project.h"
#include "lcd.h"
#include "timers.h"
#include "serial.h"

/* global variables */
//sensors
volatile uint8_t start = 0; //detect start
volatile uint8_t stop = 0; //detect stop
volatile uint8_t expired = 0; //flag for timing expired

//speed calculation
volatile long unsigned int time = 0;
const uint16_t dist = 508; //sensors are 5.08 cm apart
long int speed = 0;

//encoder
volatile uint8_t new_state, old_state;
volatile uint8_t changed = 0; //flag for state change
volatile uint8_t a, b; //encoder values
volatile uint8_t count = 0;

//buzzer
volatile uint16_t cycles = 0; //timer cycles
volatile uint16_t max; //cycles per half second

//serial
volatile uint8_t open = 0; //detect {
volatile uint8_t valid = 0; //flag for valid char
volatile uint8_t dcount = 0; //counts received chars
volatile char rchar[5]; //recieved data


int main(void) {
    // Initialize the LCD
	lcd_init();

	// Set DDR outputs
	DDRB |= (1 << PB3); //servo 
	DDRB |= (1 << PB4); // tri-state enable
	DDRB |= (1 << PB5); // yellow led
	DDRC |= (1 << PC1); // buzzer 
	DDRC |= (1 << PC2); // rgb red
	DDRC |= (1 << PC3); // rgb blue
	DDRD |= (1 << PD1); //serial output
	
	// Set internal resistors (inputs)
	PORTC |= (1 << PC4); //start sensor
	PORTC |= (1 << PC5); //stop sensor
	PORTD |= (1 << PD2) | (1 << PD3); //rotary encoder
	PORTD |= (1 << PD0); // serial input

	// Interrupts
	timer0_init(); //timer interrupts
	timer1_init();
	timer2_init();
	sei(); //global interrupt
	PCICR |= (1<<PCIE1); //pin interrupt C (sensors)
	PCMSK1 |= (1<<PCINT12) | (1<<PCINT13);
	PCICR |= (1<<PCIE2); //pin interrupt D (encoder)
	PCMSK2 |= (1<<PCINT18) | (1<<PCINT19);
	UCSR0B |= (1<<RXCIE0); //serial reciever interrrupt

	serial_init();
	PORTB &= ~(1 << PB4); //enable tri-state

	//led is green
	PORTC |= (1<<PC2); //red off
	PORTC |= (1<<PC3); //blue off

    // Write a spash screen to the LCD
	lcd_writecommand(1);
    lcd_moveto(0,0);
    lcd_stringout("EE109 Project");
    lcd_moveto(1,0);
    lcd_stringout("Kyra Shi");
    _delay_ms(2000);
    lcd_writecommand(1);

	// print speed threshold
	unsigned char save = eeprom_read_byte((void *) 100);
	if(save>0 && save<100){
		count=save;
	}
	else{
		count=50;
	}
	char thresh[3];
	snprintf(thresh, 3, "%2d", count);
	lcd_moveto(0,14);
	lcd_stringout(thresh); 

    // initial state
	int x = PIND;
	a = ((x & 1<<PD2)==(1<<PD2));
	b = ((x & 1<<PD3)==(1<<PD3));

    if (!b && !a)
	old_state = 0;
    else if (!b && a)
	old_state = 1;
    else if (b && !a)
	old_state = 2;
    else
	old_state = 3;

    new_state = old_state;

	
    while (1) { // Loop forever
		if(start){
			PORTB |= (1<<PB5); //turn on yellow led 
		}

		if(expired){ //4 seconds
			start=0;
			PORTB &= ~(1<<PB5); //turn off yellow led 
			lcd_moveto(0,0);
			lcd_stringout("expire");
			expired=0;
		}

		if(start && stop){ //swiped
			PORTB &= ~(1<<PB5); //turn off yellow led 

			//print time
			time = time * 1000 / 15625; //4000ms max
			char t[5];
			snprintf(t, 5, "%4lu", time);
			lcd_moveto(0,0);
			lcd_stringout(t);
			lcd_stringout("ms");
			
			//calculate speed
			speed = dist * 100 / time; //mm per sec
			if(speed<0){ //limit range
				speed=0;
			}
			if(speed>1000){ 
				speed=1000;
			}

			//print speed
			int speed_int = speed/10;
			int speed_fract = speed%10;
			char full[6];
			snprintf(full, 6, "%3d.%1d", speed_int, speed_fract);
			lcd_moveto(1,0);
			lcd_stringout(full);
			lcd_stringout("cm/s");

			//play note?
			if(speed_int>count){
				play_note();
			}

			//servo
			long int pos = (speed_int * -23)/100 + 35; //max=35, min=12
			// char p[3];
			// snprintf(p, 3, "%2ld", pos);
			// lcd_moveto(1,14);
			// lcd_stringout(p);
			OCR2A=pos; // change servo position

			//transmit char
			char tchar[7];
			snprintf(tchar, 7, "{%ld}", speed);
			for(size_t i=0; i<strlen(tchar); i++){
				tx_char(tchar[i]);
			}

			//reset flags
			start=0;
			stop=0;
		}

		if (changed){ // encoder state change
			// print speed threshold
			snprintf(thresh, 3, "%2d", count);
			lcd_moveto(0,14);
			lcd_stringout(thresh);

    		eeprom_update_byte((void *) 100, count); //save
			changed = 0; // reset flag
		}

		
		if(valid){ //recieve char
			int rspeed; //remote speed
			sscanf(rchar, "%d", &rspeed);
			
			//print speed
			int rspeed_int = rspeed/10;
			int rspeed_fract = rspeed%10;
			char rfull[6];
			snprintf(rfull, 6, "%3d.%1d", rspeed_int, rspeed_fract);
			lcd_moveto(1,11);
			lcd_stringout(rfull);

			//compare speed
			if(rspeed>(speed+30)){
				PORTC &= ~(1<<PC2); //red on
				PORTC |= (1<<PC3); //blue off
			}
			else if(rspeed<(speed-30)){
				PORTC &= ~(1<<PC3); //blue on
				PORTC |= (1<<PC2); //red off
			}
			else{ //green
				PORTC |= (1<<PC2); //red off
				PORTC |= (1<<PC3); //blue off
			}

			memset(rchar, 0, sizeof(rchar)); //reset array
			valid=0; //reset flag
		}

    } //end while
}




