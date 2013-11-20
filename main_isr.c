

// main system timer setup & ISR 
#include <stm32f10x.h> 

// delay function for debug
void delay(void){
	int i = 50; /* waaaaiiiit */ 
	while (i-- > 0)
		asm("nop"); 
}

// Setup main timer & interrupt for system
void setup_main_isr(void) {
	
	// Timer4: CH1 - CH4 used for PWM output
	// reload register = 256, prescaler = 7
	// 72MHz / 7 / 256 -> 40178 interrupts/second (also PWM base frequency)
	
	// enable clock to timer4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN ;
	
	// enable timer4 as interrupt source
	NVIC->ISER[0] |= 0x40000000 ;
	
	// prescaler = 7
	TIM4->PSC = 6 ;
	
 	// TIM4 auto-reload register (0-255 is PWM range)
	TIM4->ARR = 255 ;
	
	// setup CH1 - CH4 for PWM output
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 ;
	TIM4->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 ;
	TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 ;
	TIM4->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 ;
	
	// actually enable output pins
	TIM4->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	
	TIM4->CCR1 = 25 ;
	TIM4->CCR2 = 75 ;
	TIM4->CCR3 = 155 ;
	TIM4->CCR4 = 225 ;
}

void enable_main_isr() {
	// reset timer
	TIM4->CNT = 0x0 ;
	// enable interrupts at timer4 update events
	TIM4->DIER |= TIM_DIER_UIE;
	// enable counter in upcounting mode
	TIM4->CR1 |= TIM_CR1_CEN | TIM_CR1_URS ; 
}
