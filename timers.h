// main system timer setup & ISR (header file)

#ifndef _TIMERS_H_

#define _TIMERS_H_

// setup and enable the high frequency timer (audio playback interrupt)
void setup_hf_timer(void);
void enable_hf_timer(void);

// setup and enable the low frequency timer (LFO, ENV and UI updates etc.)
void setup_lf_timer(void);
void enable_lf_timer(void);

#endif

