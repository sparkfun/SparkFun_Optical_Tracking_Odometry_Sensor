/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "sfeTk/sfeToolkit.h"
#include "sfeTkStmSPI.h"
#include <stdint.h>

// Orientation register bit fields
typedef union {
    struct
    {
        uint8_t reserved : 5;
        uint8_t invertX : 1;
        uint8_t invertY : 1;
        uint8_t swapXY : 1;
    };
    uint8_t value;
} sfe_paa5160_orientation_t;

// Product ID
const uint8_t kPaa5160ProductId = 0x4F;

// Soft reset command
const uint8_t kPaa5160CmdSoftReset = 0x5A;

// Shutdown command
const uint8_t kPaa5160CmdShutdown = 0xB6;

// Resolution is 100 dpi per LSB
const uint8_t kPaa5160ResolutionPerLsb = 100;

// Max resolution is 20k dpi, but the register value is (resolution / 100) - 1
// so the max raw value is (20000 / 100) - 1 = 199
const uint16_t kPaa5160MaxResolution = 199;

class sfePaa5160
{
  public:
    /// @brief Begins the PAA5160
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t begin(sfeTkStmSPI *spiBus = nullptr);

    /// @brief Checks if the PAA5160 is connected
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t isConnected();

    /// @brief Performs a soft reset of the sensor and re-initilizaes it
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t softReset();

    /// @brief Shuts down the PAA5160, must call softReset() to power up again
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t shutdown();

    /// @brief Gets the revision ID of the PAA5160
    /// @param revisionId Revision ID value
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t getRevisionId(uint8_t &revisionId);

    /// @brief Sets the resolution of the X and Y axes. The resolution is
    /// (register value + 1) * 100 dpi. For example, a value of 0 is 100 dpi,
    /// 1 is 200 dpi, etc. The maximum resolution is 20k dpi, or a value of 199
    /// @param resX Raw resolution value for the X axis
    /// @param resY Raw resolution value for the Y axis
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t setResolution(uint16_t resX, uint16_t resY);

    /// @brief Sets the resolution of the X and Y axes. The resolution is
    /// (register value + 1) * 100 dpi. For example, a value of 0 is 100 dpi,
    /// 1 is 200 dpi, etc. The maximum resolution is 20k dpi, or a value of 199
    /// @param resXY Raw resolution value for both axes
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t setResolution(uint16_t resXY);

    /// @brief Gets the resolution of the X and Y axes. The resolution is
    /// (register value + 1) * 100 dpi. For example, a value of 0 is 100 dpi,
    /// 1 is 200 dpi, etc. The maximum resolution is 20k dpi, or a value of 199
    /// @param resX Raw resolution value for the X axis
    /// @param resY Raw resolution value for the Y axis
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t getResolution(uint16_t &resX, uint16_t &resY);

    /// @brief Gets the current conversion factor to inches for the X axis
    /// @return Conversion factor to inches for the X axis
    float rawToInchesX();

    /// @brief Gets the current conversion factor to meters for the X axis
    /// @return Conversion factor to meters for the X axis
    float rawToMetersX();

    /// @brief Gets the current conversion factor to inches for the Y axis
    /// @return Conversion factor to inches for the Y axis
    float rawToInchesY();

    /// @brief Gets the current conversion factor to meters for the Y axis
    /// @return Conversion factor to meters for the Y axis
    float rawToMetersY();

    /// @brief Sets the orientation values for the PAA5160, which can invert the
    /// X and Y axes, and swap the X and Y axes
    /// @param oritentation Orientation value
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t setOrientation(sfe_paa5160_orientation_t oritentation);

    /// @brief Gets the orientation values for the PAA5160, which can invert the
    /// X and Y axes, and swap the X and Y axes
    /// @param oritentation Orientation value
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t getOrientation(sfe_paa5160_orientation_t &oritentation);

    /// @brief Performs a burst read of all data registers, then stores the data
    /// in the provided array
    /// @param dataBuffer Raw data for burst read, must have at least 12 bytes
    /// @param bufferSize Size of the data buffer
    /// @return 0 if successful, negative for errors, positive for warnings
    sfeTkError_t readBurstData(uint8_t *dataBuffer, uint8_t bufferSize);

  protected:
    virtual void delayMillis(uint32_t milliseconds) = 0;

  private:
    // SPI bus object
    sfeTkStmSPI *_spi;

    // Resolution
    float _rawToInchX;
    float _rawToInchY;
    float _rawToMeterX;
    float _rawToMeterY;

    // Helper function to load the performance settings
    sfeTkError_t loadPerformanceSettings();

    // Helper function to initialize the PAA5160
    sfeTkError_t init();
};