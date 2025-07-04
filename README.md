# String Mirroring & DMA Block Transfer Project

## Overview

This project implements **idiom recording**, **string mirroring using DMA block transfer**, and **LED pattern display** functionalities on an **MSP430 microcontroller platform**. It demonstrates:

- String input via keypad with immediate character display
- String mirroring using DMA to reverse character arrays efficiently
- DMA-based LED pattern transfer
- DMA-based string merging between two idioms into a single output buffer

---

## File Structure

- **main.c**  
  Initializes system configuration and manages the main finite state machine loop to switch between functionalities: idle, idiom record, mirror string, LED display, and string merging.

- **api.c**  
  Implements idiom recording with keypad input, string mirroring via DMA block transfer, array printing using DMA for LEDs, and merging of two strings using DMA.

- **bsp.c**  
  Configures GPIO, Timer A (1s), and Timer B (500ms) for the system, as well as keypad and push button interrupt setups.

- **halGPIO.c**  
  Provides low-level drivers for keypad input processing, LCD display control, DMA string mirror logic, and interrupt service routines (ISR) for timers, DMA, push buttons, and keypad scanning.

---

## Key Functionalities

### 1. Idiom Recording

- Records idioms typed via keypad, with each key mapped to multiple characters (similar to T9 input)
- Handles character overwrite on multiple presses within a 1-second window
- Displays immediate visual feedback on the LCD

### 2. String Mirroring

- Uses DMA to mirror (reverse) strings stored in the `data_base` idiom array efficiently
- The mirrored string is printed back to the LCD

### 3. LED Pattern Display

- Uses DMA block transfer to send predefined LED patterns (`led_data`) to Port 9 for display with Timer B triggering

### 4. String Merging

- Merges two idioms (`str1` and `str2`) using two separate DMA channels into a destination buffer and displays the merged result

---

## Hardware Requirements

- MSP430 microcontroller
- 4x4 Keypad
- Character LCD display (compatible with project lcd.c driver)
- LED array connected to Port 9

---

## Setup and Usage

### Build

- Include all `.c` and corresponding `header/*.h` files in your Code Composer Studio (CCS) project
- Configure the MSP430 hardware connections according to pin assignments in `bsp.c` and `halGPIO.c`

### Run

- On boot, the device enters idle mode awaiting push button input to trigger FSM states:
  - **PB0:** Record idiom
  - **PB1:** Mirror idiom from database
  - **PB2:** Display LED patterns
  - **PB3:** Merge two strings

### Controls

- Use the keypad to input idioms or select idiom indices for mirroring
- LCD provides prompts and displays outputs accordingly

---

## Key Notes

- Uses **DMA0** for string mirroring and **DMA1/DMA2** for LED and string merge transfers
- Interrupt-driven design ensures low-power mode operation between events
- Fully modular design separates API, BSP, and HAL layers for clarity and maintainability

---

## Contributors

- **Developed by:** *Add your name or team here*
- **Institution:** *Add your lab, course, or university here*

---

## Future Improvements

- Extend keypad character mappings to full alphanumeric input
- Optimize LCD refresh handling for smoother user experience
- Abstract hardware-specific configurations for broader MCU portability

---
