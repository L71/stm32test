
// setup IO pins globally 

#include <stm32f10x.h> 


void setup_iopins(void) {

	// Enable clock to GPIO PORTS and other peripherals
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN |RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_SPI1EN ;
	
	// GPIO port A pin config:
	// PA4 - SPI1 NSS - GPIO output, NO HW SPI NSS management.
	// PA5 - SPI1 SCK
	// PA7 - SPI1 MOSI
	// PA9 - USART1 TX (alternate push/pull output)
	// PA10 - USART1 RX (floating input)
	GPIOA->CRH = 0x00000490 ;
	GPIOA->CRL = 0x90930000 ;
	
	// GPIO port B pin config:
	// PB6 - TIM4 CH1 PWM output
	// PB7 - TIM4 CH2 PWM output
	// PB8 - TIM4 CH3 PWM output
	// PB9 - TIM4 CH4 PWM output
	GPIOB->CRH = 0x00000099 ;
	GPIOB->CRL = 0x99000000 ;


	// GPIO port C pin config:
	// PC 8, 9, 10 = outputs push/pull, 50MHz clock for misc. status LEDs.
	GPIOC->CRH = 0x00000333;
	// PC 0-5 = output push/pull 50MHz clock for LCD display
	// 0-3 are data bits, 4 is command/data (RS) line and 5 is data latch clock (E).
	GPIOC->CRL = 0x00333333 ;

	
	
}

