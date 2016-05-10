# AtmelRC
A project to parse RC PWM signals with an ATTiny85

# Fuses and selection of clock sources
The ATTiny will run on the Internal PPL clock as clk(CPU). This will results in a nominal frequency of 16MHz, which is sufficient to be able to read the PWM input.

## Fuse settings
* Lfuse: 0x61
* Hfuse: 0xDF
* Efuse: 0xFF