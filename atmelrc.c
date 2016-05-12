#include <atmelrc.h>

//Tracking input signals
	volatile uint8_t signalA = 0;
	volatile uint8_t signalB = 0;

//Counters for time-keeping
	volatile uint16_t counterA = 0;
	volatile uint16_t counterB = 0;

//Input values
	uint16_t inputA_value = 0;
	uint16_t inputB_value = 0;

//Calibration variables
	volatile uint8_t calibration = 0;
	uint8_t save = 0;
	uint8_t calibrationINIT = 0;

	uint16_t inputA_MAX = 0; //Is read from memory at startup
	uint16_t inputA_MIN = 0; //Is read from memory at startup
	uint16_t inputB_MAX = 0; //Is read from memory at startup
	uint16_t inputB_MIN = 0; //Is read from memory at startup

	uint16_t NEW_inputA_MAX = 0; //Temp save for new values
	uint16_t NEW_inputA_MIN = 0; //Temp save for new values
	uint16_t NEW_inputB_MAX = 0; //Temp save for new values
	uint16_t NEW_inputB_MIN = 0; //Temp save for new values

	float conversionA = 0; //Conversion factor from input > PWM out
	float conversionB = 0; //Conversion factor from input > PWM out

//PWM Outputs
	uint8_t PWM_outA = 0;
	uint8_t PWM_outB = 0;



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


		//Device enters calibration when PB2 is high
		if(calibration) {


			if(!calibrationINIT){
				//Resets the calibration levels
				NEW_inputA_MIN = inputA_value;
				NEW_inputA_MAX = inputA_value;
				NEW_inputB_MIN = inputB_value;
				NEW_inputB_MAX = inputB_value;

				calibrationINIT = 1; //Will not enter this section next loop
				
				sweepPWMout(1); //Turn on visual indicator of calibration

			} else {
			
				//Update A values
					if(inputA_value > NEW_inputA_MAX){

						NEW_inputA_MAX = inputA_value;
						save = 1;

					} else if((inputA_value < NEW_inputA_MIN) && (inputA_value != 0)){

						NEW_inputB_MIN = inputA_value;
						save = 1;

					}

				//Update B values
					if(inputB_value > NEW_inputB_MAX){

						NEW_inputB_MAX = inputB_value;
						save = 1;

					} else if((inputB_value < NEW_inputB_MIN) && (inputB_value != 0)){

						NEW_inputB_MIN = inputB_value;
						save = 1;

					}

			}


		} else if (save){

			cli(); //Disable interrupts when saving data

			//Save in current session
				inputA_MAX = NEW_inputA_MAX;
				inputA_MIN = NEW_inputA_MIN;
				inputB_MAX = NEW_inputB_MAX;
				inputB_MIN = NEW_inputB_MIN;

			//Update memory locations. This might take some time...
				eeprom_update_word(AMAX, inputA_MAX);
				eeprom_update_word(AMIN, inputA_MIN);
				eeprom_update_word(BMAX, inputB_MAX);
				eeprom_update_word(BMIN, inputB_MIN);

			//Update conversion factors
				calculateConversionFactor();
			
			//Reset calibration state
				save = 0;
				sweepPWMout(0); //Turn of PWM sweeping

			sei(); //Enable interrupts again

		} else {

			//Generate the standard output
			generatePWMout();

		}

	}

}

void setup(void){

	cli(); //Disable interrupts temoprarily


	//Internal Crystal calibration
		OSCCAL = INTERNALOSCILLATORCALIBRATION;

	//Load LOW/HIGH input calibration from EEPROM
		inputA_MAX = eeprom_read_word(AMAX);
		inputA_MIN = eeprom_read_word(AMIN);
		inputB_MAX = eeprom_read_word(BMAX);
		inputB_MIN = eeprom_read_word(BMIN);

		calculateConversionFactor(); //Calculates the PWM conversion factors

	//Setup of the input pins, adding interrupts
		GIMSK |= _BV(PCIE) | _BV(INT0);
		PCMSK = _BV(PCINT3) | _BV(PCINT4); //Add interrupts for input pins
		MCUCR |= _BV(ISC00); //Add interrupt on any change of calibration pin

		
	//Setup of Timer0 for PWM modes
		TCCR0B |= _BV(CS01); //Set clock to CLK/8, activating PWM with a frequency of approximately 4 KHz
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
		//Setting timer count resolution
			OCR1C = 50;//TIMERESOLUTION < 4 ? 32 : TIMERESOLUTION * 8;
		TIMSK |= _BV(TOIE1);//Enable overflow interrupt


	sei(); //Enable interrupts again

}



void generatePWMout(void) {

	//Calculates the values of the PWM 8-bit output signals. 

		//Calculate PWM A
			if(inputA_value >= inputA_MIN){

				if(inputA_value > inputA_MAX){

					PWM_outA = UINT8_MAX;

				} else {

					PWM_outA = (uint8_t) (inputA_value - inputA_MIN) * conversionA;

				}

			} else {

				PWM_outA = 0;

			}


		//Calculate PWM B
			if(inputB_value >= inputB_MIN){

				if(inputB_value > inputB_MAX){

					PWM_outB = UINT8_MAX;

				} else {

					PWM_outB = (uint8_t) (inputB_value - inputB_MIN) * conversionB;

				}

			} else {

				PWM_outB = 0;

			}


	// Set PWM output for channels A & B

		//Channel A
			if( PWM_outA >= (UINT8_MAX - PWMDEADZONE)) {

				OCR0A = UINT8_MAX;

			} else if ( PWM_outA <= PWMDEADZONE ){

				OCR0A = 0;

			} else {

				OCR0A = PWM_outA;

			}

		//Channel B
			if( PWM_outB >= (UINT8_MAX - PWMDEADZONE)) {

				OCR0B = UINT8_MAX;

			} else if ( PWM_outB <= PWMDEADZONE ){

				OCR0B = 0;

			} else {

				OCR0B = PWM_outB;

			}

}



void calculateConversionFactor(void){

	conversionA = 255 * (inputA_MAX - inputA_MIN);
	conversionB = 255 * (inputB_MAX - inputB_MIN);

}



void sweepPWMout(uint8_t toggle){

	if(toggle) {

		TIMSK |= _BV(TOIE0); //Enable interrupt on PWM overflow
		TCCR0B |= _BV(CS00); //Slow down of PWM output to ~500Hz
		OCR0A = 0;
		OCR0B = 127;

	} else {

		TIMSK &= ~(_BV(TOIE0)); //Disable interrupt on PWM overflow
		TCCR0B &= ~(_BV(CS00)); //Restore PWM output to ~4KHz

	}

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



ISR( INT0_vect ) {

	calibration = (PINB>>PINB2) & 1;

}



ISR( TIMER0_OVF_vect ){

	//Sweep the PWM signals
	OCR0A = OCR0A + 1;
	OCR0B = OCR0B + 1;

}