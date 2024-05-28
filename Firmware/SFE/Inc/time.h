/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"

// Millisecond counter, incremented by SysTick_Handler() in stm32c0xx_it.c
extern volatile uint32_t millisCounter;

// Arudino style millis(), returns the number of milliseconds since the program
// started. Overflows after ~50 days
uint32_t millis(void);

// Arudino style micros(), returns the number of microseconds since the program
// started. Overflows after ~70 minutes
uint32_t micros(void);

// Arudino style delay(), blocks the program for a given number of milliseconds
void delay(uint32_t milliseconds);

// Arudino style delayMicroseconds(), blocks the program for a given number of
// microseconds
void delayMicroseconds(uint32_t microseconds);
