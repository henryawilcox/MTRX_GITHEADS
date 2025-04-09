#ifndef DIGITAL_H_
#define DIGITAL_H_

#include <stdint.h>

/**
 * Initialize digital I/O module with a callback for button press
 * on_button_press Function to call when button is pressed
 */
void DigitalInitialise(void (*callback)());

#endif /* DIGITAL_H_ */
