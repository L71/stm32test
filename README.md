STM32F103 based synth engine test code.

This test synth thingie was written for a Olimex STM32 development board with an STM32F103 microcontroller. In the current shape it does a not-entirely-horrible electric organ-type sound using a wavetable. 

Setup:
- a few LEDs on PWM outputs to indicate various statuses
- MCP4822 SPI DAC as audio output
- a 3.3V tolerant 4x20 OLED display (HD44700 or whatsitcalled-compatible. Some LCDs should work too)
- a MIDI input interface (optocoupler connected to serial port input).

You'll need to read the code to find out the details, sorry! :)
 
This is _very_ basic and I will probably not develop it any further. 
Also missing are some ST libs and startup files. Don't know if I can legally put these here so I won't. 
License: GPLv3, available here: http://www.gnu.org/licenses/gpl.txt

