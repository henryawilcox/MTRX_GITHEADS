# MTRX_GITHEADS
Group 10 GITHEADS
Rory Clements
Henry Atticus Wilcox
Emily Stockwell

# Roles and Responsibilities
Henry: Digital IO and Serial Communication
Rory: Hardware Timers and Team Documentation
Emily: Integration

# Project Overview
The goal of the project is to complete three modules (Digital IO, Serial Interface, Hardware Timers), and then combine them together to achieve the integration task as outlined:
Using the modules developed already (without making any changes to the modules), develop a program that can take in a string from the serial port. This string should be parsed to extract the desired behaviour. The first part of the string should be text to indicate the desired operation, the second part of the string should be a value to use as an operand. The parser should be able to distinguish between four different commands sent over serial. For a serial command to turn on the LEDs in a certain pattern, the command should be similar to: "led 10001010" The operand is a binary string that turns on/off the leds in this pattern. For a serial command to cause the message to be sent back through the serial port, the command should be similar to "serial This is a string that should be written back to the serial port" the operand is a string that should be transmitted over the serial. For a serial command to trigger a one-shot timer action, the command should be similar to: "oneshot 1000" the operand is a number that is the number of milliseconds for the one shot timer to run. For a serial command to trigger a continuous timer action, the command should be similar to: "timer 1000" the operand is a number that is the number of milliseconds for the timer period.

# Digital IO

## Summary
The DigitalIO project interfaces with the LEDs and push button on the microcontroller. It allows the user to change the state of the leds using a button press, updating the bitmask displayed by the LEDS. The project also incorporates a timer module that enables one-shot timing operations, allowing for controlled timing between LED state changes.

## Usage
Upload the code to your STM32F303 discovery board.
The board will initialize with all LEDs off.
Press the user button (blue button) to advance the LED pattern - a single LED will move across the board in a "chase" pattern.

(if chase_led_cooldown is the callback function) It prevents multiple button presses from being processed too quickly (1 second cooldown).

## Valid Input
Button press: Single press on the user button triggers the chase_led or chase_led_cooldown() function.
(Timing is handled internally with no external input required)

## Functions and Modularity
### digital_io.h
- Defines core LED and button interface functions:
- 	DigitalInitialise(): Initializes digital I/O with a callback for button press
- 	GetLEDBitmask(): Returns the current LED pattern
- 	SetLEDBitmask(): Sets LEDs according to an 8-bit mask

### digital_io.c

Contains implementation of LED control and button interrupt handling:

- GPIO Initialization: Configures GPIOE pins PE8-PE15 as outputs for LEDs
- Interrupt Setup: Configures PA0 (user button) with rising edge detection
- Button Handler: EXTI0_IRQHandler() executes the registered callback when button is pressed, as an interrupt
- LED Control: SetLEDBitmask() and GetLEDBitmask() manage LED states via a bitmask



### timer.h and timer.c

- TimerInitialise(): Sets up TIM2 for timing operations
- Timer_StartOneShot(): Starts a one-shot timer with specified delay and callback
- Interrupt Handler: TIM2_IRQHandler() executes callbacks when timer completes



### main.c
Main usage of the DigitalIO module:

- Initalises digital IO, and calls callback to either chase_led or chase_led_cooldown

- chase_led
- 	chases LED around in a ciruclar pattern immediatly upon button press

- chase_led_cooldown
- 	calls oneshot timer to only allow a button press every 1s


## Testing

| Test Case                        | Expected Output                        | Importance    |
|----------------------------------|----------------------------------------|----------------------------------------|
| Press button once             | Single LED advances position        |Validates input and output of software |
| Press button repeatedly (chase_led as callback)          | Single LED advances immiediatly with each press         |Confirms chase_led has no delay |
| Press button repeatedly (chase_led_cooldown as callback)      | Single LED advances only after 1s from last press   | confirms chase_led_cooldown correctly implemented timer|

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
	- Echoes back the full string
	- 	Reports the number of characters received
	- 	Prompts for the next input

### `serial.h`

