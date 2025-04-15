#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "serial.h"
#include "digital_io.h"
#include "serial_interrupt.h"
#include "stm32f303xc.h"

//int is_number(const char *str) {
//    if (str == NULL || *str == '\0') return 0; // Empty or NULL string
//
//    while (*str) {
//        if (*str < '0' || *str > '9') {
//            return 0; // Found a non-digit character
//        }
//        str++;
//    }
//    return 1; // All characters were digits
//}

void completion_callback(uint8_t *string_buffer, uint8_t length) {
    // Copy to modifiable buffer
    char buffer[BUFFER_SIZE];
    strncpy(buffer, (char*)string_buffer, BUFFER_SIZE);
    buffer[BUFFER_SIZE - 1] = '\0';

    // Split into operation and operand
    char *operation = strtok(buffer, " ");
    char *operand = strtok(NULL, "\r\n");

    if (operation == NULL || operand == NULL){
        SerialOutputString((uint8_t*)"Invalid input.\r\n", &USART1_PORT);
        return;
    }

    if (strcmp(operation, "led") == 0) {
        SerialOutputString((uint8_t*)"You selected led operation\r\n", &USART1_PORT);

        // Check if length is exactly 8
        if (strlen(operand) != 8) {
            SerialOutputString((uint8_t*)"Invalid input.\r\n\r\n", &USART1_PORT);
            return;
        }

        // Check if only '0' and '1'
        for (int i = 0; i < 8; i++) {
            if (operand[i] != '0' && operand[i] != '1') {
                SerialOutputString((uint8_t*)"Invalid input.\r\n", &USART1_PORT);
                return;
            }
        }

        // If valid, convert to bitmask and set
        uint8_t bitmask = (uint8_t)strtol(operand, NULL, 2);
        SetLEDBitmask(bitmask);
        SerialOutputString((uint8_t*)"LED pattern set.\r\n", &USART1_PORT);
    }
//
//    else if (strcmp(operation, "serial") == 0) {
//        SerialOutputString((uint8_t*)"You selected serial operation\r\n", &USART1_PORT);
//
//        // Echo the message
//        SerialOutputString((uint8_t*)"Echo: ", &USART1_PORT);
//        SerialOutputString((uint8_t*)operand, &USART1_PORT);
//        SerialOutputString((uint8_t*)"\r\n", &USART1_PORT);
//    }
//
//    else if (strcmp(operation, "oneshot") == 0) {
//    	SerialOutputString((uint8_t*)"You selected oneshot operation\r\n", &USART1_PORT);
//    	if (is_number(operand)) {
//    		//RUN ONESHOT FUNCTION
//    	}
//        // TODO: add operand validation and timer call
//    }
//
//    else if (strcmp(operation, "timer") == 0) {
//        SerialOutputString((uint8_t*)"You selected timer operation\r\n", &USART1_PORT);
//        if (is_number(operand)) {
//			//RUN ONESHOT FUNCTION
//		}
//
//    }
//
//    else {
//        SerialOutputString((uint8_t*)"Invalid command.\r\n", &USART1_PORT);
//    }
//
//    // Show how many characters were received
//    char num_str[10];
//    sprintf(num_str, "%d", length);
//    SerialOutputString((uint8_t*)num_str, &USART1_PORT);
//    SerialOutputString((uint8_t*)"\r\n\r\n", &USART1_PORT);
//
//    // Prompt for next input
//    SerialOutputString((uint8_t*)"Enter text (RETURN to terminate):\r\n", &USART1_PORT);
}



int main(void) {
    // Initialize the serial port with baud rate 115200 and a pointer to the callback
    SerialInitialise(BAUD_115200, &USART1_PORT, &completion_callback);
    DigitalInitialise(&completion_callback);
    //timer functions ??

    // Send welcome messages over UART
    SerialOutputString((uint8_t*)"UART ACTIVATED\r\n", &USART1_PORT);
    SerialOutputString((uint8_t*)"Enter an operation and operand (RETURN to terminate):\r\n", &USART1_PORT);

    // Enable RX interrupt-based input
    EnableSerialInterrupts(&USART1_PORT);

    // Infinite loop: main code does not need to do anything here
    // since everything is handled through interrupts
    for(;;) {

    }
}
