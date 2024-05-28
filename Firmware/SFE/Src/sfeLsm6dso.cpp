/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "sfeLsm6dso.h"

sfeTkError_t sfeLsm6dso::begin(sfeTkStmSPI *spiBus)
{
    // Ensure the SPI bus exists
    if(spiBus == nullptr)
        return kSTkErrFail;

    // Save the SPI bus
    _spi = spiBus;

    // Check if the device is connected
    sfeTkError_t err = isConnected();
    if(err != kSTkErrOk)
        return err;

    // The data rates default to zero, meaning they are off. We'll set some
    // default data rates here so it defaults to something useful
    err = setAccelDataRate(kLsm6dsoOdr104Hz);
    if(err != kSTkErrOk)
        return err;
    err = setGyroDataRate(kLsm6dsoOdr104Hz);
    if(err != kSTkErrOk)
        return err;

    return kSTkErrOk;
}

sfeTkError_t sfeLsm6dso::isConnected()
{
    // Read the product ID
    uint8_t productID = 0;
    sfeTkError_t err = _spi->readRegisterByte(kLsm6dsoRegWhoAmI, productID);
    if(err != kSTkErrOk)
        return err;
    
    // Check the product ID value
    return productID == kLsm6dsoProductId ? kSTkErrOk : kSTkErrFail;
}

sfeTkError_t sfeLsm6dso::setAccelRange(uint8_t range)
{
    sfe_lsm6dso_reg_ctrl1_xl_t ctrl1XL;
    sfeTkError_t err = _spi->readRegisterByte(kLsm6dsoRegCtrl1XL, ctrl1XL.value);
    if(err != kSTkErrOk)
        return err;
    
    ctrl1XL.fullScale = range;
    err = _spi->writeRegisterByte(kLsm6dsoRegCtrl1XL, ctrl1XL.value);
    if(err != kSTkErrOk)
        return err;

    switch (range)
    {
    case kLsm6dsoAccelRange2g:
        _rawToG = kLsm6dsoRawToG2g;
        break;
    case kLsm6dsoAccelRange4g:
        _rawToG = kLsm6dsoRawToG4g;
        break;
    case kLsm6dsoAccelRange8g:
        _rawToG = kLsm6dsoRawToG8g;
        break;
    case kLsm6dsoAccelRange16g:
        _rawToG = kLsm6dsoRawToG16g;
        break;
    default:
        return kSTkErrFail;
    }

    // Convert from G to m/s^2 (9.80665 m/s^2 = 1G)
    _rawToMps2 = _rawToG * 9.80665f;

    return kSTkErrOk;
}

sfeTkError_t sfeLsm6dso::setAccelDataRate(uint8_t rate)
{
    sfe_lsm6dso_reg_ctrl1_xl_t ctrl1XL;
    sfeTkError_t err = _spi->readRegisterByte(kLsm6dsoRegCtrl1XL, ctrl1XL.value);
    if(err != kSTkErrOk)
        return err;
    
    ctrl1XL.odr = rate;
    return _spi->writeRegisterByte(kLsm6dsoRegCtrl1XL, ctrl1XL.value);
}

sfeTkError_t sfeLsm6dso::setGyroRange(uint8_t range)
{
    sfe_lsm6dso_reg_ctrl2_g_t ctrl2G;
    sfeTkError_t err = _spi->readRegisterByte(kLsm6dsoRegCtrl2G, ctrl2G.value);
    if(err != kSTkErrOk)
        return err;
    
    ctrl2G.fullScale = range;
    err = _spi->writeRegisterByte(kLsm6dsoRegCtrl2G, ctrl2G.value);
    if(err != kSTkErrOk)
        return err;

    switch (range)
    {
    case kLsm6dsoGyroRange125Dps:
        _rawToDps = kLsm6dsoRawToDps125;
        break;
    case kLsm6dsoGyroRange250Dps:
        _rawToDps = kLsm6dsoRawToDps250;
        break;
    case kLsm6dsoGyroRange500Dps:
        _rawToDps = kLsm6dsoRawToDps500;
        break;
    case kLsm6dsoGyroRange1000Dps:
        _rawToDps = kLsm6dsoRawToDps1000;
        break;
    case kLsm6dsoGyroRange2000Dps:
        _rawToDps = kLsm6dsoRawToDps2000;
        break;
    default:
        return kSTkErrFail;
    }

    // Convert from degrees to radians (pi/180 = 0.01745329252)
    _rawToRps = _rawToDps * 0.01745329252f;

    return kSTkErrOk;
}

