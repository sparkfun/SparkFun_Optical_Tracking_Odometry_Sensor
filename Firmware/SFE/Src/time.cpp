/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "time.h"
#include "stm32c0xx_ll_cortex.h"

volatile uint32_t millisCounter = 0;

uint32_t millis(void)
{
    // Just return the millisecond counter, it gets incremented by the
    // SysTick_Handler() every millisecond
    return millisCounter;
}

uint32_t micros(void)
{
    // SysTick->VAL decrements once every clock cycle (eg. 48MHz). Once it
    // reaches zero, it is reloaded with the value from SysTick->LOAD, which is
    // set such that the cycle repeats every millisecond (eg. 47999); this is
    // what triggers the SysTick_Handler() callback, where we increment
    // millisCounter. We can use SysTick->VAL to determine the number of
    // microseconds that have elapsed since the last increment of millisCounter

    // First grab a copy of millisCounter and SysTick->VAL, then grab a second
    // copy of both. It's possible that in the time between grabbing the first
    // copy of millisCounter and SysTick->VAL, SysTick->VAL could have hit zero
    // and caused millisCounter to increment, which would cause the values to
    // be inconsistent with each other. The second copy allows us to detect this
    // condition and correct for it 
    uint32_t millisCounterCopy = millisCounter;
    uint32_t sysTickValCopy = SysTick->VAL;
    uint32_t millisCounterCopy2 = millisCounter;
    uint32_t sysTickValCopy2 = SysTick->VAL;

    // Check if the two copies of millisCounter are different
    if(millisCounterCopy != millisCounterCopy2)
    {
        // millisCounter got incremented while we were copying it, so we'll use
        // the second copy to guarantee that our copied values are consistent
        // with each other
        millisCounterCopy = millisCounterCopy2;
        sysTickValCopy = sysTickValCopy2;
    }

    // Now we can calulate the number of microseconds that have elapsed since
    // the last millisecond tick. We'll first compute the number of clock cycles
    // that have occurred since the last millisecond tick with SysTick->LOAD,
    // then convert the clock cycles to microseconds
    uint32_t microsElapsed = SysTick->LOAD - sysTickValCopy;
    microsElapsed *= 1000;
    microsElapsed /= SysTick->LOAD + 1;

    // Now we can just add the millisCounter * 1000 to the elapsed microseconds
    return millisCounterCopy * 1000 + microsElapsed;
}

void delay(uint32_t milliseconds)
{
    // Here we simply loop until the requested amount of time has elapsed. It's
    // important to use micros() instead of millis(), since the user could have
    // called this just before a millisecond tick
    uint32_t t0 = micros();
    while(micros() - t0 < milliseconds * 1000);
}

void delayMicroseconds(uint32_t microseconds)
{
    // Here we simply loop until the requested amount of time has elapsed. Note
    // that micros() actually takes a few microseconds to execute, so this won't
    // be perfectly accurate
    uint32_t t0 = micros();
    while(micros() - t0 < microseconds);
}
