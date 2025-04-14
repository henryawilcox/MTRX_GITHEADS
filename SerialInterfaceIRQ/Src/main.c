

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "serial.h"              // UART setup and transmit functions
#include "stm32f303xc.h"         // Device-specific header for STM32F3 series
#include "serial_interrupt.h"    // Interrupt-based RX handler


// This function is called when a complete string is received over UART
void completion_callback(uint8_t *string_buffer, uint8_t length) {
    // Display the received message
    SerialOutputString((uint8_t*)"You entered: ", &USART1_PORT);
    SerialOutputString(string_buffer, &USART1_PORT);
    SerialOutputString((uint8_t*)"\rCharacters received: ", &USART1_PORT);

    // Convert the received character count to a string and print it
    char num_str[10];
    sprintf(num_str, "%d", length);
    SerialOutputString((uint8_t*)num_str, &USART1_PORT);
    SerialOutputString((uint8_t*)"\r\n\r\n", &USART1_PORT);

    // Prompt the user to enter new input
    SerialOutputString((uint8_t*)"Enter text (RETURN to terminate):\r\n", &USART1_PORT);
}

int main(void) {
    // Initialize the serial port with baud rate 115200 and a pointer to the callback
    SerialInitialise(BAUD_115200, &USART1_PORT, &completion_callback);

    // Send welcome messages over UART
    SerialOutputString((uint8_t*)"UART ACTIVATED\r\n", &USART1_PORT);
    SerialOutputString((uint8_t*)"Enter text (RETURN to terminate):\r\n", &USART1_PORT);

    // Enable RX interrupt-based input
    EnableSerialInterrupts(&USART1_PORT);

    // Infinite loop: main code does not need to do anything here
    // since everything is handled through interrupts
    for(;;) {

    }
}
