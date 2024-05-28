/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"
#include "sfePaa5160.h"
#include "sfeTkStmSPI.h"
#include "adc.h"
#include "time.h"

class PAA5160 : public sfePaa5160
{
  public:
    sfeTkError_t begin()
    {
        // First we need to power up the PAA5160
        sfeTkError_t err = powerUpSequence();
        if(err != kSTkErrOk)
            return err;

        // Now we need SPI, so initialize the SPI bus
        err = _spiBus.init(PAA_CS_GPIO_Port, PAA_CS_Pin, false);
        if(err != kSTkErrOk)
            return err;

        return sfePaa5160::begin(&_spiBus);
    }

    sfeTkError_t powerUpSequence()
    {
        // Begin power up of PAA5160
        LL_GPIO_ResetOutputPin(PAA_POW_GPIO_Port, PAA_POW_Pin);

        // We need to wait for the voltage to stabilize. The ADC is configured
        // to measure the 1.8V net, so we can measure that and wait for it to
        // rise up. Testing has shown this happens within ~100us, but we'll wait
        // for 1ms to be safe
        delayMillis(1);

        // Now measure the 1.8V net and see if it's in an acceptable range
        uint16_t adcMillivolts;
        if(readADCMillivolts(adcMillivolts) == false)
            return kSTkErrFail;
        if (adcMillivolts < 1700 || adcMillivolts > 1900)
        {
            // The voltage is too far off, something is likely wrong
            return kSTkErrFail;
        }

        // The power is now stable, however the reset pin has been held low to
        // prevent the PAA5160 from getting back-power through the ESD diode. We
        // can now release the reset pin and allow the PAA5160 to turn on
        LL_GPIO_SetOutputPin(PAA_RST_GPIO_Port, PAA_RST_Pin);

        // The datasheet says we need to wait 50ms before enabling VCSEL power
        delayMillis(50);

        // Now enable VCSEL power
        LL_GPIO_ResetOutputPin(VCSEL_POW_GPIO_Port, VCSEL_POW_Pin);

        // The datasheet says to wait at least 0.1ms after VCSEL power is stable
        // Testing has shown it stabilizes within ~100us, so 1ms is plenty
        delayMillis(1);

        // The chip select pin has also been held low to prevent the PAA5160
        // from back-powering through its ESD diode, but we can set it high now
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);

        // The datasheet says to assert the reset pin for >20 microseconds, then
        // de-assert to reset the sensor
        LL_GPIO_ResetOutputPin(PAA_RST_GPIO_Port, PAA_RST_Pin);
        delayMicroseconds(20);
        LL_GPIO_SetOutputPin(PAA_RST_GPIO_Port, PAA_RST_Pin);

        // Followed by a 2ms delay
        delayMillis(2);

        return kSTkErrOk;
    }

  protected:
    void delayMillis(uint32_t milliseconds)
    {
        delay(milliseconds);
    }

  private:
    // SPI interface
    sfeTkStmSPI _spiBus;
};