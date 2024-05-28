/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "sfePaa5160.h"

// The datasheet provides a long list of register/value pairs to write to the
// PAA5160 to configure it. Here's that list!
const uint8_t performanceSettings[][2] = {
    {0x00, 0x00}, {0x3A, 0x5A}, {0x7F, 0x00}, {0x40, 0x80}, {0x7F, 0x14}, {0x4D, 0x00}, {0x53, 0x0D}, {0x4B, 0x20},
    {0x42, 0xBC}, {0x43, 0x74}, {0x58, 0x4C}, {0x79, 0x00}, {0x7F, 0x0E}, {0x54, 0x04}, {0x7F, 0x0E}, {0x55, 0x0D},
    {0x58, 0xD5}, {0x56, 0xFB}, {0x57, 0xEB}, {0x7F, 0x15}, {0x00, 0x00}, {0x7F, 0x07}, {0x40, 0x43}, {0x7F, 0x13},
    {0x49, 0x20}, {0x7F, 0x14}, {0x54, 0x02}, {0x7F, 0x15}, {0x60, 0x00}, {0x7F, 0x06}, {0x74, 0x50}, {0x7B, 0x02},
    {0x7F, 0x00}, {0x64, 0x74}, {0x65, 0x03}, {0x72, 0x0E}, {0x73, 0x00}, {0x7F, 0x14}, {0x61, 0x3E}, {0x62, 0x1E},
    {0x63, 0x1E}, {0x7F, 0x15}, {0x69, 0x1E}, {0x7F, 0x07}, {0x40, 0x40}, {0x7F, 0x00}, {0x61, 0x00}, {0x7F, 0x15},
    {0x63, 0x00}, {0x62, 0x00}, {0x7F, 0x00}, {0x61, 0xAD}, {0x7F, 0x15}, {0x5D, 0x2C}, {0x5E, 0xC4}, {0x00, 0x00},
    {0x5D, 0x04}, {0x5E, 0xEC}, {0x7F, 0x05}, {0x42, 0x48}, {0x43, 0xE7}, {0x7F, 0x06}, {0x71, 0x03}, {0x7F, 0x09},
    {0x60, 0x1C}, {0x61, 0x1E}, {0x62, 0x02}, {0x63, 0x04}, {0x64, 0x1E}, {0x65, 0x1F}, {0x66, 0x01}, {0x67, 0x02},
    {0x68, 0x02}, {0x69, 0x01}, {0x6A, 0x1F}, {0x6B, 0x1E}, {0x6C, 0x04}, {0x6D, 0x02}, {0x6E, 0x1E}, {0x6F, 0x1C},
    {0x7F, 0x05}, {0x45, 0x94}, {0x45, 0x14}, {0x44, 0x45}, {0x45, 0x17}, {0x7F, 0x09}, {0x47, 0x4F}, {0x4F, 0x00},
    {0x52, 0x04}, {0x7F, 0x0C}, {0x4E, 0x00}, {0x5B, 0x00}, {0x7F, 0x0D}, {0x71, 0x92}, {0x70, 0x07}, {0x73, 0x92},
    {0x72, 0x07}, {0x7F, 0x00}, {0x5B, 0x20}, {0x48, 0x13}, {0x49, 0x00}, {0x4A, 0x13}, {0x4B, 0x00}, {0x47, 0x01},
    {0x54, 0x55}, {0x5A, 0x50}, {0x66, 0x03}, {0x67, 0x00}, {0x7F, 0x07}, {0x40, 0x43}, {0x7F, 0x05}, {0x4D, 0x00},
    {0x6D, 0x96}, {0x55, 0x62}, {0x59, 0x21}, {0x5F, 0xD8}, {0x6A, 0x22}, {0x7F, 0x07}, {0x42, 0x30}, {0x43, 0x00},
    {0x7F, 0x06}, {0x4C, 0x01}, {0x54, 0x02}, {0x62, 0x01}, {0x7F, 0x09}, {0x41, 0x01}, {0x4F, 0x00}, {0x7F, 0x0A},
    {0x4C, 0x18}, {0x51, 0x8F}, {0x7F, 0x07}, {0x40, 0x40}, {0x7F, 0x00}, {0x40, 0x80}, {0x7F, 0x05}, {0x4D, 0x01},
    {0x7F, 0x06}, {0x54, 0x01}, {0x62, 0x01}, {0x7F, 0x09}, {0x40, 0x03}, {0x44, 0x08}, {0x4F, 0x08}, {0x7F, 0x0A},
    {0x51, 0x8E}, {0x7F, 0x00}, {0x66, 0x11}, {0x67, 0x08}
};

