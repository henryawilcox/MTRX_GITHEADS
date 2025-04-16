/*
 * timer.c
 *
 *  Created on: Apr 15, 2025
 *      Author: henrywilcox
 */

#include "digital_io.h"
#include <stdint.h>
#include "stm32f303xc.h"
#include "timer.h"

// Timer variables
static volatile uint8_t isOneShotMode = 0;
static TimerCallback oneShotCallback = 0x00;

// Initialize timer (TIM2)
void TimerInitialise(void) {
    // Enable TIM2 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Configure TIM2 for a basic timer mode
    TIM2->CR1 = 0;           // Reset CR1
    TIM2->CR2 = 0;           // Reset CR2
    TIM2->DIER = 0;          // Reset interrupt enable register

    // Setup for 1ms tick
    // Assuming 8MHz system clock
    TIM2->PSC = 8000 - 1;    // Prescaler for 1ms tick (8MHz/8000 = 1kHz)

    // Enable TIM2 interrupt in NVIC
    NVIC_SetPriority(TIM2_IRQn, 2);  // Lower priority than button press
    NVIC_EnableIRQ(TIM2_IRQn);
}

// Start a one-shot timer
void Timer_StartOneShot(uint32_t delay_ms, TimerCallback cb) {
    isOneShotMode = 1;
    oneShotCallback = cb;

    TIM2->CR1 &= ~TIM_CR1_CEN;      // Stop timer before configuring
    TIM2->PSC = 8000 - 1;           // 1 ms tick
    TIM2->ARR = delay_ms;           // Set overflow time
    TIM2->CNT = 0;                  // Start counting from 0

    TIM2->DIER |= TIM_DIER_UIE;     // Enable update interrupt
    TIM2->EGR |= TIM_EGR_UG;        // Generate update event
    TIM2->SR &= ~TIM_SR_UIF;        // Clear any pending update interrupt

    TIM2->CR1 |= TIM_CR1_CEN;       // Start timer
}

// TIM2 interrupt handler
void TIM2_IRQHandler(void) {
    // Check if update interrupt occurred
    if (TIM2->SR & TIM_SR_UIF) {
        // Clear update interrupt flag
        TIM2->SR &= ~TIM_SR_UIF;

        // Stop timer if in one-shot mode
        if (isOneShotMode) {
            TIM2->CR1 &= ~TIM_CR1_CEN;  // Disable timer
            isOneShotMode = 0;          // Reset one-shot flag

            // Call callback if defined
            if (oneShotCallback != 0x00) {
                oneShotCallback();      // Execute callback function
                oneShotCallback = 0x00; // Reset callback
            }
        }
    }
}
