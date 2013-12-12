#!/usr/bin/env python 

import numpy
import p2c

# This part calculates the phase stepping data for the phase accumulators
# Output: list of phase counter values (uint32_t[]).
# Equal 12-tone temperament

def phasetable() :
    keys=12
    fractions=64   # steps in between each half note
    base_freq=2093   # frequency of C7, base note of the highest supported octave
    base_key=96   # MIDI key

    pcm_freq=40178.0    # PCM playback sample rate
    pcm_step_max=2**32 # 32 bit phase counters

    i=0             # counter

    datalist=[]

    while i <= ( keys*fractions ) :
        freq=base_freq*(2.0**(1.0/(keys*fractions)))**i
        step=int(round(pcm_step_max/(pcm_freq/freq)))
        # print i, freq, step
        datalist.append(step)
        i=i+1

    p2c.print_c_list('uint32_t', 'phasetable', datalist)


# build a sine wave table
# 1024 samples * 12 bit resolution

def sinewave() :
    samples=1024.0
    amplitude=2047.0    # P-P /2

    i=0
    datalist=[]
    while i < samples :
        value=numpy.sin((2*numpy.pi)*(i/samples))*amplitude 
        datalist.append(int(round(value)))
        i=i+1
    # print datalist
    p2c.print_c_list('int16_t', 'wt_sinewave', datalist)   


# build a 2d array of sines+Nth harmonics
def sines_wavetable() :
    samples=1024.0      # samples per table
    amplitude=2047.0    # amplitude scale P-P/2
    out_array=[]
    wave_array=[]
    harmonics_max=5     # final table will have up to this harmonic added
    
    n=1
    while n <= harmonics_max :
        i=0.0
        while i < samples :
            x=1.0
            mix=0.0
            while x <= n :
                mix=mix+numpy.sin((2*numpy.pi)*(i/samples)*x)*(1/x)*amplitude
                x=x+1
            wave_array.append(int(round(mix)))
            i=i+1
        out_array.append(wave_array)
        wave_array=[]
        n=n+1
    # print out_array
    p2c.print_c_2d_array('int16_t', 'wt_sinewave', out_array)
    
# main call list
phasetable()
# sinewave()
sines_wavetable()




