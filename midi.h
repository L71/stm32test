// midi input/output HW interfaceing and processing (header file)

#ifndef _MIDI_H_

#define _MIDI_H_

// STM32 USART1 setup
void midi_setup_hw(void);

// initialize MIDI data buffers
void midi_setup_buffers(void);

// copy a byte to the MIDI buffer	
void midi_buffer_byte(uint8_t);

// MIDI data input processor
void midi_process_buffer(void);

#endif 

