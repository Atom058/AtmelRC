#ifndef HEADER_FILE
#define HEADER_FILE

	//Includes
	#include <avr/io.h>
	#include <avr/interrupt.h>

	//Function delcarations
	void setup(void);

	//Cosntants
	#define PWMLOWLEVEL (uint16_t) 248 //The minimum value of the PARSED PWM signal 
	#define PWMHIGHLEVEL (uint16_t) 501 //The maximum value of the PARSED PWM signal
	#define TIMERESOLUTION (uint8_t) 4 //4µs per tic
	// #define CONVERSIONFACTOR (float) (255/(PWMHIGHLEVEL-PWMLOWLEVEL))

#endif