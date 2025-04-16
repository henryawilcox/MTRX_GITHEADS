#include "timer.h"
#include "stm32f303xc.h"

static TimerCallback periodicCallback = 0;
static TimerCallback oneShotCallback = 0;

static uint32_t timerPeriod_ms = 0;
static uint8_t isOneShotMode = 0;

void Timer_Init(TimerCallback cb) {
    //timerPeriod_ms = period_ms;
    periodicCallback = cb;

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 8000 - 1;  // 1 ms tick
    //TIM2->ARR = timerPeriod_ms;
    uint32_t delay = 250;
    setTimerPeriod(delay);
    TIM2->CNT = 0;

    TIM2->DIER |= TIM_DIER_UIE;  // Enable update interrupt
    //TIM2->EGR |= TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF;

    NVIC_EnableIRQ(TIM2_IRQn);
}

void Timer_Start(void) {
    isOneShotMode = 0;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void Timer_StartOneShot(TimerCallback cb) {
    isOneShotMode = 1;
    oneShotCallback = cb;

    TIM2->CR1 &= ~TIM_CR1_CEN;      // Stop timer before configuring
    TIM2->PSC = 8000 - 1;           // 1 ms tick
    //TIM2->ARR = delay_ms;			// set overflow time
    uint32_t delay = 1000;
    setTimerPeriod(delay);
    TIM2->CNT = 0;					// start counting from 0

    TIM2->DIER |= TIM_DIER_UIE;		//setting other necessary timer vals
    //TIM2->EGR |= TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF;

    NVIC_EnableIRQ(TIM2_IRQn);		// enabling interrupt compatability
    TIM2->CR1 |= TIM_CR1_CEN;		// enabling control register
}

void setTimerPeriod(uint32_t new_period_ms) {
    timerPeriod_ms = new_period_ms;			//setting new period
    TIM2->ARR = timerPeriod_ms;				//setting overflow
    TIM2->EGR |= TIM_EGR_UG;				//setting event generation register
}

uint32_t getTimerPeriod(void) {
    return timerPeriod_ms;					//getting the period
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {			//if interrupt triggered
        TIM2->SR &= ~TIM_SR_UIF;			//reset flag

        if (isOneShotMode) {				//if in oneshot, call oneshot function using callback
            if (oneShotCallback) oneShotCallback();
            oneShotCallback = 0;			//don't do any more oneshots after you've done the first
            isOneShotMode = 0;
            TIM2->CR1 &= ~TIM_CR1_CEN; // Stop timing now
        } else {
            if (periodicCallback) periodicCallback();	//otherwise go back
        }
    }
}

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

//THESE CANNOT COEXIST BECAUSE THEY ARE BOTH DEFINITIONS OF THE BUTTON INTERRUPT HANDLER, IN INTEGRATION, THIS INTERRUPT WILL NOT BE USED TWICE
//SO THE FUNCTION CALLS WILL BE APPROPRIATELY NAMED AND DEFINED

//uncomment this interrupt handler for one shot timing of the period outlined in the handler Timer_StartOneShot(XXXX, oneShotBlink);
/*void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0;

        Timer_StartOneShot(oneShotBlink);
    }
}*/

//uncomment this interrupt handler for periodic timing of the period outlined in the main call Timer_Init(XXXX, periodicBlink);

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) { // check if flag for interrupt is set
        EXTI->PR |= EXTI_PR_PR0; // clear flag

        Timer_Init(periodicBlink); //function callback using the arbitrary period of 1000, this sets and gets the period
        //change the above number to modify the period
        Timer_Start(); //start timing

        periodicBlink(); // Initiate immediate first toggle, then continue on
    }
}

