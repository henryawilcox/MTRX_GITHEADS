/*
 * serial_interrupt.h
 *
 *  Created on: Apr 13, 2025
 *      Author: henrywilcox
 */

#ifndef SERIAL_INTERRUPT_H_
#define SERIAL_INTERRUPT_H_

void EnableSerialInterrupts(SerialPort *serial_port);

void USART1_EXTI25_IRQHandler(void);

#endif /* SERIAL_INTERRUPT_H_ */
