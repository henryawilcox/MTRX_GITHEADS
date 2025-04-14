#include <stdint.h>
#include "stm32f303xc.h"
#include "timer.h"

void enable_clocks(void);
void initialise_board(void);

// Example LED blink callback
void periodicBlink(void) {
    static uint8_t led_state = 0; //start with LEDs off
    uint8_t *led_output = ((uint8_t *)&(GPIOE->ODR)) + 1;
    *led_output = led_state ? 0xAA : 0x00;  // 0xAA = 10101010
    led_state = !led_state; //switch the LED state from 10101010 to 00000000 or vice versa dependent on current
}

void oneShotBlink(void) {
    uint8_t *led_output = ((uint8_t *)&(GPIOE->ODR)) + 1;
    *led_output = 0xFF;  // turn all LED's briefly
    for (volatile int i = 0; i < 10000; i++); //gunshot flash for some arbitrary amount of time, small relative to wait time
    *led_output = 0x00; //turn off LEDs
}

void enable_clocks(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // For EXTI
}

void initialise_board(void) {
    // Configure PE8-15 as output
    uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
    *led_output_registers = 0x5555;

    // Turn off LEDs initially
    uint8_t *led_output = ((uint8_t *)&(GPIOE->ODR)) + 1;
    *led_output = 0x00;

    // Configure PC13 as input (default state)
    GPIOA->MODER &= ~(3 << (0 * 2)); // PA0 input

    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;      // Clear EXTI0 bits
    SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI0_PA;   // Set EXTI0 to PA0

    EXTI->IMR  |= EXTI_IMR_MR0;     // Unmask EXTI0
    EXTI->FTSR |= EXTI_FTSR_TR0;    // Trigger on falling edge
    NVIC_EnableIRQ(EXTI0_IRQn);     // Enable EXTI0 interrupt in NVIC

}

//THESE COMMENTED SECTIONS DEMONSTRATE THE TIMERS USING A BUTTON AS INTERRUPT, IN INTEGRATION, THIS WILL BE DONE WHEN THE NEWLINE
//CHARACTER IS ENTERED ON UART. PLEASE DO NOT DELETE. THIS WILL BE USED WHEN DEMONSTRATING TIMERS ON DEMO DAY

//uncomment this interrupt handler for one shot timing of the period outlined in the handler Timer_StartOneShot(XXXX, oneShotBlink);
/*void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0;

        uint32_t delay = 3000;
        Timer_StartOneShot(delay, oneShotBlink);
    }
}*/

//uncomment this interrupt handler for periodic timing of the period outlined in the main call Timer_Init(XXXX, periodicBlink);

/*void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0;

        uint32_t newPeriod = 1000;
        Timer_Init(newPeriod, periodicBlink);
        Timer_Start();

        periodicBlink(); // Immediate first toggle
    }
}*/

//FOR EMILY CHECK HERE FOR INTEGRATION CODE!
//COPY AND PASTE THESE INTO THE RELEVANT IF STATEMENTS FOR THE INTEGRATION

//PLEASE SEE BELOW IN main(void) SOME EXAMPLES ON HOW TO ACTUALLY CALL THE FUNCTIONS

//THIS CODE SHOULD RUN AFTER YOU HAVE CHECKED IF THE OPERATOR IS ONESHOT AND THE OPERAND IS ALL DIGITS
//IT SHOULD BE DEFINED IN INTEGRATION.c OR IN A RELEVANT HEADER (INTEGRATION.h)
void oneshot_action(uint32_t operand){
	Timer_StartOneShot(operand, oneShotBlink); //call function to trigger a oneshot using the specified operand from the serial comm.
}

//THIS CODE SHOULD RUN AFTER YOU HAVE CHECKED IF THE OPERATOR IS ONESHOT AND THE OPERAND IS ALL DIGITS
//IT SHOULD BE DEFINED IN INTEGRATION.c OR IN A RELEVANT HEADER (INTEGRATION.h)
void periodic_action(uint32_t operand){
	setTimerPeriod(operand); //set the hardware timer ARR period as the operand given in the serial comm.
	Timer_Init(operand, periodicBlink); // initialise the timer for periodic blinking with the operand as the delay_time
	Timer_Start();
	periodicBlink(); // Immediate first toggle
}




int main(void) {
    enable_clocks();
    initialise_board();

    //example of how to call the periodic function in the integration section

    /*
    uint32_t delay_period_example = 1000; //set some number for the period (0.5s on, 0.5s off)
    periodic_action(delay_period_example); //call for a repeated flash using this delay period
    */

    //example of how to call the one shot function in the integration section
    /*
    uint32_t wait_time_example = 5000; //set some number for the one shot wait time (5s wait then a small gun shot flash)
    oneshot_action(wait_time_example); //call for the one shot using the specified delay time
    */

    while (1) {
        // Main loop does nothing; logic handled in interrupts
    }
}