- Defines constants and types:
	- 	`BUFFER_SIZE` – sets the size of the receive buffer
	- 	`BaudRate` –  for supported baud rate values
	- 	`SerialPort` – structure for USART port configuration and buffers
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
The SerialInterfaceIRQ project is the second part of Exercise 2: Serial Interface. It builds upon the previous polling-based implementation by using interrupts to handle incoming characters more efficiently. When characters arrive via UART1, an interrupt service routine (ISR) is triggered that reads the character, stores it in a buffer, and echoes it back to the terminal. Once a terminating character (carriage return \r or newline \n) is detected, a callback function prints the complete string and reports the character count, similar to the polling-based version. This interrupt-driven approach allows the main program to perform other tasks while waiting for serial input, improving overall system responsiveness.

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
### main.c
- Initializes the serial interface using SerialInitialise().
- Sets up the completion callback function completion_callback().
- Enables interrupt-based reception with EnableSerialInterrupts().
- Main loop remains empty as processing happens in the interrupt context.

### serial_interrupt.c

- EnableSerialInterrupts(): Configures the USART1 peripheral for interrupt-driven operation by enabling the RXNE (Receive Not Empty) interrupt and setting up the NVIC
- USART1_EXTI25_IRQHandler(): The interrupt service routine that handles:
	- Character reception, echoing, and buffer storage
	- End-of-line detection and callback triggering
	- Buffer overflow protection
	- Transmit buffer management for non-blocking output

### serial.c

Contains the core serial configuration functions similar to the polling version:
- SerialInitialise(): Sets up the USART peripheral, GPIO pins, and baud rate.
- SerialOutputChar(): Implementation now uses a circular buffer for non-blocking transmission.
- SerialOutputString(): Sends a null-terminated string using the non-blocking method.

## Testing
## Testing
| Test Case                        | Expected Output                        | Importance    |
|----------------------------------|----------------------------------------|----------------------------------------|
| `hello` + `Enter`               | Echoes back "hello", count = 6         |Checks for echoing and basic functionality. |
| `ABC123!@#` + `Enter`           | Echoes full string, count = 10          |Confirms handling of mixed ASCII characters. |
| Empty input + `Enter`           | Echoes blank, count = 1               | Edge case for no input (empty string).|
| 254 characters + `Enter`        | Echoes up to limit, count = 255        |Ensures buffer boundary is respected.|
| Exceed buffer then `Enter`      | Echoes up to limit, count = 255, then after next request for info, it echoes the remaining chars and the overflow count             |Validates robustness against buffer overflows.|


# Hardware Timer Interface
## Summary
The objective of the Hardware Timers section is tri-fold. Firstly, a module to implement a periodic timer is required. It should be able to be triggered when called by a function callback. Secondly, the function should be able to reset the timer with a new period. The period should be able to be set without global definition. Get set functions can be used in this case. Finally, a one-shot timer should be able to be triggered in a similar fashion, where a light will flash once and indicate the conclusion of the one-shot.

## Usage
1. Open the timer.c file
2. Since only one external interrupt handler for a button can be used at any one time, select the handler that corresponds to the desired behaviour (one-shot OR periodic timing)
3. Edit the delay variable (unint32_t delay) in the respective function:
   Timer_StartOneShot for One Shot wait-time
   Timer_Init for Periodic Timer period
   corresponding to the period in milliseconds. (1000ms = 1s)
5. Flash the code to your STM32F303 discovery board.
6. Press the button to trigger interrupt and enable the timing (oneshot will wait the delay time and then flash once, remaining off until the button is pressed again to trigger another oneshot) (periodic will start off for the period time, then flash on and hold for the period time, then turn off for the period time and repeat)

## Valid Input
No direct inputs in serial. However, delay must be of the size 32 bits (or less). Make sure the desired handler is uncommented in the timer.c and that both handlers are not uncommented at run time.

