#include "digital_io.h"
#include <stdint.h>
#include "stm32f303xc.h"



// store a pointer to the function that is called when a button is pressed
// set a default value of NULL so that it won't be called until the
// function pointer is defined
void (*on_button_press)() = 0x00;

static uint8_t led_state = 0x00;

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

/**
 * Get the current LED bitmask
 * @return Current state of LEDs as a bitmask
 */
uint8_t GetLEDBitmask()
{
    // Return the current LED state from our stored variable
    return led_state;
}

/**
 * Set the LED state using a bitmask
 * @param mask Bitmask representing LED states
 */
void SetLEDBitmask(uint8_t mask)
{
    // Store the new LED state
    led_state = mask;

    // Write the pattern to the high byte of the ODR register (PE8-PE15)
    uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;
    *led_register = mask;
}



void DigitalInitialise(void (*callback)())
{
	enable_clocks();
	initialise_board();

	// set the interrupt handling function
	on_button_press = callback;

	// enable the interrupt for the button
	enable_interrupt();

}


