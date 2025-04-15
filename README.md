# MTRX_GITHEADS
Group 10 GITHEADS
Rory Clements
Henry Atticus Wilcox
Emily Stockwell

# Roles and Responsibilities

# Project Overview


# Digital IO

## Summary
**bolding**

## Usage


## Valid Input


## Functions and Modularity

## Testing
# Serial Interface
## Summary
The SerialInterface project is the first part of Exercise 2: Serial Interface. It interfaces with UART1 of the STM32F3Discovery microcontroller to allow the module to read incoming characters into a memory buffer until a terminating character (a carriage return (`\r`) or newline (`\n`) in this case) is received. It then triggers a callback function that prints the string back onto the interface and the number of characters received, including the terminating character. It uses a blocking, polling-based approach to receive characters one at a time and process full messages when a terminator is detected.

## Usage
1. Flash the code to your STM32F303 discovery board.
2. Connect to the board using a serial terminal (e.g., PuTTY (Windows), Cutecom (Mac)) with the following settings:
	- **Baud rate:** 115200
	- **Data bits:** 8
	- **Stop bits:** 1
	- **Parity:** None
	- **Flow control:** None
	- **Line discipline options:** Local echo ON and (optional) Local line editing ON
	- Implicit CR in every LF
	- Implicit LF in every CR 
	- Note: Ensure the Port number is the one you have the STM32 plugged into
3. When prompted, type a message and press `Enter`. The board will:
   - Print the full string once the line ends
   - Report the total character count 

## Valid Input
- Accepts any printable ASCII characters.
- Input is terminated when a carriage return (`\r`) or newline (`\n`) is detected.
- The input buffer is limited to 255 characters (defined by `BUFFER_SIZE`).
- Characters beyond the buffer size will be ignored or discarded.


## Functions and Modularity

### `main.c`

- Initializes the serial interface using `SerialInitialise()`.
- Polls to check for inputs using 'SerialInputString()'.
- Defines a **callback function** `completion_callback()` which:
- 	Echoes back the full string
- 	Reports the number of characters received
- 	Prompts for the next input

### `serial.h`

- Defines constants and types:
- 	`BUFFER_SIZE` – sets the size of the receive buffer
- 	'BaudRate` –  for supported baud rate values
- 	'SerialPort` – structure for USART port configuration and buffers
- Declares the core serial communication functions used throughout the program.

### `serial.c`

- Contains the implementation of all UART functions:
- 	**SerialInitialise():** Configures the USART peripheral and relevant GPIO pins, sets baud rate, and enables transmission and reception.
- 	**SerialOutputChar():** Waits until the transmit buffer of the UART is ready, then sends one character.
- 	**SerialOutputString():** Sends a full null-terminated string over UART, character by character using `SerialOutputChar()`.
- 	**SerialReceiveChar():** Waits for a character to be received, handling timeout and overrun errors. Returns 1 if character is received successfully.
- 	**SerialInputString():** Reads characters until `\r` or `\n` is received, stores them in the `rx_buffer`, echoes them one by one, and then triggers the callback. Returns the number of characters received (excluding null terminator).


## Testing
| Test Case                        | Expected Output                        | Importance    |
|----------------------------------|----------------------------------------|----------------------------------------|
| `hello` + `Enter`               | Echoes back "hello", count = 6         |Checks for echoing and basic functionality. |
| `ABC123!@#` + `Enter`           | Echoes full string, count = 10          |Confirms handling of mixed ASCII characters. |
| Empty input + `Enter`           | Echoes blank, count = 1               | Edge case for no input (empty string).|
| 254 characters + `Enter`        | Echoes up to limit, count = 255        |Ensures buffer boundary is respected.|
| Exceed buffer then `Enter`      | Echoes up to limit, count = 255, then after next request for info, it echoes the remaining chars and the overflow count             |Validates robustness against buffer overflows.|


# Serial Interface IRQ
## Summary
**bolding**

## Usage


## Valid Input


## Functions and Modularity

## Testing

# Timer Interface
## Summary
**bolding**

## Usage


## Valid Input


## Functions and Modularity

## Testing

# Integration Interface
## Summary
**bolding**

## Usage


## Valid Input


## Functions and Modularity

## Testing
