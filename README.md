# AtmelRC
A project to parse RC PWM signals with an ATTiny85. It might be possible to use other Atmel chips as well, but the program was aimed at the capabilities of the ATTiny85. There are probably some registries which needs to be updated in order for it to work with other devices. No warranties ;).

The program will initially not aim to reduce power consumption. The assumption is that the other RC equipment will consume significantly more power, making the contribution from the ATTiny insignificant in comparison.

# Installation/How to use
This will be a brief guide of how to configure everything to work!

## Getting the tools
The toolchain used by the author was provided through [WIN-AVR][Win-AVR link]. This toolchain is compatible with Windows, but the "originals" are living in UNIX.

The files are prepared so that a chip can be programmed with [avrdude](http://www.nongnu.org/avrdude/) or similar (included in Win-AVR package).

Finally, a programmer is needed. The simplest form is probably an arduino, or a [USBASP][USBASP link]. The list of avrdude's supported programmers [can be found here][avrdude syntax documentation]


## Fuses and selection of clock sources
The ATTiny will run on the Internal PPL clock as clk(CPU). This will results in a nominal frequency of 16MHz, which is sufficient to be able to read the PWM input. The clock source is set by configuring the fuses of the microcontroller.

Because the PWM signal is highly sensitive to differences, it is not guaranteed that the full output will be available. In order to ensure this, it can be a good idea to calibrate the internal ocillator of the chip. How to do this is explained in the datasheet of the microcontroller. In essence: 

* The system clock can be seen on the CLKI pin by configuring a fuse
* The internal clock can be read by an external instrument (e.g. multimeter)
* (The internal clock can be divided by 8 to make it easier to measure)
* The OSCCAL registry can then be adjusted to get the frequency close to the nominal 8MHz

### Fuse settings
* Lfuse: 0xE1
* Hfuse: 0xDF
* Efuse: 0xFF

Can be written using [avrdude][avrdude syntax documentation]


## Burning the software to the ATMEL
This is the easy part! [Simply use avrdude][avrdude syntax documentation]


## Pins
The inputs from the RC receiver are tied to hardware interrupts on the ATTiny85. This means that the timing pulse will initiate an interrupt vector in software, where the timekeeping will begin and end.

The ATTiny85 has 2 pins that can be used as PWM outputs. These are the output port of the chip when used as the RC parser. 

### Pin configuration
* Pin 1: RESET (not used by program. Tie to Vcc with resistor.)
* Pin 2: PCINT3/PB3, interrupt vector and first channel input
* Pin 3: PCINT4/PB4, interrupt vector and second channel input
* Pin 4: GND
* Pin 5: PB0/OC0A, PWM output 1
* Pin 6: PB1/OC0B, PWM output 2
* Pin 7: PB2 _Not utilised_
* Pin 8: Vcc

# External resources
* [WIN-AVR][WIN-AVR link]
* [USBASP programmers][USBASP link]
* [AVR-LIBC library](http://www.nongnu.org/avr-libc/)
* [Fuse calculator for atmel chips](http://www.engbedded.com/fusecalc/)
* [General troubleshooting at AVR Freaks](http://avrfreaks.net/)

[avrdude syntax documentation]: http://www.nongnu.org/avrdude/user-manual/avrdude_4.html#Option-Descriptions
[Win-AVR link]:(http://winavr.sourceforge.net/)
[USBASP link]:(http://www.fischl.de/usbasp/)