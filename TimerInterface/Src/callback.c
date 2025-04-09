#include <callback.h>
#include "stm32f303xc.h"


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


static uint8_t off_time = 0x00;


void EXTI0_IRQHandler(void)
{
	// run the button press handler (make sure it is not null first !)
	if (on_button_press != 0x00) {
		on_button_press();
	}

	// reset the interrupt (so it doesn't keep firing until the next trigger)
	EXTI->PR |= EXTI_PR_PR0;
}


void enable_interrupt() {
	// Disable the interrupts while messing around with the settings
	//  otherwise can lead to strange behaviour
	__disable_irq();

	// Enable the system configuration controller (SYSCFG in RCC)
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// External Interrupts details on large manual page 294)
	// PA0 is on interrupt EXTI0 large manual - page 250
	// EXTI0 in  SYSCFG_EXTICR1 needs to be 0x00 (SYSCFG_EXTICR1_EXTI0_PA)
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;

	//  Select EXTI0 interrupt on rising edge
	EXTI->RTSR |= EXTI_RTSR_TR0; // rising edge of EXTI line 0 (includes PA0)

	// set the interrupt from EXTI line 0 as 'not masked' - as in, enable it.
	EXTI->IMR |= EXTI_IMR_MR0;

	// Tell the NVIC module that EXTI0 interrupts should be handled
	NVIC_SetPriority(EXTI0_IRQn, 1);  // Set Priority
	NVIC_EnableIRQ(EXTI0_IRQn);

	// Re-enable all interrupts (now that we are finished)
	__enable_irq();
}


void enable_clocks() {

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN; //enable the clocks for peripherals (GPIOA, C and E)
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //enable peripherals

}


void initialise_board() {

	// initialise the discovery board I/O outputs
	uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
	*led_output_registers = 0x5555;

}


void trigger_prescaler() {

	// make counter overflow so prescaler sets in
	TIM2->ARR = 0x01;
	TIM2->CNT = 0x00;
	asm("NOP");
	asm("NOP");
	asm("NOP");
	TIM2->ARR = 0xffffffff;

}


uint32_t GetPeriod()
{
    // Return the current LED state from our stored variable
    return off_time;
}



void SetPeriod(uint32_t period)
{
    // Store the new LED state
	off_time = period;


}


void TimerInitialise(void (*callback)()) // while loops in here, pass in (off-period, *callback)
{

	off_time = callback;

	enable_clocks();
	initialise_board();

	TIM2->CR1 |= TIM_CR1_CEN; //start the counter running

	TIM2->PSC = 0x04;
	trigger_prescaler(); // store a value for the prescaler
	/*
	@ questions for timed_loop
	@ what can make it run faster/slower (there are multiple ways)
	*/



}
