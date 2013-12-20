

// main system timer setup & ISR 
#include <stm32f10x.h> 

// Setup main timer & interrupt for system
void setup_hf_timer() {
	
	// Timer4: CH1 - CH4 used for PWM output
	// reload register = 256, prescaler = 7
	// 72MHz / 7 / 256 -> 40178 interrupts/second (also PWM base frequency)
	
	// set priority for interrupts according to group; no subgrouping
	SCB->AIRCR = 0x05fa0300;
	
	// set priority for this IRQ to highest (=0)
	NVIC->IP[TIM4_IRQn] = 0x00 ; // MSB nibble indicates priority
	
	// enable clock to timer4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN ;
	
	// enable timer4 (irq30) as interrupt source
	NVIC->ISER[0] |= NVIC_ISER_SETENA_30 ;
	
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
	
}

void enable_hf_timer() {
	// reset timer
	TIM4->CNT = 0x0 ;
	// enable interrupts at timer4 update events
	TIM4->DIER |= TIM_DIER_UIE;
	// enable counter in upcounting mode
	TIM4->CR1 |= TIM_CR1_CEN | TIM_CR1_URS ; 
}

void setup_lf_timer() {
	// Timer3: used for low frequency updates (ENVs, LFOs, UI I/O etc.)
	// prescaler = 719, reload register = 100
	// 72 MHz / 720 / 100 = 1000 interrupts/second 
	
	// set priority for this IRQ to 7 (halfway-low/high) 
	NVIC->IP[TIM3_IRQn] = 0x70 ; // MSB nibble indicates priority
	// enable clock to timer3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN ;
	
	// prescaling 72M / 720 for 10 ints/second resolution in counter
	TIM3->PSC = 719 ;
	
 	// TIM4 auto-reload register (will generate interrupt when this is reached)
	// test: should be 100 for 1ms interrupt interval
	TIM3->ARR = 200 ;	// 4 ints/sec
}

void enable_lf_timer() {
	// reset timer
	TIM3->CNT = 0x0 ;
	// enable counter in upcounting mode
	TIM3->CR1 |= TIM_CR1_CEN | TIM_CR1_URS ;
	// enable timer3 (irq29) as interrupt source
	NVIC->ISER[0] |= NVIC_ISER_SETENA_29 ;
	// enable interrupts at timer3 update events
	TIM3->DIER |= TIM_DIER_UIE;
}

