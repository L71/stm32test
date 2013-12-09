TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O2 -g -Wall
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o iopins.o main_isr.o lcd.o ringbuffer.o midi.o spi_dac.o synth_core.o global.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common

resources: 
	./data/lookuptables.py > lookuptables.h
