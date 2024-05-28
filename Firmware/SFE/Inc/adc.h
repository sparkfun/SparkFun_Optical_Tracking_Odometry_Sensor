/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"

// Analog voltage supply in millivolts. This application is only ever 3.3V
const uint16_t kVddaMv = 3300;

// Helper function to calibrate and enable the ADC
bool calibrateAndEnableADC(void);

// Helper function to read the ADC value in millivolts
bool readADCMillivolts(uint16_t &adcRaw);
