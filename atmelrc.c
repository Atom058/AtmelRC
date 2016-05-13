#include <atmelrc.h>

//Tracking input signals
	volatile uint8_t signalA = 0;
	volatile uint8_t signalB = 0;

//Counters for time-keeping
	volatile uint16_t counterA = 0;
	volatile uint16_t counterB = 0;
	volatile uint16_t counterC = CALIBRATIONDEBOUNCE;
	volatile uint16_t counterD = 0;

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

	uint8_t channelMargin = 0;
	uint8_t lowerBoundSet = 0;

	uint8_t calA_done = 0;
	uint8_t calB_done = 0;

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
		if(calibration && counterC >= CALIBRATIONDEBOUNCE) {


			if(!calibrationINIT){
				//Resets the calibration levels
					NEW_inputA_MIN = 0;//inputA_value;
					NEW_inputA_MAX = 0;//inputA_value;
					NEW_inputB_MIN = 0;//inputB_value;
					NEW_inputB_MAX = 0;//inputB_value;

				//Also reset lingering input values. Needed for persistant channel B calibration.
					inputA_value = 0;
					inputB_value = 0;

				calibrationINIT = 1; //Will not enter this section next loop

			} else {
			
				//Update A values
					if(!calA_done){

						DDRB |= _BV(DDB0); //Turn on channel A
						DDRB &= ~(_BV(DDB1)); //Turn off channel B

						if(inputA_value > NEW_inputA_MAX){

							NEW_inputA_MAX = inputA_value;
							NEW_inputA_MIN = inputA_value; //Restore min value
							channelMargin = inputA_value / 10; //set the minimum amount of travel in the band
							OCR0A = UINT8_MAX; //Turn output on
							counterD = 0; //Reset calibration timer
							lowerBoundSet = 0;

						} else if(
									(inputA_value <= NEW_inputA_MIN) && 
									(inputA_value > channelMargin) && //Sets minimum signal level
									((NEW_inputA_MAX - inputA_value) > channelMargin) //Minimum travel distance
								) {

							NEW_inputA_MIN = inputA_value;
							OCR0A = 0; //Turn output off
							lowerBoundSet = 1;
							counterD = 0; //Reset calibration timer

						}

						if(lowerBoundSet) {

							// Generate PWM output
								uint8_t pwm_temp = (uint8_t) 
									(inputA_value - NEW_inputA_MIN) * (UINT8_MAX / (NEW_inputA_MAX - NEW_inputA_MIN));

								if(pwm_temp > (UINT8_MAX - PWMDEADZONE)){

									OCR0A = UINT8_MAX;

								} else if (pwm_temp <= PWMDEADZONE){

									OCR0A = 25; //Not 0, as the calibration done should be visible

								} else {

									OCR0A = pwm_temp;

								}

							if(counterD > (CALIBRATIONDEBOUNCE * 4)){
								
								// Wait until accepting and proceeding to channel B
								calA_done = 1;
								lowerBoundSet = 0;
								save = 1;

							}

						}

					}

				//Update B values
					if(calA_done && !calB_done) {

						DDRB |= _BV(DDB1); //Turn on channel B
						DDRB &= ~(_BV(DDB0)); //Turn off channel A

						if(inputB_value > 20){

							if(inputB_value > NEW_inputB_MAX){

								NEW_inputB_MAX = inputB_value;
								NEW_inputB_MIN = inputB_value; //Restore min value
								channelMargin = inputB_value / 10; //set the minimum amount of travel in the band
								OCR0B = UINT8_MAX; //Turn output on
								counterD = 0; //Reset calibration timer
								lowerBoundSet = 0;

							} else if(
										(inputB_value <= NEW_inputB_MIN) && 
										(inputB_value > channelMargin) && //Sets minimum signal level
										((NEW_inputB_MAX - inputB_value) > channelMargin) //Minimum travel distance
									) {

								NEW_inputB_MIN = inputB_value;
								lowerBoundSet = 1;
								counterD = 0; //Reset calibration timer

							}

							if(lowerBoundSet) {

								// Generate PWM out
									uint8_t pwm_temp = (uint8_t) 
										(inputB_value - NEW_inputB_MIN) * (UINT8_MAX / (NEW_inputB_MAX - NEW_inputB_MIN));

									if(pwm_temp > (UINT8_MAX - PWMDEADZONE)){

										OCR0B = UINT8_MAX;

									} else if (pwm_temp <= PWMDEADZONE){

										OCR0B = 25; //Not 0, as the calibration done should be visible

									} else {

										OCR0B = pwm_temp;

									}

								if(counterD > (CALIBRATIONDEBOUNCE * 4)){
									
									// Wait until accepting and proceeding to channel B
									calB_done = 1;
									lowerBoundSet = 0;
									save = 1;
									DDRB &= ~(_BV(DDB1)); //Turn off channel B

								}

							}

						}

					}

			}


		} else if (save){

			cli(); //Disable interrupts when saving data

			//Save in current session and update memory locations. This might take some time...
				if(calA_done){

					inputA_MAX = NEW_inputA_MAX;
					inputA_MIN = NEW_inputA_MIN;
					eeprom_update_word(AMAX, inputA_MAX);
					eeprom_update_word(AMIN, inputA_MIN);
	
				}

				if(calB_done){

					inputB_MAX = NEW_inputB_MAX;
					inputB_MIN = NEW_inputB_MIN;
					eeprom_update_word(BMAX, inputB_MAX);
					eeprom_update_word(BMIN, inputB_MIN);
					
				}

			//Update conversion factors
				calculateConversionFactor();
			
			//Reset calibration state
				counterC = 0; //Debounce timer
				calibrationINIT = 0;
				calA_done = 0;
				calB_done = 0;
				save = 0;

			//Restore PWM outputs
				DDRB |= _BV(DDB0) | _BV(DDB1);
				inputA_value = 0;
				inputB_value = 0;
				

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
		GIMSK |= _BV(PCIE);
		PCMSK = _BV(PCINT3) | _BV(PCINT4); //Add interrupts for input pins

		
	//Setup of Timer0 for PWM modes
		TCCR0B |= _BV(CS01); //Set clock to CLK/8, activating PWM with a frequency of approximately 4 KHz
		TCCR0A |= _BV(WGM00); //Set Phase-correct PWM mode
		OCR0A = PWM_outA; //Set the inital compare levels for outA
		OCR0B = PWM_outB; //Set the inital compare levels for outB
		DDRB |= _BV(DDB0) | _BV(DDB1); // Set pins as output
		TCCR0A |= _BV(COM0A1) | _BV(COM0B1); //Set pins as PWM outputs
		TIMSK |= _BV(TOIE0); //Enable interrupt on PWM overflow


	//Setup of Timer1 for time-keeping
		PLLCSR |= _BV(PLLE);
		while( (PLLCSR & _BV(PLOCK)) == 0 ); //Wait for PLL to lock in
		PLLCSR |= _BV(PCKE);
		TCCR1 |= _BV(CS12); //Set clock to PCK/8, giving a resolution of 8 counts per µs
		TCCR1 |= _BV(CTC1) | _BV(PWM1A); //Reset timer after OCR1C match, and activate "PWM" mode
		//Setting timer count resolution
			OCR1C = (TIMERESOLUTION < 4) ? 32 : TIMERESOLUTION * 8;
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

	//Conversion A caculation
		if(inputA_MAX > inputA_MIN){
			
			conversionA = UINT8_MAX / (inputA_MAX - inputA_MIN);

		} else {

			conversionA = 0;

		}

	//Conversion B caculation
	if(inputB_MAX > inputB_MIN){
		
		conversionB = UINT8_MAX / (inputB_MAX - inputB_MIN);

	} else {

		conversionB = 0;

	}	

}


ISR( TIMER0_OVF_vect ){

	if(calibration) {

		if(counterD < UINT16_MAX){

			counterD = counterD + 1;

		}

	} else {

		//Counter to debounce the calibration setting
		if(counterC < UINT16_MAX){

			counterC = counterC + 1;

		}

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
	calibration = (PINB>>PINB2) & 1;

}



ISR( INT0_vect ) {

	calibration = (PINB>>PINB2) & 1;

}