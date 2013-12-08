// synth engine etc 

#include <stm32f10x.h> 	// included to get data types, not used otherwise
#include "lookuptables.h"


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
