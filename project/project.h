/* global variables */
//sensors
extern volatile uint8_t start; //detect start
extern volatile uint8_t stop; //detect stop
extern volatile uint8_t expired; //flag for timing expired

//speed calculation
extern volatile long unsigned int time;
extern const uint16_t dist; //sensors are 5.08 cm apart
extern long int speed;

//encoder
extern volatile uint8_t new_state, old_state;
extern volatile uint8_t changed; //flag for state change
extern volatile uint8_t a, b; //encoder values
extern volatile uint8_t count;

//buzzer
extern volatile uint16_t cycles; //timer cycles
extern volatile uint16_t max; //cycles per half second

//serial
extern volatile uint8_t open; //detect {
extern volatile uint8_t valid; //detect valid char
extern volatile uint8_t dcount; //counts received data
extern volatile char rchar[5]; //recieved data