#ifndef HEADER_FILE
#define HEADER_FILE

	//Includes
	#include <avr/io.h>
	#include <avr/interrupt.h>

	//Function delcarations
	void setup(void);

	//Cosntants
	#define PWMLOWLEVEL (uint16_t) 1000 //The maximum value of the PWM signal
	#define PWMHIGHLEVEL (uint16_t) 2000 //The minimum value of the PWM signal
	#define CONVERSIONFACTOR (float) (255/PWMLOWLEVEL)

#endif