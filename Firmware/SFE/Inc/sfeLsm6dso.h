/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "sfeTk/sfeToolkit.h"
#include "sfeTkStmSPI.h"
#include <stdint.h>

// Register address definitions
const uint8_t kLsm6dsoRegFuncCfgAccess = 0x01;
const uint8_t kLsm6dsoRegPinCtrl = 0x02;
const uint8_t kLsm6dsoRegS4sTphL = 0x04;
const uint8_t kLsm6dsoRegS4sTphH = 0x05;
const uint8_t kLsm6dsoRegS4sRr = 0x06;
const uint8_t kLsm6dsoRegFifoCtrl1 = 0x07;
const uint8_t kLsm6dsoRegFifoCtrl2 = 0x08;
const uint8_t kLsm6dsoRegFifoCtrl3 = 0x09;
const uint8_t kLsm6dsoRegFifoCtrl4 = 0x0A;
const uint8_t kLsm6dsoRegCounterBdrReg1 = 0x0B;
const uint8_t kLsm6dsoRegCounterBdrReg2 = 0x0C;
const uint8_t kLsm6dsoRegInt1Ctrl = 0x0D;
const uint8_t kLsm6dsoRegInt2Ctrl = 0x0E;
const uint8_t kLsm6dsoRegWhoAmI = 0x0F;
const uint8_t kLsm6dsoRegCtrl1XL = 0x10;
const uint8_t kLsm6dsoRegCtrl2G = 0x11;
const uint8_t kLsm6dsoRegCtrl3C = 0x12;
const uint8_t kLsm6dsoRegCtrl4C = 0x13;
const uint8_t kLsm6dsoRegCtrl5C = 0x14;
const uint8_t kLsm6dsoRegCtrl6C = 0x15;
const uint8_t kLsm6dsoRegCtrl7G = 0x16;
const uint8_t kLsm6dsoRegCtrl8XL = 0x17;
const uint8_t kLsm6dsoRegCtrl9XL = 0x18;
const uint8_t kLsm6dsoRegCtrl10C = 0x19;
const uint8_t kLsm6dsoRegAllIntSrc = 0x1A;
const uint8_t kLsm6dsoRegWakeupSrc = 0x1B;
const uint8_t kLsm6dsoRegTapSrc = 0x1C;
const uint8_t kLsm6dsoRegD6dSrc = 0x1D;
const uint8_t kLsm6dsoRegStatusReg = 0x1E;
const uint8_t kLsm6dsoRegOutTempL = 0x20;
const uint8_t kLsm6dsoRegOutTempH = 0x21;
const uint8_t kLsm6dsoRegOutXLG = 0x22;
const uint8_t kLsm6dsoRegOutXHG = 0x23;
const uint8_t kLsm6dsoRegOutYLG = 0x24;
const uint8_t kLsm6dsoRegOutYHG = 0x25;
const uint8_t kLsm6dsoRegOutZLG = 0x26;
const uint8_t kLsm6dsoRegOutZHG = 0x27;
const uint8_t kLsm6dsoRegOutXLA = 0x28;
const uint8_t kLsm6dsoRegOutXHA = 0x29;
const uint8_t kLsm6dsoRegOutYLA = 0x2A;
const uint8_t kLsm6dsoRegOutYHA = 0x2B;
const uint8_t kLsm6dsoRegOutZLA = 0x2C;
const uint8_t kLsm6dsoRegOutZHA = 0x2D;

// Product ID in WhoAmI register
const uint8_t kLsm6dsoProductId = 0x6C;

// Accelerometer full scale range values
const uint8_t kLsm6dsoAccelRange2g = 0x00;
const uint8_t kLsm6dsoAccelRange16g = 0x01;
const uint8_t kLsm6dsoAccelRange4g = 0x02;
const uint8_t kLsm6dsoAccelRange8g = 0x03;

// Accelerometer full scale range conversion factors
const float kLsm6dsoRawToG2g = 0.061e-3f;
const float kLsm6dsoRawToG4g = 0.122e-3f;
const float kLsm6dsoRawToG8g = 0.244e-3f;
const float kLsm6dsoRawToG16g = 0.488e-3f;

