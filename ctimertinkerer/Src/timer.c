#include "timer.h"
#include "stm32f303xc.h"

static TimerCallback periodicCallback = 0;
static uint32_t timerPeriod_ms = 0;

static TimerCallback oneShotCallback = 0;
static uint8_t isOneShotMode = 0;

void Timer_Init(uint32_t period_ms, TimerCallback cb) {
    timerPeriod_ms = period_ms;
    periodicCallback = cb;

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 8000 - 1;   // 64 MHz / 64000 = 1 kHz (1 tick = 1 ms)
    TIM2->ARR = timerPeriod_ms;

    TIM2->EGR |= TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void Timer_Start(void) {
    while (1) {
        if (TIM2->SR & TIM_SR_UIF) { //"Has the timer expired?"
            TIM2->SR &= ~TIM_SR_UIF; //clears the update flag

            if (isOneShotMode) { //Checks whether we're in one-shot mode (trigger a callback once, then stop).
                if (oneShotCallback) oneShotCallback(); //If a valid callback function is stored, call it.
                oneShotCallback = 0; //set callback to 0
                isOneShotMode = 0; //keep going
                TIM2->CR1 &= ~TIM_CR1_CEN; // Stop timer
                return;
            }

            if (periodicCallback) periodicCallback();
        }
    }
}

void setTimerPeriod(uint32_t new_period_ms) {
    timerPeriod_ms = new_period_ms;
    TIM2->ARR = timerPeriod_ms;
    TIM2->EGR |= TIM_EGR_UG;
}

uint32_t getTimerPeriod(void) {
    return timerPeriod_ms;
}

//void Timer_StartOneShot(uint32_t delay_ms, TimerCallback cb) {
//    oneShotCallback = cb;
//    isOneShotMode = 1;
//
//    TIM2->PSC = 8000 - 1;    // 1 ms tick
//    TIM2->ARR = delay_ms;
//    TIM2->CNT = 0;
//
//    TIM2->EGR |= TIM_EGR_UG;
//    TIM2->SR &= ~TIM_SR_UIF;
//    TIM2->CR1 |= TIM_CR1_CEN;
//
//    // Blocking wait for one-shot to complete
//    while (1) {
//        if (TIM2->SR & TIM_SR_UIF) {
//            TIM2->SR &= ~TIM_SR_UIF;
//
//            if (oneShotCallback) oneShotCallback();
//            oneShotCallback = 0;
//            isOneShotMode = 0;
//
//            TIM2->CR1 &= ~TIM_CR1_CEN; // Stop timer
//            return;
//        }
//    }
//}

void Timer_StartOneShot(uint32_t delay_ms, TimerCallback cb) {
    oneShotCallback = cb;
    isOneShotMode = 1;

    TIM2->CR1 &= ~TIM_CR1_CEN;      // Stop timer before config
    TIM2->PSC = 8000 - 1;           // 1 ms tick
    TIM2->ARR = delay_ms;
    TIM2->CNT = 0;

    TIM2->EGR |= TIM_EGR_UG;        // Force update
    TIM2->SR &= ~TIM_SR_UIF;
    TIM2->CR1 |= TIM_CR1_CEN;       // Start timer

    // Blocking wait for one-shot to complete
    while (1) {
        if (TIM2->SR & TIM_SR_UIF) {
            TIM2->SR &= ~TIM_SR_UIF;

            if (oneShotCallback) oneShotCallback();
            oneShotCallback = 0;
            isOneShotMode = 0;

            TIM2->CR1 &= ~TIM_CR1_CEN; // Stop timer
            return;
        }
    }
}
