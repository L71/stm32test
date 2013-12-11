#include <stm32f10x.h> 

#include "iopins.h"
#include "main_isr.h"
#include "lcd.h"
#include "midi.h"
#include "ringbuffer.h"
#include "spi_dac.h"
#include "global.h"
#include "synth_core.h"


// main system TIM4 interrupt handler
void TIM4_IRQHandler(void)
{
	// light up ISR execution indicator
	cpu_load_led_on(); 
	
	// finish any still running SPI DAC transaction
	spi1_dac_finalize();
	
	// static loop counter, keeps track of periodic actions
	// to execute at lower intervals than main ISR period
	static uint8_t isr_c ;
	isr_c++ ;
	isr_c &= 0b00011111 ; // ISR counter, max 32 = wrap around at appr. 0.8 ms.
	
	
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
	spi1_dac_write_cha(read_audio_buffer());
	
	// kill ISR execution indicator
	cpu_load_led_off();
	
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
	
	// setup SPI1 DAC communication
	spi1_dac_setup_hw();
	
	// initialize & start main timer
	setup_main_isr();
	enable_main_isr();
	
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