// Compute the number of performance settings
const uint8_t numPerformanceSettings = sizeof(performanceSettings) / sizeof(performanceSettings[0]);

sfeTkError_t sfePaa5160::loadPerformanceSettings()
{
    // This follows the datasheet's procedure for loading the performance
    // optimization settings into the PAA5160. It mostly involves writing
    // a bunch of register/value pairs, with a couple extra steps

    sfeTkError_t err = kSTkErrOk;

    // Step 1-19: write first set of register/value pairs
    for (int i = 1; i <= 19; i++)
    {
        err = _spi->writeRegisterByte(performanceSettings[i][0], performanceSettings[i][1]);
        if(err != kSTkErrOk)
            return err;
    }

    // Step 20: check bit 7 of register 0x58, there's a slight difference in
    // operation depending on its value
    uint8_t reg58 = 0;
    err = _spi->readRegisterByte(0x58, reg58);
    if(err != kSTkErrOk)
        return err;
    
    if ((reg58 & 0x80) == 0)
    {
        err = _spi->writeRegisterByte(0x58, 0x04);
        if(err != kSTkErrOk)
            return err;
        err = _spi->writeRegisterByte(0x57, 0x80);
        if(err != kSTkErrOk)
            return err;
    }
    else
    {
        err = _spi->writeRegisterByte(0x58, 0x84);
        if(err != kSTkErrOk)
            return err;
        err = _spi->writeRegisterByte(0x57, 0x00);
        if(err != kSTkErrOk)
            return err;
    }

    // Step 21-54: write next set of register/value pairs
    for (int i = 21; i <= 54; i++)
    {
        err = _spi->writeRegisterByte(performanceSettings[i][0], performanceSettings[i][1]);
        if(err != kSTkErrOk)
            return err;
    }

    // Step 55: wait 100ms
    delayMillis(100);

    // Step 56-147: write remaining set of register/value pairs
    for (int i = 56; i < numPerformanceSettings; i++)
    {
        err = _spi->writeRegisterByte(performanceSettings[i][0], performanceSettings[i][1]);
        if(err != kSTkErrOk)
            return err;
    }

    return kSTkErrOk;
}

sfeTkError_t sfePaa5160::init()
{
    sfeTkError_t err = 0;

    // Now we need to loop until the observation register is 0xB7 or 0xBF
    uint8_t observation = 0;
    uint8_t retries = 10;
    while ((observation != 0xB7) && (observation != 0xBF))
    {
        // Check if we've run out of retries
        if(retries == 0)
            return kSTkErrFail;
        retries--;

        // First clear the observation register
        err = _spi->writeRegisterByte(0x15, 0x00);
        if(err != kSTkErrOk)
            return err;

        // Then wait for 1ms
        delayMillis(1);

        // Now read the observation register
        err = _spi->readRegisterByte(0x15, observation);
        if(err != kSTkErrOk)
            return err;
    }

    // Now we load the performance settings as described in the datasheet
    err = loadPerformanceSettings();
    if(err != kSTkErrOk)
        return err;

    // The datahseet also suggests to read these registers to clear motion bit
    // and buffers
    uint8_t rawData;
    err = _spi->readRegisterByte(0x02, rawData);
    if(err != kSTkErrOk)
            return err;
    err = _spi->readRegisterByte(0x03, rawData);
    if(err != kSTkErrOk)
            return err;
    err = _spi->readRegisterByte(0x04, rawData);
    if(err != kSTkErrOk)
            return err;
    err = _spi->readRegisterByte(0x05, rawData);
    if(err != kSTkErrOk)
            return err;
    err = _spi->readRegisterByte(0x06, rawData);
    if(err != kSTkErrOk)
            return err;

    return kSTkErrOk;
}

sfeTkError_t sfePaa5160::begin(sfeTkStmSPI *spiBus)
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

    return init();
}

sfeTkError_t sfePaa5160::isConnected()
{
    // Read the product ID
    uint8_t productID = 0;
    sfeTkError_t err = _spi->readRegisterByte(0x00, productID);
    if(err != kSTkErrOk)
        return err;
        
    // Check the product ID value
    if (productID != kPaa5160ProductId)
        return kSTkErrFail;
    
    // Read the inverse product ID
    err = _spi->readRegisterByte(0x5F, productID);
    if(err != kSTkErrOk)
        return err;
        
    // Check the inverse product ID value
    if (productID != (uint8_t)(~kPaa5160ProductId))
        return kSTkErrFail;

    return kSTkErrOk;
}