sfeTkError_t sfeLsm6dso::setGyroDataRate(uint8_t rate)
{
    sfe_lsm6dso_reg_ctrl2_g_t ctrl2G;
    sfeTkError_t err = _spi->readRegisterByte(kLsm6dsoRegCtrl2G, ctrl2G.value);
    if(err != kSTkErrOk)
        return err;
    
    ctrl2G.odr = rate;
    return _spi->writeRegisterByte(kLsm6dsoRegCtrl2G, ctrl2G.value);
}

sfeTkError_t sfeLsm6dso::setInterrupts(uint8_t pin, uint8_t sources)
{
    uint8_t reg;
    if (pin == kLsm6dsoInt1Pin)
        reg = kLsm6dsoRegInt1Ctrl;
    else if (pin == kLsm6dsoInt2Pin)
        reg = kLsm6dsoRegInt2Ctrl;
    else
        return kSTkErrFail;

    return _spi->writeRegisterByte(reg, sources);
}

sfeTkError_t sfeLsm6dso::getInterrupts(uint8_t pin, uint8_t &sources)
{
    uint8_t reg;
    if (pin == kLsm6dsoInt1Pin)
        reg = kLsm6dsoRegInt1Ctrl;
    else if (pin == kLsm6dsoInt2Pin)
        reg = kLsm6dsoRegInt2Ctrl;
    else
        return kSTkErrFail;

    return _spi->readRegisterByte(reg, sources);
}

sfeTkError_t sfeLsm6dso::readRawGyroZ(int16_t &gyroZ)
{
    uint8_t data[2];
    size_t readBytes = 0;
    sfeTkError_t err = _spi->readRegisterRegion(kLsm6dsoRegOutZLG, data, 2, readBytes);
    if(err != kSTkErrOk)
        return err;
    
    gyroZ = (data[1] << 8) | data[0];
    return kSTkErrOk;
}

sfeTkError_t sfeLsm6dso::readRawAccelGyroAll(uint8_t *dataBuffer, uint8_t bufferSize)
{
    // Ensure the buffer exists
    if (dataBuffer == nullptr)
        return kSTkErrFail;

    // Ensure the buffer is large enough
    if (bufferSize < 12)
        return kSTkErrFail;

    size_t readBytes = 0;
    return _spi->readRegisterRegion(kLsm6dsoRegOutXLG, dataBuffer, 12, readBytes);
}

sfeTkError_t sfeLsm6dso::readAccelGyroAll(float *dataBuffer, uint8_t bufferSize)
{
    // Ensure the buffer exists
    if (dataBuffer == nullptr)
        return kSTkErrFail;

    // Ensure the buffer is large enough
    if (bufferSize < 6)
        return kSTkErrFail;

    // Read the raw data
    uint8_t rawData[12];
    sfeTkError_t err = readRawAccelGyroAll(rawData, 12);
    if(err != kSTkErrOk)
        return err;
    
    // Convert the raw data to floats
    dataBuffer[0] = (int16_t)((rawData[1] << 8) | rawData[0]) * _rawToDps;
    dataBuffer[1] = (int16_t)((rawData[3] << 8) | rawData[2]) * _rawToDps;
    dataBuffer[2] = (int16_t)((rawData[5] << 8) | rawData[4]) * _rawToDps;
    dataBuffer[3] = (int16_t)((rawData[7] << 8) | rawData[6]) * _rawToG;
    dataBuffer[4] = (int16_t)((rawData[9] << 8) | rawData[8]) * _rawToG;
    dataBuffer[5] = (int16_t)((rawData[11] << 8) | rawData[10]) * _rawToG;

    return kSTkErrOk;
}

