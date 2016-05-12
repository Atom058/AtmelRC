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
	#define TIMERESOLUTION (uint8_t) 4 //µs per tic. A low value will not improve accuracy due to overheads.
	#define INTERNALOSCILLATORCALIBRATION 0b01011000
	#define PWMDEADZONE (uint8_t) 15

	//Memory addresses of stored calibration. Randomly spaced #YOLO
	#define AMAX (uint16_t*) 0x00
	#define AMIN (uint16_t*) 0x04
	#define BMAX (uint16_t*) 0x08
	#define BMIN (uint16_t*) 0x0a

#endif