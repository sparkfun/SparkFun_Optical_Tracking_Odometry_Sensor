/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"

// Useful constants for rotation
const float kPi = 3.1415926535897932384626433832795f;
const float k2Pi = 2.0f * kPi;
const float kPi2 = kPi / 2.0f;
const float kRadToDeg = 180.0f / kPi;
const float kDegToRad = kPi / 180.0f;

// Conversion factor for the linear position registers. 16-bit signed registers
// with a max value of 10 meters (394 inches) gives a resolution of about 0.0003
// mps (0.012 ips)
const float kMeterToInt16 = 32768.0f / 10.0f;
const float kInt16ToMeter = 1.0f / kMeterToInt16;

// Conversion factor for the linear velocity registers. 16-bit signed registers
// with a max value of 5 mps (197 ips) gives a resolution of about 0.00015 mps
// (0.006 ips)
const float kMpsToInt16 = 32768.0f / 5.0f;
const float kInt16ToMps = 1.0f / kMpsToInt16;

// Conversion factor for the linear acceleration registers. 16-bit signed
// registers with a max value of 157 mps^2 (16 g) gives a resolution of
// about 0.0048 mps^2 (0.49 mg)
const float kMpssToInt16 = 32768.0f / (16.0f * 9.80665f);
const float kInt16ToMpss = 1.0f / kMpssToInt16;

// Conversion factor for the angular position registers. 16-bit signed registers
// with a max value of pi radians (180 degrees) gives a resolution of about
// 0.00096 radians (0.0055 degrees)
const float kRadToInt16 = 32768.0f / kPi;
const float kInt16ToRad = 1.0f / kRadToInt16;

// Conversion factor for the angular velocity registers. 16-bit signed registers
// with a max value of 34.9 rps (2000 dps) gives a resolution of about 0.0011
// rps (0.061 degrees per second)
const float kRpsToInt16 = 32768.0f / (2000.0f * kDegToRad);
const float kInt16ToRps = 1.0f / kRpsToInt16;

// Conversion factor for the angular acceleration registers. 16-bit signed
// registers with a max value of 3141 rps^2 (180000 dps^2) gives a resolution of
// about 0.096 rps^2 (5.5 dps^2)
const float kRpssToInt16 = 32768.0f / (kPi * 1000.0f);
const float kInt16ToRpss = 1.0f / kRpssToInt16;