float sfeLsm6dso::rawToDps()
{
    return _rawToDps;
}

float sfeLsm6dso::rawToRps()
{
    return _rawToRps;
}

float sfeLsm6dso::rawToG()
{
    return _rawToG;
}

float sfeLsm6dso::rawToMps2()
{
    return _rawToMps2;
}

sfeTkError_t sfeLsm6dso::selfTest()
{
    // Create arrays to hold the normal, positive, and negative data
    float normalData[6];
    float posData[6];
    float negData[6];

    // First read all data with the self-test disabled
    sfeTkError_t err = readAccelGyroAll(normalData, 6);
    if(err != kSTkErrOk)
        return err;
    
    // Enable the positive self-test
    err = setSelfTest(kLsm6dsoSelfTestXLPos, kLsm6dsoSelfTestGPos);
    if(err != kSTkErrOk)
        return err;
    
    // Wait for it to take effect (10ms is arbitrary, a few measurement cycles)
    delayMillis(10);

    // Read the data with the positive self-test enabled
    err = readAccelGyroAll(posData, 6);
    if(err != kSTkErrOk)
        return err;
    
    // Enable the negative self-test
    err = setSelfTest(kLsm6dsoSelfTestXLNeg, kLsm6dsoSelfTestGNeg);
    if(err != kSTkErrOk)
        return err;
    
    // Wait for it to take effect (10ms is arbitrary, a few measurement cycles)
    delayMillis(10);

    // Read the data with the negative self-test enabled
    err = readAccelGyroAll(negData, 6);
    if(err != kSTkErrOk)
        return err;
    
    // Disable the self-test
    err = setSelfTest(kLsm6dsoSelfTestXLNormal, kLsm6dsoSelfTestGNormal);
    if(err != kSTkErrOk)
        return err;
    
    // Check the self-test results
    for(int i = 0; i < 6; i++)
    {
        // Compute the difference between the normal and self-test data
        float positiveDifference = posData[i] - normalData[i];
        float negativeDifference = normalData[i] - negData[i];

        // Check if the difference is within the acceptable range
        if(i < 3)
        {
            // Gyroscope data
            if(positiveDifference < kLsm6dsoSelfTestMinDps || positiveDifference > kLsm6dsoSelfTestMaxDps)
                return kSTkErrFail;
            if(negativeDifference < kLsm6dsoSelfTestMinDps || negativeDifference > kLsm6dsoSelfTestMaxDps)
                return kSTkErrFail;
        }
        else
        {
            // Accelerometer data
            if(positiveDifference < kLsm6dsoSelfTestMinG || positiveDifference > kLsm6dsoSelfTestMaxG)
                return kSTkErrFail;
            if(negativeDifference < kLsm6dsoSelfTestMinG || negativeDifference > kLsm6dsoSelfTestMaxG)
                return kSTkErrFail;
        }
    }

    // Self-test passed!
    return kSTkErrOk;
}

sfeTkError_t sfeLsm6dso::setSelfTest(uint8_t stAccel, uint8_t stGyro)
{
    // Read the current value of CTRL5_C
    sfe_lsm6dso_reg_ctrl5_t ctrl5C;
    sfeTkError_t err = _spi->readRegisterByte(kLsm6dsoRegCtrl5C, ctrl5C.value);
    if(err != kSTkErrOk)
        return err;
    
    // Set the self-test bits
    ctrl5C.selfTestXL = stAccel;
    ctrl5C.selfTestG = stGyro;
    return _spi->writeRegisterByte(kLsm6dsoRegCtrl5C, ctrl5C.value);
}
