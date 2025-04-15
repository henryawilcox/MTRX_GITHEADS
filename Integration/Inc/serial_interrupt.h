/*
 * serial_interrupt.h
 *
 *  Created on: Apr 13, 2025
 *      Author: henrywilcox
 */

#ifndef SERIAL_INTERRUPT_H_
#define SERIAL_INTERRUPT_H_

/**
 * Configures and enables UART interrupts for the specified serial port
 * serial_port Pointer to the serial port structure to configure
 */
void EnableSerialInterrupts(SerialPort *serial_port);

/**
 * USART1 interrupt handler
 * Processes received data and transmission events for USART1
 * Called automatically when USART1 triggers an interrupt
 */
void USART1_EXTI25_IRQHandler(void);
#endif /* SERIAL_INTERRUPT_H_ */
