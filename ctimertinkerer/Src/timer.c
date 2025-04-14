#include "timer.h"
#include "stm32f303xc.h"

static TimerCallback periodicCallback = 0;
static TimerCallback oneShotCallback = 0;

static uint32_t timerPeriod_ms = 0;
static uint8_t isOneShotMode = 0;

void Timer_Init(uint32_t period_ms, TimerCallback cb) {
    timerPeriod_ms = period_ms;
    periodicCallback = cb;

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 8000 - 1;  // 1 ms tick
    TIM2->ARR = timerPeriod_ms;
    TIM2->CNT = 0;

    TIM2->DIER |= TIM_DIER_UIE;  // Enable update interrupt
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF;

    NVIC_EnableIRQ(TIM2_IRQn);
}

void Timer_Start(void) {
    isOneShotMode = 0;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void Timer_StartOneShot(uint32_t delay_ms, TimerCallback cb) {
    isOneShotMode = 1;
    oneShotCallback = cb;

    TIM2->CR1 &= ~TIM_CR1_CEN;      // Stop timer before configuring
    TIM2->PSC = 8000 - 1;           // 1 ms tick
    TIM2->ARR = delay_ms;
    TIM2->CNT = 0;

    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->SR &= ~TIM_SR_UIF;

    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 |= TIM_CR1_CEN;
}

void setTimerPeriod(uint32_t new_period_ms) {
    timerPeriod_ms = new_period_ms;
    TIM2->ARR = timerPeriod_ms;
    TIM2->EGR |= TIM_EGR_UG;
}

uint32_t getTimerPeriod(void) {
    return timerPeriod_ms;
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;

        if (isOneShotMode) {
            if (oneShotCallback) oneShotCallback();
            oneShotCallback = 0;
            isOneShotMode = 0;
            TIM2->CR1 &= ~TIM_CR1_CEN; // Stop timer
        } else {
            if (periodicCallback) periodicCallback();
        }
    }
}
