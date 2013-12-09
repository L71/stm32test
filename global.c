// global utility functions and definitions

// CPU load / does actual work indicator on/off
void cpu_load_led_on(void) {
	GPIOC->BSRR = GPIO_BSRR_BS8;	// LED ON!
}
void cpu_load_led_off(void) {
	GPIOC->BRR = GPIO_BRR_BR8;		// LED OFF!
}

