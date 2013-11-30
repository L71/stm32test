TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O1 -g -Wall
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o iopins.o main_isr.o lcd.o ringbuffer.o midi.o spi_dac.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common

resources: 
	./lookuptables.py > lookuptables.h
