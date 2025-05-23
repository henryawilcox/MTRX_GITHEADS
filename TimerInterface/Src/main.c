/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <timer.h>
#include "stm32f303xc.h"


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


void timer_speed(void)
{
	uint8_t led_mask_pattern = 0b01010101;
	uint8_t *led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;
	uint32_t on_time = 0x80000;

	// Get period
	uint32_t off_time = GetPeriod();


	TIM2->CNT = 0;

	// turn on the LEDs
	*led_output_register = led_mask_pattern;

	while (TIM2->CNT < on_time) {} ; // pwm loop for on time

	// turn off the LEDs
	*led_output_register = 0x00;

	while (TIM2->CNT < off_time) {}; // pwm loop for off time


	// Update period
	SetPeriod(off_time);


}


int main(void)
{

	TimerInitialise(&timer_speed);

	/* Loop forever */
	for(;;);

}
