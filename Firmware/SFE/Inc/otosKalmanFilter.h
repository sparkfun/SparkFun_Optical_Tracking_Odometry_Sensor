/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"

// State vector dimension
const uint8_t kOtosKfSize = 3;

// Helpful matrix indexing constants
const uint8_t kOtosKfIdx00 = kOtosKfSize * 0 + 0;
const uint8_t kOtosKfIdx01 = kOtosKfSize * 0 + 1;
const uint8_t kOtosKfIdx02 = kOtosKfSize * 0 + 2;
const uint8_t kOtosKfIdx10 = kOtosKfSize * 1 + 0;
const uint8_t kOtosKfIdx11 = kOtosKfSize * 1 + 1;
const uint8_t kOtosKfIdx12 = kOtosKfSize * 1 + 2;
const uint8_t kOtosKfIdx20 = kOtosKfSize * 2 + 0;
const uint8_t kOtosKfIdx21 = kOtosKfSize * 2 + 1;
const uint8_t kOtosKfIdx22 = kOtosKfSize * 2 + 2;

class otosKalmanFilter
{
  public:
    otosKalmanFilter(float q00, float q11, float q22);
    void reset();
    void predict(float dt);
    void updateVel(float vel, float velVar);
    void updateAcc(float acc, float accVar);

    // State vector, representing position, velocity, and acceleration
    float xData[kOtosKfSize];

    // Error covariance matrix
    float pData[kOtosKfSize * kOtosKfSize];

  private:
    void update(float measurement, float variance, uint8_t idx);

    // Process model matrix
    float _fData[kOtosKfSize * kOtosKfSize];

    // Process noise covariance matrix
    float _qData[kOtosKfSize * kOtosKfSize];
};
