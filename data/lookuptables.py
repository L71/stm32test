#!/usr/bin/env python 


# python list -> C data type printer
def print_c_list(datatype, varname, data) :

    datasize=len(data)
    cols = 8 # insert line break every N lines in data output
    c = 0
    print datatype, varname+'['+str(datasize)+']'+' = {'
    for i, e in enumerate(data) :
        # print e,
        print '0x'+'{:08X}'.format(e),
        if i < datasize-1 :
            print ',',
        c = c+1
        if c == cols :
            print ''
            c=0
    print '}'
    



# This part calculates the phase stepping data for the phase accumulators
# Output: list of phase counter values (uint32_t[]).
# Equal 12-tone temperament

def phasetable() :
    keys = 12
    fractions = 2   # steps in between each half note
    base_freq = 2093   # frequency of C7, base note of the highest supported octave
    base_key = 96   # MIDI key

    pcm_freq = 40178.0    # PCM playback sample rate
    pcm_step_max=2**32 # 32 bit phase counters

    i=0             # counter

    datalist=[]

    while i <= ( keys*fractions ) :
        freq = base_freq*(2.0**(1.0/(keys*fractions)))**i
        step = int(round(pcm_step_max/(pcm_freq/freq)))
        # print i, freq, step
        datalist.append(step)
        i=i+1

    print_c_list('uint32_t', 'phasetable', datalist)


# main call list
phasetable()

