/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "sfeTkStmSPI.h"
#include "time.h"

sfeTkError_t sfeTkStmSPI::init(GPIO_TypeDef *csPort, uint32_t csPin, bool rwBitHighOnRead)
{
    // Store provided parameters
    _csPort = csPort;
    _csPin = csPin;
    _rwBitHighOnRead = rwBitHighOnRead;

    // Done!
    return kSTkErrOk;
}

sfeTkError_t writeByte(uint8_t data)
{
    return kSTkErrFail;
}

sfeTkError_t sfeTkStmSPI::writeRegisterByte(uint8_t regAddr, uint8_t data)
{
    return writeRegisterRegion(regAddr, &data, 1);
}

sfeTkError_t writeRegisterWord(uint8_t devReg, uint16_t data)
{
    return kSTkErrFail;
}

sfeTkError_t sfeTkStmSPI::writeRegisterRegion(uint8_t regAddr, const uint8_t *data, size_t numBytes)
{
    // Set chip select pin low
    LL_GPIO_ResetOutputPin(_csPort, _csPin);

    // Set write bit within register address
    regAddr &= ~0x80;
    regAddr |= _rwBitHighOnRead ? 0x00 : 0x80;

    // Send register address and wait for it to finish
    LL_SPI_TransmitData8(SPI1, regAddr);
    uint32_t timeoutStart = millis();
    while (LL_SPI_IsActiveFlag_BSY(SPI1) == 1)
    {
        // Check for timeout
        if (millis() - timeoutStart > 1)
        {
            // Set chip select pin high
            LL_GPIO_SetOutputPin(_csPort, _csPin);

            // Return error
            return kSTkErrFail;
        }
    }

    // Because we're in full-duplex mode, we can throw away received bytes
    LL_SPI_ReceiveData8(SPI1);

    // Loop through all bytes to write
    for (size_t i = 0; i < numBytes; i++)
    {
        // Trigger next byte transfer and wait for it to finish
        LL_SPI_TransmitData8(SPI1, data[i]);
        uint32_t timeoutStart = millis();
        while (LL_SPI_IsActiveFlag_BSY(SPI1) == 1)
        {
            // Check for timeout
            if (millis() - timeoutStart > 1)
            {
                // Set chip select pin high
                LL_GPIO_SetOutputPin(_csPort, _csPin);

                // Return error
                return kSTkErrFail;
            }
        }

        // Because we're in full-duplex mode, we can throw away received bytes
        LL_SPI_ReceiveData8(SPI1);
    }

    // Set chip select pin high
    LL_GPIO_SetOutputPin(_csPort, _csPin);

    // Done!
    return kSTkErrOk;
}

sfeTkError_t sfeTkStmSPI::readRegisterByte(uint8_t regAddr, uint8_t &data)
{
    size_t readBytes;
    return readRegisterRegion(regAddr, &data, 1, readBytes);
}

sfeTkError_t readRegisterWord(uint8_t devReg, uint16_t &data)
{
    return kSTkErrFail;
}

sfeTkError_t sfeTkStmSPI::readRegisterRegion(uint8_t regAddr, uint8_t *data, size_t numBytes, size_t &readBytes)
{
    // Set chip select pin low
    LL_GPIO_ResetOutputPin(_csPort, _csPin);

    // Set read bit within register address
    regAddr &= ~0x80;
    regAddr |= _rwBitHighOnRead ? 0x80 : 0x00;

    // Send register address and wait for it to finish
    LL_SPI_TransmitData8(SPI1, regAddr);
    uint32_t timeoutStart = millis();
    while (LL_SPI_IsActiveFlag_BSY(SPI1) == 1)
    {
        // Check for timeout
        if (millis() - timeoutStart > 1)
        {
            // Set chip select pin high
            LL_GPIO_SetOutputPin(_csPort, _csPin);

            // Return error
            return kSTkErrFail;
        }
    }

    // Because we're in full-duplex mode, we can throw away the first byte
    LL_SPI_ReceiveData8(SPI1);

    // Loop through all bytes to read
    for (size_t i = 0; i < numBytes; i++)
    {
        // Trigger next byte transfer and wait for it to finish
        LL_SPI_TransmitData8(SPI1, 0);
        uint32_t timeoutStart = millis();
        while (LL_SPI_IsActiveFlag_BSY(SPI1) == 1)
        {
            // Check for timeout
            if (millis() - timeoutStart > 1)
            {
                // Set chip select pin high
                LL_GPIO_SetOutputPin(_csPort, _csPin);

                // Return error
                return kSTkErrFail;
            }
        }

        // Store received byte in provided buffer
        data[i] = LL_SPI_ReceiveData8(SPI1);
    }

    // Set chip select pin high
    LL_GPIO_SetOutputPin(_csPort, _csPin);

    // Done!
    return kSTkErrOk;
}
