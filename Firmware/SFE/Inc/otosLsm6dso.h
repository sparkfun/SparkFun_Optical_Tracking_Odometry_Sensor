/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"
#include "sfeLsm6dso.h"
#include "sfeTkStmSPI.h"
#include "time.h"

class LSM6DSO : public sfeLsm6dso
{
  public:
    sfeTkError_t begin()
    {
        sfeTkError_t err = _spiBus.init(LSM_CS_GPIO_Port, LSM_CS_Pin);
        if(err != kSTkErrOk)
            return err;

        return sfeLsm6dso::begin(&_spiBus);
    }

  protected:
    void delayMillis(uint32_t milliseconds)
    {
        delay(milliseconds);
    }

  private:
    // SPI interface
    sfeTkStmSPI _spiBus;
};