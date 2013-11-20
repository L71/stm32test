// SPI1 DAC control setup (header file)

#ifndef _SPI1_H_

#define _SPI1_H_

// setup HW
void spi1_dac_setup_hw(void);

// end an SPI transfer with this; this raises the SS signal
void spi1_dac_finalize(void);
	
// void spi1_dac_write(uint16_t);	// internal function, called by the two following...
void spi1_dac_write_cha(uint16_t);	// write 16 bits to DAC channel A
void spi1_dac_write_chb(uint16_t);	// write 16 bits to DAC channel B



#endif
