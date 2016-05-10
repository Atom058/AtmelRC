# AtmelRC
A project to parse RC PWM signals with an ATTiny85. It is possible to use other Atmel chips as well, but the program was aimed at the capabilities of hte ATTiny85. No warranties ;).

# Fuses and selection of clock sources
The ATTiny will run on the Internal PPL clock as clk(CPU). This will results in a nominal frequency of 16MHz, which is sufficient to be able to read the PWM input.

## Fuse settings
* Lfuse: 0xE1
* Hfuse: 0xDF
* Efuse: 0xFF

# Pins
The inputs from the RC receiver are tied to hardware interrupts on the ATTiny85. This means that the timing pulse will initiate an interrupt vector in software, where the timekeeping will begin and end.

The ATTiny85 has 2 pins that can be used as PWM outputs. These are the output port of the chip when used as the RC parser. 

## Pin configuration
* Pin 1: RESET (not used by program. Tie to Vcc with resistor.)
* Pin 2: PCINT3, interrupt vector and first channel input
* Pin 3: PCINT4, interrupt vector and second channel input
* Pin 4: GND
* Pin 5: OC0A, PWM output 1
* Pin 6: OC0B, PWM output 2
* Pin 7: _Not utilised_
* Pin 8: Vcc
