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

	#define CALIBRATIONDEBOUNCE (uint16_t) (F_CPU/4096) //This should be about 1 second

	// Clock Calibration
		#define INTERNALOSCILLATORCALIBRATION 0b01011000
		#define TIMERESOLUTION (uint8_t) 4 //µs per tic. 4µs is maximum resolution, and the standard setting.

	//Deadzone on both high and low ends. 15 is ~10% of 255.
	#define PWMDEADZONE (uint8_t) 25


	//Memory addresses of stored calibration. "Randomly" spaced #YOLO
	#define AMAX (uint16_t*) 0x00
	#define AMIN (uint16_t*) 0x04
	#define BMAX (uint16_t*) 0x08
	#define BMIN (uint16_t*) 0x0a

#endif