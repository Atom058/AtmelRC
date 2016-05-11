#include <atmelrc.h>

volatile uint8_t signalA = 0;
volatile uint8_t signalB = 0;

volatile uint16_t counterA = 0;
volatile uint16_t counterB = 0;

int16_t inputA_value = 0;
int16_t inputB_value = 0;

uint16_t PWM_outA = 0;
uint16_t PWM_outB = 0;

int main (void) {

	setup();

	while(1){

		// input value parsing
			if( counterA && !signalA ){
				//If the counter has measured a time, and the signal is no longer present
				inputA_value = counterA;
				counterA = 0;
			}

			if( counterB && !signalB ){
				//If the counter has measured a time, and the signal is no longer present
				inputB_value = counterB;
				counterB = 0;
			}


		//Calculates the values of the PWM 8-bit output signals. 
		// 	The inputB_value is matched almost 1-1 for 8bits.
			if(inputA_value >= PWMLOWLEVEL){
				
				PWM_outA = inputA_value - PWMLOWLEVEL;

			} else {

				PWM_outA = 0;

			}

			if(inputB_value >= PWMLOWLEVEL){

				PWM_outB = inputB_value - PWMLOWLEVEL;

			} else {

				PWM_outB = 0;

			}


		// Set PWM output for channels A & B
			if( PWM_outA >= 240) { //Channel A

				OCR0A = 255;
				// PORTB &= ~(_BV(PB2));

			} else if ( PWM_outA <= 15 ){

				OCR0A = 0;
				// PORTB &= ~(_BV(PB2));

			} else {

				OCR0A = (uint8_t) PWM_outA;
				// PORTB |= _BV(PB2);

			}

			if( PWM_outB >= 240) { //Channel B

				OCR0B = 255;

			} else if ( PWM_outB <= 15 ){

				OCR0B = 0;

			} else {

				OCR0B = (uint8_t) PWM_outB;

			}

	}

}

void setup(void){

	cli(); //Disable interrupts temoprarily


	//Internal Crystal calibration
		OSCCAL = INTERNALOSCILLATORCALIBRATION;

	//Use pin2 as debug
		DDRB |= _BV(DDB2);

	//Setup of the two input pins, adding interrupts
		GIMSK |= _BV(PCIE);
		PCMSK = _BV(PCINT3) | _BV(PCINT4);

		
	//Setup of Timer0 for PWM modes
		TCCR0B |= _BV(CS01); //Set clock to CLK/8, activating PWM
		TCCR0A |= _BV(WGM00); //Set Phase-correct PWM mode
		OCR0A = PWM_outA; //Set the inital compare levels for outA
		OCR0B = PWM_outB; //Set the inital compare levels for outB
		DDRB |= _BV(DDB0) | _BV(DDB1); // Set pins as output
		TCCR0A |= _BV(COM0A1) | _BV(COM0B1); //Set pins as PWM outputs


	//Setup of Timer1 for time-keeping
		PLLCSR |= _BV(PLLE);
		while( (PLLCSR & _BV(PLOCK)) == 0 ); //Wait for PLL to lock in
		PLLCSR |= _BV(PCKE);
		TCCR1 |= _BV(CS12); //Set clock to PCK/8, giving a resolution of 8 counts per µs
		TCCR1 |= _BV(CTC1) | _BV(PWM1A); //Reset timer after OCR1C match, and activate "PWM" mode
		OCR1C = 30; //Number of counts for 4 µs
		TIMSK |= _BV(TOIE1);//Enable overflow interrupt


	sei(); //Enable interrupts again

}


ISR( TIMER1_OVF_vect ){

	//Function to calculate µs of the signal
	if(signalA) {
		counterA = counterA + 1; //Add another tic to the counter. 
	} else if(signalB){
		counterB = counterB + 1; //Add another tic to the counter. 
	}

}


ISR( PCINT0_vect ) {

	//Sets signal values
	signalA = (PINB>>PINB3) & 1;
	signalB = (PINB>>PINB4) & 1;

}