// Gyro full scale range values
const uint8_t kLsm6dsoGyroRange125Dps = 0x01;
const uint8_t kLsm6dsoGyroRange250Dps = 0x00;
const uint8_t kLsm6dsoGyroRange500Dps = 0x02;
const uint8_t kLsm6dsoGyroRange1000Dps = 0x04;
const uint8_t kLsm6dsoGyroRange2000Dps = 0x06;

// Gyro full scale range conversion factors
const float kLsm6dsoRawToDps125 = 4.375e-3f;
const float kLsm6dsoRawToDps250 = 8.75e-3f;
const float kLsm6dsoRawToDps500 = 17.5e-3f;
const float kLsm6dsoRawToDps1000 = 35e-3f;
const float kLsm6dsoRawToDps2000 = 70e-3f;

// Accelerometer and gyroscope output data rate values
const uint8_t kLsm6dsoOdrDisable = 0x00;
const uint8_t kLsm6dsoOdr12Hz5 = 0x01;
const uint8_t kLsm6dsoOdr26Hz = 0x02;
const uint8_t kLsm6dsoOdr52Hz = 0x03;
const uint8_t kLsm6dsoOdr104Hz = 0x04;
const uint8_t kLsm6dsoOdr208Hz = 0x05;
const uint8_t kLsm6dsoOdr416Hz = 0x06;
const uint8_t kLsm6dsoOdr833Hz = 0x07;
const uint8_t kLsm6dsoOdr1660Hz = 0x08;
const uint8_t kLsm6dsoOdr3330Hz = 0x09;
const uint8_t kLsm6dsoOdr6660Hz = 0x0A;
const uint8_t kLsm6dsoOdr1Hz6 = 0x0B;

// Interrupt pins
const uint8_t kLsm6dsoInt1Pin = 0x00;
const uint8_t kLsm6dsoInt2Pin = 0x01;

// Interrupt sources
const uint8_t kLsm6dsoInt1SrcDrdyXl = 0x01;
const uint8_t kLsm6dsoInt1SrcDrdyG = 0x02;
const uint8_t kLsm6dsoInt1SrcBoot = 0x04;
const uint8_t kLsm6dsoInt1SrcFifoTh = 0x08;
const uint8_t kLsm6dsoInt1SrcFifoOvr = 0x10;
const uint8_t kLsm6dsoInt1SrcFifoFull = 0x20;
const uint8_t kLsm6dsoInt1SrcCntBdr = 0x40;
const uint8_t kLsm6dsoInt1SrcDenDrdy = 0x80;
const uint8_t kLsm6dsoInt2SrcDrdyXl = 0x01;
const uint8_t kLsm6dsoInt2SrcDrdyG = 0x02;
const uint8_t kLsm6dsoInt2SrcDrdyTemp = 0x04;
const uint8_t kLsm6dsoInt2SrcFifoTh = 0x08;
const uint8_t kLsm6dsoInt2SrcFifoOvr = 0x10;
const uint8_t kLsm6dsoInt2SrcFifoFull = 0x20;
const uint8_t kLsm6dsoInt2SrcCntBdr = 0x40;

// Self-test register values
const uint8_t kLsm6dsoSelfTestXLNormal = 0x00;
const uint8_t kLsm6dsoSelfTestXLPos = 0x01;
const uint8_t kLsm6dsoSelfTestXLNeg = 0x02;
const uint8_t kLsm6dsoSelfTestGNormal = 0x00;
const uint8_t kLsm6dsoSelfTestGPos = 0x01;
const uint8_t kLsm6dsoSelfTestGNeg = 0x03;

// Self-test minimum and maximum values in dps (at 2000dps range) and g
const float kLsm6dsoSelfTestMinDps = 150.0f;
const float kLsm6dsoSelfTestMaxDps = 700.0f;
const float kLsm6dsoSelfTestMinG = 0.05f;
const float kLsm6dsoSelfTestMaxG = 1.7f;