## Functions and Modularity
All functions are included in timer.c.
Timer_Init = Initialises the timer for periodic timing, and internally defines a period and sets it using setTimerPeriod
Timer_Start = Begins the timer counting by setting the control register to on.
Timer_StartOneShot = Initialises the timer for a one shot configuration, and internally defines a period and sets it using setTimerPeriod
setTimerPeriod = Takes in a integer as argument and sets the overflow ARR value to that number
getTimerPeriod = Retrieves and returns the ARR value
TIM2_IRQHandler = Checks to see if the Timer is set to oneshot or periodic configuration, then calls the appropriate GPIO behaviours with periodicBlink OR oneShotBlink
periodicBlink = Begins periodic flashing of LEDs in the bit mask 10101010
oneShotBlink = Triggers a one shot timer that flashes all LEDs in the bit mask 11111111 at the conclusion of the oneshot timing.
enable_clocks = Enables the clocks (from Tutorial)
initialise_board = Initialises the board (from Tutorial)
EXTI0_IRQHandler = Handles the external interrupt triggered by the user button press and uses it to initiate either timer configurations and calls.

# Integration Interface
## Summary

Integration demonstrates each of the components of the lab: **Digital IO** (LED bitmask control), **Serial Interfacing** (UART receive/transmit) and **Timer Interfacing** (one-shot and periodic actions). 

It listens for an interrupt that indicates serial input. When it receives a string with an operation and an operand, it applies an 'if-else' sequence for each operation. It applies input validity checks then calls on the functions from each individual module.

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
3. When prompted, type a command of one of the following forms and press `Enter`:
- `led [8-digit binary]` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ Sets LEDs to a binary pattern
- `serial [string]` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ Echoes the string
- `oneshot [integer]` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ Waits delay of [integer] and then flashes LEDs  
- `timer [integer]` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;→ Starts flashing LEDs with delay of [integer]


After each command, a response is printed over UART and the program prompts for the next input.


## Valid Input
| Command    | Operand Type | Format                     | Notes                                 |
|------------|--------------|----------------------------|----------------------------------------|
| `led`      | Binary String| Exactly 8 characters (0/1) | Controls LED bitmask output           |
| `serial`   | Any string    | Max length 254 chars       | Echoed back to user                   |
| `oneshot`  | Integer       | Positive number (ms)       | One-time delay               |
| `timer`    | Integer       | Positive number (ms)       | Repeated delay               |

Invalid inputs (e.g. wrong formats, missing operands, too long) are caught and reported with `"Invalid input"` or `"Invalid command"` messages.


## Functions and Modularity

This project combines the program files from each of the other projects, including:
- `Digital_io.c`
- `Serial_interrupt.c`
- `Serial.c`
- `Timer.c`
  
As well as each of their respective header files. 

The `main.c` file consists of the following functions and their calls to functions in each project file:
- **`main()`**  
  Initializes serial communication, sets up digital and timer subsystems, and enables RX interrupts then continues in infinite loop as it waits for an interrupt.

- **`completion_callback()`**  
  Triggered when a complete input string is received. Parses and validates the input, then:
  - Stops the timer if it is running then routes to `SetLEDBitmask()` in `digital_io.c` for the `led` operation
  - Echoes back strings for the `serial` operation
  - Calls `oneshot_action()` from `timer.c` for delayed output
  - Calls `periodic_action()` from `timer.c` for repeated timer behaviour

- **`is_number()`**  
  Utility function that checks if a string contains only numeric digits.


## Testing
| Input Command         | Expected Behaviour                               | Importance                             |
|-----------------------|--------------------------------------------------|---------------------------------------------|
| `led 10101010`        | LEDs set in 0b10101010 pattern                   | Tests binary parsing and LED control        |
| `led 10101`           | "Invalid input." message                         | Validates input length                      |
| `serial hello world`  | Echoes "hello world"                             | Verifies full string echoing over UART      |
| `oneshot 1000`        | Executes action after ~1s delay                  | Confirms numeric parsing and timing         |
| `timer 250`           | Starts periodic blinking switching every .25 seconds                        | Validates looped timer behaviour             |
| `timer abc`           | "Invalid input." message                         | Catches non-numeric operand                 |
| `MTRX 2700`             | "Invalid command." message                       | Ensures invalid commands are rejected       |

### Additional Checks
It is also important to check that the running of consecutive commands works as desired.

If you are running the timer operation:
- 	If you call the `oneshot` operation, it should turn the LEDs off for the delay period, flash, then stay off. This shows the timer from the previous command has been stopped. It also should not change if the oneshot function is called when the timer flashes are on or off.
- 	If the `led` operation is called, it should change to the provided bitmask and stay that way.
 

