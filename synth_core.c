// synth engine etc 

#include <stm32f10x.h> 	// included to get data types, not used otherwise
#include "lookuptables.h"
#include "ringbuffer.h"

// Defines etc:
#define POLYPHONY			4
#define AUDIO_BUF_SIZE		32



// sound engine data structures

// oscillator control struct[]
struct osc_set_str {
	uint16_t origin_key;		// original non-detuned key.pitch (or 0 if not playing)
	uint32_t phase_cur_ptr1;	// current position for phase acc 1
	uint32_t phase_step1;		// phase step length for acc 1 (including modulations)
	uint32_t phase_cur_ptr2;	// current position for phase acc 2
	uint32_t phase_step2;		// phase step length for acc 2(including modulations)
};
struct osc_set_str osc[POLYPHONY];	// osc[0-N] 

// current playing patch data structure
struct patch {
	uint16_t osc1_transpose;	//osc1 transpose +/- keys
	uint16_t osc2_transpose;	//osc2 transpose +/- keys
	uint16_t osc2_detune;		//osc2 detune
	uint16_t osc2_phase;		//osc2 start phase relative osc1
};


// Audio data output buffer, filled in here and read from main timer ISR
struct ringbuf audiobuf_str; 
// the actual data buffer
uint16_t audiobuf[AUDIO_BUF_SIZE];	


// convert midikey.fraction pitch value (8+8bits) to 32-bit phase accumulator step length.
uint32_t key_to_phasestep(uint16_t key_fract) {

	uint16_t key=((key_fract & 0xff00) >> 8);	// MIDI key number
	uint16_t fract=(key_fract & 0x00ff);			// MIDI key fraction
	
	uint16_t table_fractions=64;	// table entries per note/key
	uint16_t table_base_key=96;	// base MIDI key for the phase acc. table
	uint16_t divider=1;	// table divisor
	
	//failsafe; pitch > midi key #108 (C8) not supported. Max out here.
	if (key_fract > ((96+13)*256)) {
		return(phasetable[12*table_fractions]);
	}

	// loop and adjust divider until we have the right octave
	while (key < table_base_key) {
		divider=divider << 1;	// octave divider ++
		key=key+12;	// key + 1 octave
	}
	
	// now key is in table interval.
	// find out which ... 
	uint16_t k=key-table_base_key;	// no of key into the table we are
	uint16_t f=fract/(256/table_fractions);	// additional fraction
	
	return(phasetable[(k*64)+f]/divider);	// return entry in table
}


// synth core startup stuff
void synth_core_setup() {
	// initialize audio output buffer
	buffer_init(&audiobuf_str, AUDIO_BUF_SIZE);
}



