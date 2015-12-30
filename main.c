#include <stm32f10x.h> 

#include "iopins.h"
#include "timers.h"
#include "lcd.h"
#include "midi.h"
#include "ringbuffer.h"
#include "spi_dac.h"
#include "global.h"
#include "synth_core.h"


// main system TIM4 interrupt handler
void TIM4_IRQHandler()
{
	// light up ISR execution indicator
	cpu_load_led_on(); 
	
	// finish any still running SPI DAC transaction
	spi1_dac_finalize();
	
	// static loop counter, keeps track of periodic actions
	// to execute at lower intervals than main ISR period
	static uint32_t isr_c ;
	isr_c++ ;
	isr_c &= 0x0000003f ; // ISR counter, max 64 = wrap around at appr. 1.6 ms.
	
	
	if ( isr_c == 16 ) {	// check for data in LCD buffer
		lcd_hw_write();
	}
	if ( isr_c == 24 ) {	// finalize LCD data transfer
		lcd_hw_write_finalize();
	}
	
	if (USART1->SR & USART_SR_RXNE) {	// USART has recieved a byte
		// midi_input = USART1->DR ;
		// midi_buffer_byte(midi_input);
		midi_buffer_byte(USART1->DR);
	}
	
	// read audio buffer & copy to DAC CH A.
	spi1_dac_write_cha((uint16_t)read_audio_buffer());
	
	// kill ISR execution indicator
	cpu_load_led_off();
	
	// clear timer4 interrupt flag before return
	TIM4->SR = (uint16_t)~TIM_SR_UIF;  
	
}


void TIM3_IRQHandler() {
	cpu_load_led_on();
	if (GPIOC->ODR & GPIO_ODR_ODR10)
	{ 	// toggle LED @ GPIOC9
		GPIOC->BRR = GPIO_BRR_BR10;
	}
	else
	{
		GPIOC->BSRR = GPIO_BSRR_BS10;
	}


	cpu_load_led_off();

	// clear timer3 interrupt flag before return
	TIM3->SR = (uint16_t)~TIM_SR_UIF;
}

// begin operations!
int main(void) {

	// system setup
	setup_iopins();
	
	// LCD init ... 
	lcd_setup_buffer();
	lcd_setup_hw();
	lcd_write_custom_chars();
	
	// setup SPI1 DAC communication
	spi1_dac_setup_hw();
	
	// initialize & start audio (hf) timer
	setup_hf_timer();
	enable_hf_timer();
	// initialize & start env/lfo/ui etc (lf) timer
	// setup_lf_timer();
	// enable_lf_timer();
	
	// setup MIDI buffers and start USART for MIDI communication
	midi_setup_buffers();
	midi_setup_hw();

	// synth core setup & initialization
	synth_core_setup();
	
	
	lcd_place_cursor(5,0);
    uint8_t i;
    const uint8_t text[] = "- STM32F -";  
    for ( i=0 ; text[i] != '\0' ; i++ )
            lcd_write_char(text[i]) ;
	
	while (1) {
		// led_toggle();
		midi_process_buffer();
		// ldelay();
		render_audio();
	}
	
}