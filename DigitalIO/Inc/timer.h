/*
 * timer.h
 *
 *  Created on: Apr 15, 2025
 *      Author: henrywilcox
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "digital_io.h"

// Function pointer type for timer callbacks
typedef void (*TimerCallback)(void);

// Initialize digital I/O
void DigitalInitialise(void (*callback)());

// LED functions
uint8_t GetLEDBitmask();
void SetLEDBitmask(uint8_t mask);

// Timer functions
void TimerInitialise(void);
void Timer_StartOneShot(uint32_t delay_ms, TimerCallback cb);


#endif /* TIMER_H_ */
