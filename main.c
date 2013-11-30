#include <stm32f10x.h> 

#include "iopins.h"
#include "main_isr.h"
#include "lcd.h"
#include "midi.h"
#include "ringbuffer.h"
#include "spi_dac.h"

void ldelay(void){
	int i = 10000; /* waaaaiiiit */ 
	while (i-- > 0)
		asm("nop"); 
}

void led_toggle(void)
{
    // If PORTC BIT 9 set, clear it
    if (GPIOC->ODR & GPIO_ODR_ODR9)
    {
        GPIOC->BRR = GPIO_BRR_BR9;
    }
    else // set it
    {
		GPIOC->BSRR = GPIO_BSRR_BS9;
    }
}


// main system TIM4 interrupt handler
void TIM4_IRQHandler(void)
{
	// static loop counter, keeps track of periodic actions
	// to execute at lower intervals than main ISR period
	static uint8_t isr_c ;
	isr_c++ ;
	isr_c &= 0b00111111 ; // ISR counter, max 64 = wrap around at appr. 1.6 ms.
	
	uint8_t midi_input = 0 ; 
	
	// light up ISR execution indicator
	GPIOC->BSRR = GPIO_BSRR_BS8; 
	// delay();

	
	if ( isr_c == 32 ) {
		led_toggle();  // blink led on PC9
	} 
	
	if ( isr_c == 40 ) {	// check for data in LCD buffer
		lcd_hw_write();
	}
	if ( isr_c == 45 ) {	// finalize LCD data transfer
		lcd_hw_write_finalize();
	}
	
	if (USART1->SR & USART_SR_RXNE) {	// USART has recieved a byte
		// lcd_place_cursor(0,0);
		// lcd_write_char(127) ;
		midi_input = USART1->DR ;
		midi_buffer_byte(midi_input);
		// lcd_write_hex8(midi_input);
	}
	
	// end any SPI transactions
	// spi1_dac_finalize();
	
	// kill ISR execution indicator
	GPIOC->BRR = GPIO_BRR_BR8;
	// clear timer4 interrupt flag before return
	TIM4->SR = (uint16_t)~TIM_SR_UIF;  
}


// begin operations!
int main(void) {

	// system setup
	setup_iopins();
	
	// LCD init ... 
	lcd_setup_buffer();
	lcd_setup_hw();
	lcd_write_custom_chars();
	
	// setuo SPI1 DAC communication
	spi1_setup_hw();
	
	// initialize & start main timer
	setup_main_isr();
	enable_main_isr();
	
	// setup MIDI buffers and start USART for MIDI communication
	midi_setup_buffers();
	midi_setup_hw();

	
	lcd_place_cursor(5,0);
    uint8_t i;
    const uint8_t text[] = "- STM32F -";  
    for ( i=0 ; text[i] != '\0' ; i++ )
            lcd_write_char(text[i]) ;
	
	while (1) {
		// led_toggle();
		midi_process_buffer();
		// ldelay();
	}
	
}