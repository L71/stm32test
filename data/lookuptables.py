#!/usr/bin/env python 

import numpy

# python list -> C data type printer
def print_c_list(datatype, varname, data) :

    datasize=len(data)
    # defaults print parameters...
    cols=8 # insert line break every N lines in data output
    formatstr='{:#9x}' # default printout format 8 pos Hex.
    c=0
    
    # setup for specific data types
    if datatype == 'uint32_t' or datatype == 'int32_t' :
        formatstr='{:#11x}'
        cols=6
    elif datatype == 'uint16_t' or datatype == 'int16_t' :
        formatstr='{:#7x}'
        cols=8
    elif datatype == 'uint8_t' or datatype == 'int8_t' :
        formatstr='{:#5x}'
        cols=12
    
    # print C definition:
    print datatype, varname+'['+str(datasize)+']'+' = {'
    for i, e in enumerate(data) :
        # print e,
        print formatstr.format(e),
        if i < datasize-1 :
            print ',',
        c = c+1
        if c == cols :
            print ''
            c=0
    print '};'
    
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

    print_c_list('uint32_t', 'phasetable', datalist)


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
    print_c_list('int16_t', 'wt_sinewave', datalist)   
      
# main call list
phasetable()
sinewave()
