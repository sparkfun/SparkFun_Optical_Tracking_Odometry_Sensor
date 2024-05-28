
// sfeTkII2C.h
//
// Defines the I2C communication bus interface for the SparkFun Electronics Toolkit
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

#include "sfeTkIBus.h"

class sfeTkII2C : public sfeTkIBus
{
  public:
    // set the address to No address and stop flag to true
    sfeTkII2C() : _address{kNoAddress}, _stop{true}
    {
    }
    sfeTkII2C(uint8_t addr) : _address{addr}
    {
    }

    /*--------------------------------------------------------------------------
        @brief A simple ping of the device at the set address

        @retval sfeTkError_t - ok on success

    */
    virtual sfeTkError_t ping() = 0;

    /*--------------------------------------------------------------------------
        @brief setter for the I2C address

        @param devAddr The device's address

    */
    virtual void setAddress(uint8_t devAddr)
    {
        _address = devAddr;
    }

    /*--------------------------------------------------------------------------
        @brief getter for the I2C address

        @retval uint8_t returns the address for the device

    */
    virtual uint8_t address(void)
    {
        return _address;
    }

    /*--------------------------------------------------------------------------
        @brief setter for I2C stop message (vs restarts)

    */
    virtual void setStop(bool stop)
    {
        _stop = stop;
    }

    /*--------------------------------------------------------------------------
        @brief getter for I2C stops message (vs restarts)

        @retval bool returns the value of "send stop"

    */
    virtual bool stop(void)
    {
        return _stop;
    }

    static constexpr uint8_t kNoAddress = 0;

  private:
    uint8_t _address;
    bool _stop;
};

//};