sfeTkError_t sfePaa5160::softReset()
{
    // Send soft reset command
    sfeTkError_t err = _spi->writeRegisterByte(0x3A, kPaa5160CmdSoftReset);
    if(err != kSTkErrOk)
        return err;

    // Now re-initialize the sensor
    return init();
}

sfeTkError_t sfePaa5160::shutdown()
{
    return _spi->writeRegisterByte(0x3B, kPaa5160CmdShutdown);
}

sfeTkError_t sfePaa5160::getRevisionId(uint8_t &revisionId)
{
    return _spi->readRegisterByte(0x01, revisionId);
}

sfeTkError_t sfePaa5160::setResolution(uint16_t resX, uint16_t resY)
{
    // Ensure provided resolutions are valid
    if (resX > kPaa5160MaxResolution)
        return kSTkErrFail;
    if (resY > kPaa5160MaxResolution)
        return kSTkErrFail;

    sfeTkError_t err = kSTkErrOk;

    // Write resolution values to registers
    err = _spi->writeRegisterByte(0x48, resX & 0xFF);
    if(err != kSTkErrOk)
        return err;
    err = _spi->writeRegisterByte(0x49, resX >> 8);
    if(err != kSTkErrOk)
        return err;
    err = _spi->writeRegisterByte(0x4A, resY & 0xFF);
    if(err != kSTkErrOk)
        return err;
    err = _spi->writeRegisterByte(0x4B, resY >> 8);
    if(err != kSTkErrOk)
        return err;

    // New resolution doesn't take effect until we tell it to update
    uint8_t setRes;
    err = _spi->readRegisterByte(0x47, setRes);
    if(err != kSTkErrOk)
        return err;
    setRes |= 0x01;
    err = _spi->writeRegisterByte(0x47, setRes);
    if(err != kSTkErrOk)
        return err;

    // Compute the raw to inch and meter conversion factors
    _rawToInchX = 1.0f / ((resX + 1) * 100);
    _rawToInchY = 1.0f / ((resY + 1) * 100);
    _rawToMeterX = _rawToInchX * 0.0254f;
    _rawToMeterY = _rawToInchY * 0.0254f;

    // Done!
    return kSTkErrOk;
}
sfeTkError_t sfePaa5160::setResolution(uint16_t resXY)
{
    return setResolution(resXY, resXY);
}

sfeTkError_t sfePaa5160::getResolution(uint16_t &resX, uint16_t &resY)
{
    sfeTkError_t err = kSTkErrOk;

    uint8_t res[4];
    err = _spi->readRegisterByte(0x48, res[0]);
    if(err != kSTkErrOk)
        return err;
    err = _spi->readRegisterByte(0x49, res[1]);
    if(err != kSTkErrOk)
        return err;
    err = _spi->readRegisterByte(0x4A, res[2]);
    if(err != kSTkErrOk)
        return err;
    err = _spi->readRegisterByte(0x4B, res[3]);
    if(err != kSTkErrOk)
        return err;

    resX = (res[1] << 8) | res[0];
    resY = (res[3] << 8) | res[2];

    return kSTkErrOk;
}

float sfePaa5160::rawToInchesX()
{
    return _rawToInchX;
}

float sfePaa5160::rawToMetersX()
{
    return _rawToMeterX;
}

float sfePaa5160::rawToInchesY()
{
    return _rawToInchY;
}

float sfePaa5160::rawToMetersY()
{
    return _rawToMeterY;
}

sfeTkError_t sfePaa5160::setOrientation(sfe_paa5160_orientation_t oritentation)
{
    sfe_paa5160_orientation_t reg;
    sfeTkError_t err = _spi->readRegisterByte(0x5B, reg.value);
    if(err != kSTkErrOk)
        return err;
    
    reg.invertX = oritentation.invertX;
    reg.invertY = oritentation.invertY;
    reg.swapXY = oritentation.swapXY;
    return _spi->writeRegisterByte(0x5B, reg.value);
}

sfeTkError_t sfePaa5160::getOrientation(sfe_paa5160_orientation_t &oritentation)
{
    return _spi->readRegisterByte(0x5B, oritentation.value);
}

sfeTkError_t sfePaa5160::readBurstData(uint8_t *dataBuffer, uint8_t bufferSize)
{
    // Ensure the data buffer exists
    if(dataBuffer == nullptr)
        return kSTkErrFail;
    
    // Ensure the data buffer is large enough
    if(bufferSize < 12)
        return kSTkErrFail;

    size_t readBytes = 0;
    return _spi->readRegisterRegion(0x16, dataBuffer, 12, readBytes);
}