// Accelerometer Control register bitfield definitions
typedef union {
    struct
    {
        uint8_t reserved : 1;
        uint8_t lowPassFilter : 1;
        uint8_t fullScale : 2;
        uint8_t odr : 4;
    };
    uint8_t value;
} sfe_lsm6dso_reg_ctrl1_xl_t;

// Gyroscope Control register bitfield definitions
typedef union {
    struct
    {
        uint8_t reserved : 1;
        uint8_t fullScale : 3;
        uint8_t odr : 4;
    };
    uint8_t value;
} sfe_lsm6dso_reg_ctrl2_g_t;

// Control 5 register bitfield definitions
typedef union {
    struct
    {
        uint8_t selfTestXL : 2;
        uint8_t selfTestG : 2;
        uint8_t reserved : 1;
        uint8_t rounding : 2;
        uint8_t xlUlpEn : 1;
    };
    uint8_t value;
} sfe_lsm6dso_reg_ctrl5_t;

class sfeLsm6dso
{
  public:
    /// @brief Begins the LSM6DSO
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t begin(sfeTkStmSPI *spiBus = nullptr);

    /// @brief Checks if the LSM6DSO is connected
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t isConnected();

    /// @brief Sets the accelerometer full scale range
    /// @param range 
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t setAccelRange(uint8_t range);

    /// @brief Sets the accelerometer output data rate
    /// @param rate 
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t setAccelDataRate(uint8_t rate);

    /// @brief Sets the gyroscope full scale range
    /// @param range 
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t setGyroRange(uint8_t range);

    /// @brief Sets the gyroscope output data rate
    /// @param rate 
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t setGyroDataRate(uint8_t rate);

    /// @brief Sets which interrupt sources to enable on the provided pin
    /// @param pin Which interrupt pin on the LSM6DSO to configure
    /// @param sources Bit mask of interrupt sources to enable on the pin
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t setInterrupts(uint8_t pin, uint8_t sources);

    /// @brief Gets the interrupt sources enabled on the provided pin
    /// @param pin Which pin on the LSM6DSO to query
    /// @param sources Bit mask of interrupt sources enabled on the pin
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t getInterrupts(uint8_t pin, uint8_t &sources);

    /// @brief Reads the raw gyroscope Z value
    /// @param gyroZ Raw gyroscope Z value
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t readRawGyroZ(int16_t &gyroZ);

    /// @brief Performs a burst read of all accelerometer and gyroscope data
    /// @param dataBuffer Buffer to store the data, must be at least 12 bytes
    /// @param bufferSize Size of the data buffer
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t readRawAccelGyroAll(uint8_t *dataBuffer, uint8_t bufferSize);

    /// @brief Performs a burst read of all accelerometer and gyroscope data and
    /// converts the data to dps and g
    /// @param dataBuffer Buffer to store the data, must be at least 6 elements.
    /// Will be filled with [gyroX, gyroY, gyroZ, accelX, accelY, accelZ]
    /// @param bufferSize Size of the data buffer
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t readAccelGyroAll(float *dataBuffer, uint8_t bufferSize);

    /// @brief Returns the conversion factor to deg/sec for the gyroscope
    /// @return Conversion factor to deg/sec for the gyroscope
    float rawToDps();

    /// @brief Returns the conversion factor to rad/sec for the gyroscope
    /// @return Conversion factor to rad/sec for the gyroscope
    float rawToRps();

    /// @brief Returns the conversion factor to g for the accelerometer
    /// @return Conversion factor to g for the accelerometer
    float rawToG();

    /// @brief Returns the conversion factor to m/s^2 for the accelerometer
    /// @return Conversion factor to m/s^2 for the accelerometer
    float rawToMps2();

    /// @brief Performs a self-test on the LSM6DSO. Sensor must be stationary!
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t selfTest();

  protected:
    virtual void delayMillis(uint32_t milliseconds) = 0;

  private:
    // Handle to the SPI interface
    sfeTkStmSPI *_spi;

    // Conversion factors
    float _rawToG;
    float _rawToMps2;
    float _rawToDps;
    float _rawToRps;

    sfeTkError_t setSelfTest(uint8_t stAccel, uint8_t stGyro);
};