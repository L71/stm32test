// global utility functions and definitions

#include <stm32f10x.h> 

#include "global.h"

// CPU load / does actual work indicator on/off
inline void cpu_load_led_on(void) {
	GPIOC->BSRR = GPIO_BSRR_BS8;	// LED ON!
}

inline void cpu_load_led_off(void) {
	GPIOC->BRR = GPIO_BRR_BR8;		// LED OFF!
}

// global error indication
//
uint32_t global_error=0;
void global_indicate_error(uint32_t new_error) {
	global_error |= new_error ;	// add error condition to register
	GPIOC->BSRR = GPIO_BSRR_BS9;	// light up error LED.
}

