#ifndef HEADER_FILE
#define HEADER_FILE

	//Includes
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>

	//Function delcarations
	void setup(void);
	void generatePWMout(void);
	void sweepPWMout(uint8_t toggle);
	void calculateConversionFactor(void);

	//Constants
	#define PWMLOWLEVEL (uint16_t) 248 //The minimum value of the PARSED PWM signal 
	#define PWMHIGHLEVEL (uint16_t) 501 //The maximum value of the PARSED PWM signal
	#define TIMERESOLUTION (uint8_t) 4 //4µs per tic
	#define INTERNALOSCILLATORCALIBRATION 0b01011000
	#define PWMDEADZONE (uint8_t) 15

	//Memory locations. Not sure if this is the way to go?
	#define A_HIGH_MEM (uint16_t) 0
	#define A_LOW_MEM (uint16_t) 2
	#define B_HIGH_MEM (uint16_t) 4
	#define B_LOW_MEM (uint16_t) 6

#endif