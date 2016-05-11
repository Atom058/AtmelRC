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
		GIMSK |= _BV(PCIE);
		PCMSK = _BV(PCINT3) | _BV(PCINT4);

		//Output pins - resetting everything!
		PORTB = _BV(0);
		DDRB = _BV(0);


		//Set pin7/PB2 as output, with high level
		PORTB |= _BV(PB2);


	//Setup of output pins
		DDRB |= _BV(DDB0) | _BV(DDB1);

	//Setup of PWM modes

	sei(); //Enable interrupts again

}

//Interrupt vectors
ISR ( PCINT0_vect ) {
	
	//To test: Toggle the corresponding ports of input/output

	// PORTB |= _BV(PB0) | _BV(PB1);


	if( (PINB>>PINB3) & ~(pb3_state) ){
		//IF PB3 has changed

		//record new state of pb3
		pb3_state = (PINB>>PINB3) & _BV(1);

		//Toggle pin 0
		PORTB ^= _BV(PB0);

	} else if ( (PINB>>PINB4) & ~(pb4_state) ){
		//IF PB4 has changed

		//record new state of pb4
		pb4_state = (PINB>>PINB4) & _BV(1);

		//Toggle pin 1
		PORTB ^= _BV(PB1);

	}

}
