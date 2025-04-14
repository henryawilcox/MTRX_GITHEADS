#include "serial.h"

#include "stm32f303xc.h"
#include <stddef.h>



// We store the pointers to the GPIO and USART that are used
//  for a specific serial port. To add another serial port
//  you need to select the appropriate values.



// instantiate the serial port parameters
//   note: the complexity is hidden in the c file
SerialPort USART1_PORT = {USART1,
		GPIOC,
		RCC_APB2ENR_USART1EN, // bit to enable for APB2 bus
		0x00,	// bit to enable for APB1 bus
		RCC_AHBENR_GPIOCEN, // bit to enable for AHB bus
		0xA00,
		0xF00,
		0x770000,  // for USART1 PC10 and 11, this is in the AFR low register
		0x00, // no change to the high alternate function register
		0x00, //buffer intialised at 0
		0x00 // default function pointer is NULL
		};



// InitialiseSerial - Initialise the serial port
// Input: baudRate is from an enumerated set
void SerialInitialise(BaudRate baudRate, SerialPort *serial_port, void (*callback)(uint8_t *data, uint8_t length)) {

	serial_port->completion_function = callback; //callback to function

    // enable clock power, system configuration clock and GPIOC
    // common to all UARTs
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // enable the GPIO which is on the AHB bus
    RCC->AHBENR |= serial_port->MaskAHBENR;

    // set pin mode to alternate function for the specific GPIO pins
    serial_port->GPIO->MODER = serial_port->SerialPinModeValue;

    // enable high speed clock for specific GPIO pins
    serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;

    // set alternate function to enable USART to external pins
    serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
    serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;

    // enable the device based on the bits defined in the serial port definition
    RCC->APB1ENR |= serial_port->MaskAPB1ENR;
    RCC->APB2ENR |= serial_port->MaskAPB2ENR;

    // Get a pointer to the 16 bits of the BRR register that we want to change
    uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR; // only 16 bits used!

    // Baud rate calculation from datasheet
    switch(baudRate){
    case BAUD_9600:
        *baud_rate_config = 0x341;  // 9600 at 8MHz
        break;
    case BAUD_19200:
        *baud_rate_config = 0x1A0;  // 19200 at 8MHz
        break;
    case BAUD_38400:
        *baud_rate_config = 0xD0;   // 38400 at 8MHz
        break;
    case BAUD_57600:
        *baud_rate_config = 0x8B;   // 57600 at 8MHz
        break;
    case BAUD_115200:
        *baud_rate_config = 0x46;   // 115200 at 8MHz
        break;
    }

    // enable serial port for tx and rx
    serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void SerialOutputChar(uint8_t data, SerialPort *serial_port) {
    // Wait until the transmit data register is empty
    while((serial_port->UART->ISR & USART_ISR_TXE) == 0) {
        // Do nothing, just wait
    }

    // Send the data
    serial_port->UART->TDR = data;
}

void SerialOutputString(uint8_t *pt, SerialPort *serial_port) {
    uint32_t counter = 0;
    while(*pt) {
        SerialOutputChar(*pt, serial_port);
        counter++;
        pt++;
    }
}


// returns 1 if valid char, 0 if timeout
uint8_t SerialReceiveChar(SerialPort *serial_port, uint8_t *received_char) {
    uint16_t timeout = 0xffff;

    while (1) {
        timeout--;
        if (timeout == 0)
            return 0;

        // Check for overrun or framing error
        if (serial_port->UART->ISR & USART_ISR_ORE || serial_port->UART->ISR & USART_ISR_FE) {
            serial_port->UART->ICR |= USART_ICR_ORECF | USART_ICR_FECF;
        }

        // Check if data is available to read
        if (serial_port->UART->ISR & USART_ISR_RXNE) {
            break;
        }
    }

    // Read the received data
    *received_char = serial_port->UART->RDR;
    return 1;
}

// Read a string from serial port into buffer until terminating character (CR or LF)
uint16_t SerialInputString(SerialPort *serial_port) {
    uint8_t receivedChar;
    uint16_t index = 0;

    // Clear buffer first
    for (int i = 0; i < BUFFER_SIZE; i++) {
        serial_port->rx_buffer[i] = 0;
    }

    // Read characters until enter key or buffer full
    while (index < BUFFER_SIZE - 1) {
        if (SerialReceiveChar(serial_port, &receivedChar)) {


            // Store character in buffer
            serial_port->rx_buffer[index++] = receivedChar;

            // Break if CR or LF (terminating characters)
            if (receivedChar == '\r' || receivedChar == '\n') {
                break;
            }
        }
    }

    // Null-terminate the string
    serial_port->rx_buffer[index] = '\0';

    // Null-terminate string and strip '\r' if needed
    uint8_t clean_length = index;
    if (index > 0 && (serial_port->rx_buffer[index - 1] == '\r' || serial_port->rx_buffer[index - 1] == '\n')) {
        clean_length--;
        serial_port->rx_buffer[clean_length] = '\0';  // Replace terminator with '\0'
    } else {
        serial_port->rx_buffer[clean_length] = '\0';  // Add null at end
    }

    // Call the completion function if it exists
    if (serial_port->completion_function != NULL) {
        serial_port->completion_function(serial_port->rx_buffer, clean_length);
    }

    return index;
}
