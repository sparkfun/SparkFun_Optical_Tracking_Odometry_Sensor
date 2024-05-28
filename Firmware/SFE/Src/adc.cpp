/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "adc.h"
#include "time.h"

bool calibrateAndEnableADC(void)
{
    // Start ADC calibration and wait for it to finish
    LL_ADC_StartCalibration(ADC1);
    uint32_t timeoutStart = millis();
    while (LL_ADC_IsCalibrationOnGoing(ADC1))
    {
        // Check for timeout
        if (millis() - timeoutStart > 1)
            return false;
    }

    // Must wait an additional amount of time after calibration is finished
    // before ADC can be enabled. The delay time is defined as 2 ADC clock
    // cycles, which runs at the same frequency as the CPU in this application,
    // so this will actually take a lot longer than needed, but that's okay
    uint32_t delayCounter = LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES;
    while (delayCounter != 0)
    {
        delayCounter--;
    }

    // Now we can enable ADC and wait for it to be ready
    LL_ADC_Enable(ADC1);
    timeoutStart = millis();
    while (!LL_ADC_IsActiveFlag_ADRDY(ADC1))
    {
        // Check for timeout
        if (millis() - timeoutStart > 1)
            return false;
    }

    return true;
}

bool readADCMillivolts(uint16_t &adcRaw)
{
    // Start ADC conversion and wait for it to finish
    LL_ADC_REG_StartConversion(ADC1);
    uint32_t timeoutStart = millis();
    while (!LL_ADC_IsActiveFlag_EOC(ADC1))
    {
        // Check for timeout
        if (millis() - timeoutStart > 1)
            return false;
    }

    // Read raw ADC value and convert to millivolts
    adcRaw = LL_ADC_REG_ReadConversionData12(ADC1);
    adcRaw = __LL_ADC_CALC_DATA_TO_VOLTAGE(kVddaMv, adcRaw, LL_ADC_RESOLUTION_12B);

    return true;
}
