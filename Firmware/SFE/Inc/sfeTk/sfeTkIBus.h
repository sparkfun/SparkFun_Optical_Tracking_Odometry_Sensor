
// sfeTkIBus.h
//
// Defines the communication bus interface for the SparkFun Electronics Toolkit -> sfeTk
/*

The MIT License (MIT)

Copyright (c) 2023 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions: The
above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED
"AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include "sfeTkError.h"
#include <cstddef>

// Define our error codes for the bus. Note Errors are negative, warnings/info positive
// BUT keep the same increment on the base

const sfeTkError_t kSTkErrBusNotInit = kSTkErrFail * (kSTkErrBaseBus + 1);
const sfeTkError_t kSTkErrBusTimeout = kSTkErrFail * (kSTkErrBaseBus + 2);
const sfeTkError_t kSTkErrBusNoResponse = kSTkErrFail * (kSTkErrBaseBus + 3);
const sfeTkError_t kSTkErrBusDataTooLong = kSTkErrFail * (kSTkErrBaseBus + 4);
const sfeTkError_t kSTkErrBusNullSettings = kSTkErrFail * (kSTkErrBaseBus + 5);
const sfeTkError_t kSTkErrBusNullBuffer = kSTkErrFail * (kSTkErrBaseBus + 6);
const sfeTkError_t kSTkErrBusUnderRead = kSTkErrBaseBus + 7;
const sfeTkError_t kSTkErrBusNotEnabled = kSTkErrBaseBus + 8;

// Define the bus interface.

class sfeTkIBus
{
  public:
    /*--------------------------------------------------------------------------
        @brief Write a single byte to the device

        @param data Data to write.

        @retval sfeTkError_t -  kSTkErrOk on successful execution.

    */
    virtual sfeTkError_t writeByte(uint8_t data) = 0;

    /*--------------------------------------------------------------------------
        @brief Write a single byte to the given register

        @param devReg The device's register's address.
        @param data Data to write.

        @retval sfeTkError_t -  kSTkErrOk on successful execution.

    */
    virtual sfeTkError_t writeRegisterByte(uint8_t devReg, uint8_t data) = 0;

    /*--------------------------------------------------------------------------
        @brief Write a single word (16 bit) to the given register

        @param devReg The device's register's address.
        @param data Data to write.

        @retval sfeTkError_t -  kSTkErrOk on successful execution.

    */
    virtual sfeTkError_t writeRegisterWord(uint8_t devReg, uint16_t data) = 0;

    /*--------------------------------------------------------------------------
        @brief Writes a number of bytes starting at the given register's address.

        @param devAddr The device's address/pin
        param devReg The device's register's address.
        @param data Data to write.

        @retval sfeTkError_t kSTkErrOk on successful execution

    */
    virtual sfeTkError_t writeRegisterRegion(uint8_t devReg, const uint8_t *data, size_t length) = 0;

    /*--------------------------------------------------------------------------
        @brief Read a single byte from the given register

        @param devReg The device's register's address.
        @param data Data to read.

        @retval sfeTkError_t -  kSTkErrOk on successful execution.

    */
    virtual sfeTkError_t readRegisterByte(uint8_t devReg, uint8_t &data) = 0;

    /*--------------------------------------------------------------------------
        @brief Read a single word (16 bit) from the given register

        @param devReg The device's register's address.
        @param data Data to read.

        @retval sfeTkError_t -  kSTkErrOk on successful execution.
    */
    virtual sfeTkError_t readRegisterWord(uint8_t devReg, uint16_t &data) = 0;

    /*--------------------------------------------------------------------------
        @brief Reads a block of data from the given register.

        @param devAddr The device's I2C address.
        @param devReg The device's register's address.
        @param data Data to write.
        @param numBytes - length of data
        @param[out] readBytes - number of bytes read

        @retval int returns kSTkErrOk on success, or kSTkErrFail code

    */
    virtual sfeTkError_t readRegisterRegion(uint8_t reg, uint8_t *data, size_t numBytes, size_t &readBytes) = 0;
};

//};
