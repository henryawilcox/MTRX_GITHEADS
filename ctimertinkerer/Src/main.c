// THIS FILE CAN BE USED TO DEMONSTRATE THE FUNCTIONALITY OF THE HARDWARE TIMERS USING AN EXTERNAL INTERRUPT TRIGGERED BY A USER BUTTON
// THERE ARE TWO OPTIONS: ONESHOT OR PERIODIC TIMING AFTER BUTTON PRESS
// THE SELECTED OPTION MUST BE UNCOMMENTED IN TIMER.C BEFORE RUNNING

#include <stdint.h>
#include "stm32f303xc.h"
#include "timer.h"

void enable_clocks(void);
void initialise_board(void);

int main(void) {
    enable_clocks(); //enabling clocks
    initialise_board(); //enabling all other ports and board

    while (1) {
        // Main loop does nothing; logic handled in interrupts
    }
}
