/*
 * serial_interrupt.c
 *
 *  Created on: Apr 13, 2025
 *      Author: henrywilcox
 */

#include "serial.h"
#include "serial_interrupt.h"
#include "stm32f303xc.h"

// Flag to indicate if a complete string has been received
volatile uint8_t string_recieved;

// Index for storing characters in the RX buffer
volatile uint8_t rx_index = 0;

// Buffers for receiving data
// rx_buffer1 is filled live during character reception
// rx_buffer2 is a safe copy handed off to the main loop
uint8_t rx_buffer1[BUFFER_SIZE];
uint8_t rx_buffer2[BUFFER_SIZE];

void EnableSerialInterrupts(SerialPort *serial_port) {
    // Disable global interrupts to prevent issues during setup
    __disable_irq();

    // Reset flags and index
    rx_index = 0;
    string_recieved = 0;

    // Clear both RX buffers
    for (int i = 0; i < BUFFER_SIZE; i++) {
        rx_buffer1[i] = 0;
        rx_buffer2[i] = 0;
    }

    // Enable Receive Not Empty interrupt (triggers when data arrives)
    serial_port->UART->CR1 |= USART_CR1_RXNEIE;

    // Enable USART1 interrupt in the NVIC
    NVIC_EnableIRQ(USART1_IRQn);              // Be sure your startup file maps this correctly
    NVIC_SetPriority(USART1_IRQn, 0);         // Highest priority (lowest numerical value)

    // Re-enable global interrupts
    __enable_irq();
}

// Interrupt Service Routine for USART1
void USART1_EXTI25_IRQHandler(void) {
    // ---------------------------
    // RX SECTION
    // ---------------------------
    // Check if RXNE (Receive Data Register Not Empty) is set
    if (USART1_PORT.UART->ISR & USART_ISR_RXNE) {
        // Read the incoming character
        uint8_t received_char = USART1_PORT.UART->RDR;

        // Echo the character back for user feedback
        SerialOutputChar(received_char, &USART1_PORT);

        // Only process if there's space in the buffer
        if (rx_index < BUFFER_SIZE - 1) {
            // Store received character in live RX buffer
            rx_buffer1[rx_index++] = received_char;

            // Check for end-of-input character (ENTER/RETURN key)
            if (received_char == '\r' || received_char == '\n') {
                // Null-terminate the string
                rx_buffer1[rx_index] = '\0';
                string_recieved = 1;

                // Copy buffer1 to buffer2 (safe for processing outside ISR)
                for (int i = 0; i < BUFFER_SIZE; i++) {
                    rx_buffer2[i] = rx_buffer1[i];
                    rx_buffer1[i] = 0; // Clear buffer1 for next message
                }

                // Trigger the completion callback with the full message
                USART1_PORT.completion_function(rx_buffer2, rx_index);

                // Reset index for next message
                rx_index = 0;
            }
        } else {
            // If buffer overflows, clear and notify
            rx_index = 0;
            uint8_t BUFFEROVERFLOW[BUFFER_SIZE] = "ERROR: BUFFER OVERFLOW";
            USART1_PORT.completion_function(BUFFEROVERFLOW, rx_index);
        }
    }

    // ---------------------------
    // TX SECTION
    // ---------------------------
    // Check if TXE (Transmit Data Register Empty) interrupt is enabled and pending
    if ((USART1->CR1 & USART_CR1_TXEIE) && (USART1->ISR & USART_ISR_TXE)) {
        // Check if there is data left to transmit in the circular buffer
        if (USART1_PORT.tx_tail != USART1_PORT.tx_head) {
            // Send next character from the TX buffer
            USART1->TDR = USART1_PORT.tx_buffer[USART1_PORT.tx_tail];
            USART1_PORT.tx_tail = (USART1_PORT.tx_tail + 1) % TX_BUFFER_SIZE;
        } else {
            // Transmission is complete — disable TXE interrupt
            USART1->CR1 &= ~USART_CR1_TXEIE;
            USART1_PORT.tx_busy = 0;
        }
    }
}
