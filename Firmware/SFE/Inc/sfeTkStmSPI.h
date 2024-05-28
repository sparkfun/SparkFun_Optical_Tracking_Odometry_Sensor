/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"
#include "sfeTk/sfeTkISPI.h"

class sfeTkStmSPI
{
  public:
    sfeTkError_t init(GPIO_TypeDef *csPort, uint32_t csPin, bool rwBitHighOnRead = true);

    sfeTkError_t writeByte(uint8_t data);

    sfeTkError_t writeRegisterByte(uint8_t regAddr, uint8_t data);

    sfeTkError_t writeRegisterWord(uint8_t devReg, uint16_t data);

    sfeTkError_t writeRegisterRegion(uint8_t regAddr, const uint8_t *data, size_t numBytes);

    sfeTkError_t readRegisterByte(uint8_t regAddr, uint8_t &data);

    sfeTkError_t readRegisterWord(uint8_t devReg, uint16_t &data);

    sfeTkError_t readRegisterRegion(uint8_t regAddr, uint8_t *data, size_t numBytes, size_t &readBytes);

  private:
    GPIO_TypeDef *_csPort;
    uint32_t _csPin;
    bool _rwBitHighOnRead;
};