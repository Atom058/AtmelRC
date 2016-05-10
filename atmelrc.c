#include <atmelrc.h>

uint8_t pb3_state = 0;
uint8_t pb4_state = 0;

int main (void) {

	setup();

	while(1){
		//This loop is continuously running

	}

}

void setup(void){

	cli(); //Disable interrupts temoprarily

	//Setup of the two input pins, adding interrupts
	
		//Interrupts
		GIMSK |= (1<<PCIE);
		PCMSK = (1<<PCINT3) | (1<<PCINT4);

		//Output pins - resetting everything!
		PORTB = 0;
		DDRB = 0;
		//Enable pullup on pin7/PB2
		PORTB |= (1<<PB2);


	//Setup of output pins
		DDRB |= (1<<DDB0) | (1<<DDB1);

	//Setup of PWM modes

	sei(); //Enable interrupts again

}

//Interrupt vectors
ISR ( PCINT0_vect ) {
	
	//To test: Toggle the corresponding ports of input/output

	if( (PINB>>PINB3) & ~(pb3_state) ){
		//IF PB3 has changed

		//record new state of pb3
		pb3_state = (PINB>>PINB3) & 1;

		//Toggle pin 0
		PORTB |= (1<<PB0);

	} else if ( (PINB>>PINB4) & ~(pb4_state) ){
		//IF PB4 has changed

		//record new state of pb3
		pb4_state = (PINB>>PINB4) & 1;

		//Toggle pin 0
		PORTB |= (1<<PB1);

	}

}