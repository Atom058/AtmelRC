# AtmelRC
A project to parse RC PWM signals with an ATTiny85. It might be possible to use other Atmel chips as well, but the program was aimed at the capabilities of the ATTiny85. There are probably some registries which needs to be updated in order for it to work with other devices. No warranties ;).

The program will initially not aim to reduce power consumption. The assumption is that the other RC equipment will consume significantly more power, making the contribution from the ATTiny85 insignificant in comparison.

The firmware features a calibration mode to ensure that the full output is made available.

# Installation/How to use
This will be a brief guide of how to configure everything to work!

## Getting the tools
The toolchain used by the author was provided through [WIN-AVR][WIN-AVR link]. This toolchain is compatible with Windows, but the "originals" are living in UNIX.

The files are prepared so that a chip can be programmed with [avrdude](http://www.nongnu.org/avrdude/) or similar (included in Win-AVR package).

Finally, a programmer is needed. The simplest form is probably an Arduino, or a [USBASP][USBASP link]. The list of avrdude's supported programmers [can be found here][avrdude syntax documentation].


## Fuses and selection of clock sources
The ATTiny85 will run on the Internal PPL clock as clk(CPU). This will result in a nominal frequency of 16MHz, which is sufficient to be able to read the PWM input. The clock source is set by configuring the fuses of the microcontroller.

The fuses are set to preserve EEPROM settings during chip-erase cycles. This means that the calibration settings stored in the EEPROM are not erased during a firmware update. 

Because the PWM signal is highly sensitive to differences, it is not guaranteed that the full output will be available. In order to ensure this, it can be a good idea to calibrate the internal oscillator of the chip. How to do this is explained in the datasheet of the microcontroller. In essence: 

* The system clock can be seen on the CLKI pin by configuring a fuse
* The internal clock can be read by an external instrument (e.g. multimeter)
* (The internal clock can be divided by 8 to make it easier to measure)
* The OSCCAL registry can then be adjusted to get the frequency close to the nominal 8MHz

### Fuse settings
* Lfuse: 0xE1
* Hfuse: 0xD7
* Efuse: 0xFF

The fuse settings can be written using [avrdude][avrdude syntax documentation], as such:
```
avrdude -p t85 -U lfuse:w:0xe1:m -U hfuse:w:0xd7:m -U efuse:w:0xff:m
```

## Burning the software to the ATTiny85
This is the easy part! [Simply use avrdude][avrdude syntax documentation], as such:

```
avrdude -p t85 -U flash:w:atmelrc.hex:h
```

## Calibration of PWM output
The output of the PWM is dependent on how the ingoing signals gets processed. To ensure a proper and full operation, a calibration of the device is needed. The process of calibration is explained in the steps below. The easiest way to calibrate is probably to connect the PWM outputs to a LED.

1. Connect the GND of the ATTiny85 with the GND for the RC receiver
1. Connect the signal cables to PB3 (_Pin 2_) and PB4 (_Pin 3_) for channel A and channel B respectively.
1. Connect PB2 (_Pin 7_) to VCC to initiate calibration
1. The Channel A PWM output will turn on; the calibration has now started.
	1. Move Channel A to MAX position.
	1. Move Channel A to MIN position. An adjustment from MAX is needed for this to be registered.
	1. Hold the outputs for ~5 seconds until the PWM output turns off. 
		* The PWM output can be adjusted between min/max values during this waiting time. 
		* However, the output will never be fully off, because the _calibration done_ signal should be noticable.
		* If a new MAX position is recorded during this time, the calibration resets to step 2.
	1. After Channel A's output turns off, calibration of Channel B starts (will only calibrate if Channel B has input).
1. __OPTIONAL__: The Channel B PWM output will turn on; the calibration has now started.
	1. Move Channel B to MAX position.
	1. Move Channel B to MIN position.
	1. Wait ~5 seconds until Channel B's output turns off.
1. Calibration levels are now recorded, but not saved.
1. Disconnect __PB2__ from VCC. The calibration values is stored to EEPROM, making them persistent. __IMPORTANT__: _Do not disconnect power to the ATTiny85 during this step, as this might lead to the EEPROM becoming corrupted_.
1. DONE! The maximum and minimum values are now stored. __NOTE__: The calibration values will not be updated unless the entire motion of one channel is completed. Channel B should therefore not update if only Channel A was calibrated. 

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
* Pin 7: PB2 _Calibration pin, set to VCC to begin calibration._
* Pin 8: Vcc

#Known bugs
* B1: The calibration will sometimes be overwritten with the wrong values. This is most likely due to a brown-out error.
* B2: The FULL ON position for a switch does not give a satisfying full-on signal. This could probably be solved with removing the PWM and replacing it with a normal pinout function.

# External resources
* [WIN-AVR][WIN-AVR link]
* [USBASP programmers][USBASP link]
* [AVR-LIBC library](http://www.nongnu.org/avr-libc/)
* [Fuse calculator for Atmel chips](http://www.engbedded.com/fusecalc/)
* [General troubleshooting at AVR Freaks](http://avrfreaks.net/)

[avrdude syntax documentation]: http://www.nongnu.org/avrdude/user-manual/avrdude_4.html#Option-Descriptions
[WIN-AVR link]:http://winavr.sourceforge.net/
[USBASP link]:http://www.fischl.de/usbasp/