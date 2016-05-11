#include <atmelrc.h>

uint8_t pb3_state = 0;
uint8_t pb4_state = 0;

uint16_t counterA = 0;
uint16_t counterB = 0;

uint8_t PWM_outA = 127;
uint8_t PWM_outB = 0;

int main (void) {

	setup();

	while(1){

		//This loop is continuously running
	
	}

}

void setup(void){

	cli(); //Disable interrupts temoprarily

	//Internal Crystal calibration
	OSCCAL = 0b01011000;

	//Setup of the two input pins, adding interrupts
	
		//Interrupts
		// GIMSK |= _BV(PCIE);
		// PCMSK = _BV(PCINT3) | _BV(PCINT4);

		//Set pin7/PB2 as output, with high level
		// DDRB |= _BV(DDB2);
		// PORTB |= _BV(PB2);


	//Setup of Timer1 for time-keeping

	//Setup of Timer0 for PWM modes
		TCCR0B |= _BV(CS01); //Set clock to CLK/8, activating PWM
		TCCR0A |= _BV(WGM00); //Set Phase-correct PWM mode
		OCR0A = PWM_outA; //Set the inital compare levels for outA
		OCR0B = PWM_outB; //Set the inital compare levels for outB
		DDRB |= _BV(DDB0) | _BV(DDB1); // Set pins as output
		TCCR0A |= _BV(COM0A1) | _BV(COM0B1); //Set pins as PWM outputs
		TIMSK |= _BV(TOIE0); //Enable interrupts
		

	sei(); //Enable interrupts again

}

ISR( TIMER0_OVF_vect ){

	counterA++;
	if(counterA > 50){
		counterA = 0;
		OCR0A = OCR0A + 1;
		OCR0B = OCR0B + 1;
	}

	// PORTB ^= ~(_BV(PB4)); //Toggle PB4

}

ISR( TIMER0_COMPA_vect ) {

	counterA++;

	if(counterA > 10000){
		counterA = 0;
		PWM_outA += 10;
		OCR0A = _BV(PWM_outA);
	}

}

ISR( TIMER0_COMPB_vect ) {

	counterB++;

	if(counterB > 10000){
		counterB = 0;
		PWM_outB += 10;
		OCR0B = _BV(PWM_outB);
	}

}

//Interrupt vectors
ISR( PCINT0_vect ) {
	
	//To test: Toggle the corresponding ports of input/output

	// PORTB |= _BV(PB0) | _BV(PB1);
/*
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

*/

}

