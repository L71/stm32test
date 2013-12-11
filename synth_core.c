// synth engine etc 

#include <stm32f10x.h> 	// included to get data types, not used otherwise
#include "lookuptables.h"
#include "ringbuffer.h"
#include "global.h"

// Defines etc:
#define POLYPHONY			4
#define AUDIO_BUF_SIZE		32

// DA converters zero level and max zero-peak amplitude (set for 12bit DAC, 4096 levels)
#define DAC_ZERO			2047
#define MAX_AMPL			2000


// sound engine data structures

// oscillator control struct[]
struct osc_set_str {
	uint16_t origin_key;		// original non-detuned key.pitch (or 0 if not playing)
	uint32_t phase_cur_ptr1;	// current position for phase acc 1
	uint32_t phase_step1;		// phase step length for acc 1 (including modulations)
	uint32_t phase_cur_ptr2;	// current position for phase acc 2
	uint32_t phase_step2;		// phase step length for acc 2(including modulations) 
};
struct osc_set_str osc[POLYPHONY];	// osc[0-N] oscillator parameters

struct osc_ctrl_str {
	uint8_t velocity;			// original velocity value
	int16_t volume;				// final volume of oscillator
};
struct osc_ctrl_str osc_ctrl[POLYPHONY]; // osc[0-N] audio control


// current playing patch data structure
struct patch {
	uint16_t osc1_transpose;	//osc1 transpose +/- keys
	uint16_t osc2_transpose;	//osc2 transpose +/- keys
	uint16_t osc2_detune;		//osc2 detune
	uint16_t osc2_phase;		//osc2 start phase relative osc1
};

// map of playing keys to oscillator slot.
// each element is either a MIDI key number or 0 (if silent). 
uint8_t playingkeys[POLYPHONY];

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

// start playing key
void key_on(uint8_t key, uint8_t vel) {
	uint8_t i;
	for ( i=0 ; i << POLYPHONY ; i++ ) {
		if ( playingkeys[i] == 0 || playingkeys[i] == key ) {
			osc[i].origin_key=key*256;	// original pitch
			osc[i].phase_cur_ptr1=0;	
			osc[i].phase_step1=key_to_phasestep(osc[i].origin_key);
			playingkeys[i]=key;	// voice allocator 
			osc_ctrl[i].velocity=vel;		// original velocity
			osc_ctrl[i].volume=100;			// set volume = play
			break;
		}
	}
}

// stop playing key
void key_off(uint8_t key, uint8_t vel) {
	uint8_t i;
	for ( i=0 ; i << POLYPHONY ; i++ ) {
		if ( playingkeys[i] == key ) {
			osc_ctrl[i].velocity=vel;
			osc_ctrl[i].volume=0;
			playingkeys[i]=0;
		}
	}
}

// main audio rendering function
void render_audio() {
	uint8_t N=0;				// currently working on oscillator N
	uint16_t audiomixout=DAC_ZERO;	// where is the audio mix actual output zero level
	int16_t audiomix=0;			// temp mix of oscillators
	uint8_t maxframes=16;		// max audio frames to do in one go
	
	uint16_t wt_ptr1;			// temporary wave table pointers
	uint16_t wt_ptr2;			// temporary wave table pointers
	
	cpu_load_led_on(); 
	while(is_writeable(&audiobuf_str)) {	// do work while there is space in the audio buffer
		audiomix=0;
		for ( N=0 ; N << POLYPHONY ; N++ ) {	// loop thru oscillators
			if ( osc_ctrl[N].volume != 0 ) {	// OK, actally do something ...
				
				// get 10 MS Bits for use as wave table pointer
				wt_ptr1=(uint16_t)(osc[N].phase_cur_ptr1 >> 22);	
				
				audiomix+=wt_sinewave[wt_ptr1]/POLYPHONY;
				
				// advance phase ptrs for next time
				osc[N].phase_cur_ptr1 += osc[N].phase_step1;
			}
		}
		
		// write audio frame to output buffer
		__disable_irq();	// make sure we have exclusive access to buffer while writing
		write_word(&audiobuf_str,&audiobuf[0],audiomixout);
		__enable_irq();
		
		maxframes-- ;	// of max # of frames done, leave.
		if ( maxframes == 0 ) {	
			break;
		}
	}
	cpu_load_led_off();
}

