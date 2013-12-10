// synth engine etc (header file)

#ifndef _SYNTH_CORE_H_

#define _SYNTH_CORE_H_

// convert midikey.fraction value (8+8bits) to 32-bit phase accumulator step length.
uint32_t key_to_phasestep(uint16_t);

// initialize audio buffers & other necessary startup work.
void synth_core_setup(void);
	

#endif
