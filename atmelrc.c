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


		//Calculates the values of the PWM 8-bit output signals 255 * (value-1000)/1000
		PWM_outA = CONVERSIONFACTOR * ( inputA_value - PWMLOWLEVEL );
		PWM_outB = CONVERSIONFACTOR * ( inputB_value - PWMLOWLEVEL );


		// Set PWM output for channel A
		if( PWM_outA > 255) {

			OCR0A = 255;

		} else if ( PWM_outA < 0 ){

			OCR0A = 0;

		} else {

			OCR0A = (uint8_t) PWM_outA;

		}

		// Set PWM output for channel B
		if( PWM_outB > 255) {

			OCR0B = 255;

		} else if ( PWM_outB < 0 ){

			OCR0B = 0;

		} else {

			OCR0B = (uint8_t) PWM_outB;

		}

	}

}

void setup(void){

	cli(); //Disable interrupts temoprarily


	//Internal Crystal calibration
		OSCCAL = 0b01011000;

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
		TCCR1 |= _BV(CTC1); //Reset timer after OCR1C match
		OCR1C = 8; //Number of counts for 1 µs
		TIMSK |= _BV(TOIE1);//Enable overflow interrupt


	sei(); //Enable interrupts again

}


ISR( TIMER1_OVF_vect ){

	//Function to calculate µs of the signal
	cli();
	if(signalA) {
		counterA++; //Add another 1µs to the counter. 
	} else if(signalB){
		counterB++;
	}
	sei();

}


ISR( PCINT0_vect ) {

	//Sets signal values
	signalA = (PINB>>PINB3) & 1;
	signalB = (PINB>>PINB4) & 1;

}


