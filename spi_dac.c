// SPI1 DAC control setup 

#include <stm32f10x.h> 

// setup SPI1
void spi1_setup_hw(void) {
	// setup: 16bit data frames, bit rate 72MHz/8 = 9MHz, SPI Master mode, SW slave management, Set SSI bit (keep as master).
	SPI1->CR1 = SPI_CR1_DFF | SPI_CR1_BR_0 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI ;
	
	// Use Slave Select output (SS)
	// SPI1->CR2 = SPI_CR2_SSOE ;
	
	// PA4 is SS output, active low. Set it H (inactive) now.
	GPIOA->BSRR = 0x0010 ;
	
	// Go! 
	SPI1->CR1 |= SPI_CR1_SPE ; 

}

void spi1_dac_finalize(void) {
	// check (and if necessary, wait for) completion of SPI transfer.
	while(SPI1->SR & SPI_SR_BSY) { // wait for BSY to clear
		;
	}
	GPIOA->BSRR = 0x0010 ;	// SPI SS -> H
}

void spi1_dac_write_cha(uint16_t word) {
	word &= 0x0fff ;	// strip any unwanted bits
	word |= 0x1000 ;
	GPIOA->BRR = 0x0010 ;	// SS -> L
	SPI1->DR = word ;
}

void spi1_dac_write_chb(uint16_t word) {
	word &= 0x0fff ;	// strip any unwanted bits
	word |= 0x9000 ;
	GPIOA->BRR = 0x0010 ;	// SS -> L
	SPI1->DR = word ;
}

