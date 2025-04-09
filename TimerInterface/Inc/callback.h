#ifndef _CALLBACK_H
#define _CALLBACK_H

void EXTI0_IRQHandler(void);
void enable_interrupt();
void enable_clocks();
void initialise_board();
void trigger_prescaler();
uint32_t GetPeriod();
void SetPeriod(uint32_t period);
void TimerInitialise(void (*callback)());

#endif
