#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include "stm32f303xc.h"

// Define buffer size for input string
#define BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256

// Define baud rate options
typedef enum {
    BAUD_9600,
    BAUD_19200,
    BAUD_38400,
    BAUD_57600,
    BAUD_115200
} BaudRate;

// Forward declaration of the SerialPort structure
typedef struct _SerialPort SerialPort;

// Function prototypes
void SerialInitialise(BaudRate baudRate, SerialPort *serial_port, void (*callback)(uint8_t *data, uint8_t length));

void SerialOutputChar(uint8_t data, SerialPort *serial_port);
void SerialOutputString(uint8_t *pt, SerialPort *serial_port);
uint8_t SerialReceiveChar(SerialPort *serial_port, uint8_t *received_char);
uint16_t SerialInputString(SerialPort *serial_port);

// Define the SerialPort structure
struct _SerialPort {
    USART_TypeDef *UART;
    GPIO_TypeDef *GPIO;
    volatile uint32_t MaskAPB2ENR;    // mask to enable RCC APB2 bus registers
    volatile uint32_t MaskAPB1ENR;    // mask to enable RCC APB1 bus registers
    volatile uint32_t MaskAHBENR;     // mask to enable RCC AHB bus registers
    volatile uint32_t SerialPinModeValue;
    volatile uint32_t SerialPinSpeedValue;
    volatile uint32_t SerialPinAlternatePinValueLow;
    volatile uint32_t SerialPinAlternatePinValueHigh;
    void (*completion_function)(uint8_t*, uint8_t);  // Changed to match main.c

    volatile uint16_t tx_head;
    volatile uint16_t tx_tail;
    volatile uint8_t tx_busy;
    volatile uint8_t tx_buffer[TX_BUFFER_SIZE]; // buffer for outgoing data
};

// Declare the serial port to be used
extern SerialPort USART1_PORT;

#endif // SERIAL_H
