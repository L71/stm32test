// global utility functions and definitions (header file)

#ifndef _GLOBAL_H_

#define _GLOBAL_H_

// CPU load / does actual work indicator on/off
//
void cpu_load_led_on(void);
void cpu_load_led_off(void);




// define error conditions; there are ORed into global_error if they happen.
#define LCD_BUF_NOT_WR			0x00000001		// LCD buffer not writeable = overrun
#define MIDI_BUF_NOT_WR			0x00000002		// MIDI buffer not writeable = overrun
#define AUDIO_BUF_NOT_RD		0x00000004		// AUDIO buffer notreadable = underrun
// set global error conditions
void global_indicate_error(uint32_t);



#endif
