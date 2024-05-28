/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#pragma once

#include "main.h"

// Register location definitions
const uint8_t kOtosRegProdID = 0x00;
const uint8_t kOtosRegHwVersion = 0x01;
const uint8_t kOtosRegFwVersion = 0x02;
// const uint8_t kOtosRegI2cAddress = 0x03;
const uint8_t kOtosRegScalarLinear = 0x04;
const uint8_t kOtosRegScalarAngular = 0x05;
const uint8_t kOtosRegImuCalib = 0x06;
const uint8_t kOtosRegReset = 0x07;

const uint8_t kOtosRegSignalProcess = 0x0E;
const uint8_t kOtosRegSelfTest = 0x0F;

const uint8_t kOtosRegOffXL = 0x10;
const uint8_t kOtosRegOffXH = 0x11;
const uint8_t kOtosRegOffYL = 0x12;
const uint8_t kOtosRegOffYH = 0x13;
const uint8_t kOtosRegOffHL = 0x14;
const uint8_t kOtosRegOffHH = 0x15;

const uint8_t kOtosRegStatus = 0x1F;

const uint8_t kOtosRegPosXL = 0x20;
const uint8_t kOtosRegPosXH = 0x21;
const uint8_t kOtosRegPosYL = 0x22;
const uint8_t kOtosRegPosYH = 0x23;
const uint8_t kOtosRegPosHL = 0x24;
const uint8_t kOtosRegPosHH = 0x25;
const uint8_t kOtosRegVelXL = 0x26;
const uint8_t kOtosRegVelXH = 0x27;
const uint8_t kOtosRegVelYL = 0x28;
const uint8_t kOtosRegVelYH = 0x29;
const uint8_t kOtosRegVelHL = 0x2A;
const uint8_t kOtosRegVelHH = 0x2B;
const uint8_t kOtosRegAccXL = 0x2C;
const uint8_t kOtosRegAccXH = 0x2D;
const uint8_t kOtosRegAccYL = 0x2E;
const uint8_t kOtosRegAccYH = 0x2F;
const uint8_t kOtosRegAccHL = 0x30;
const uint8_t kOtosRegAccHH = 0x31;

const uint8_t kOtosRegPosStdXL = 0x32;
const uint8_t kOtosRegPosStdXH = 0x33;
const uint8_t kOtosRegPosStdYL = 0x34;
const uint8_t kOtosRegPosStdYH = 0x35;
const uint8_t kOtosRegPosStdHL = 0x36;
const uint8_t kOtosRegPosStdHH = 0x37;
const uint8_t kOtosRegVelStdXL = 0x38;
const uint8_t kOtosRegVelStdXH = 0x39;
const uint8_t kOtosRegVelStdYL = 0x3A;
const uint8_t kOtosRegVelStdYH = 0x3B;
const uint8_t kOtosRegVelStdHL = 0x3C;
const uint8_t kOtosRegVelStdHH = 0x3D;
const uint8_t kOtosRegAccStdXL = 0x3E;
const uint8_t kOtosRegAccStdXH = 0x3F;
const uint8_t kOtosRegAccStdYL = 0x40;
const uint8_t kOtosRegAccStdYH = 0x41;
const uint8_t kOtosRegAccStdHL = 0x42;
const uint8_t kOtosRegAccStdHH = 0x43;

const uint8_t kOtosRegLsmGyrXL = 0x44;
const uint8_t kOtosRegLsmGyrXH = 0x45;
const uint8_t kOtosRegLsmGyrYL = 0x46;
const uint8_t kOtosRegLsmGyrYH = 0x47;
const uint8_t kOtosRegLsmGyrZL = 0x48;
const uint8_t kOtosRegLsmGyrZH = 0x49;
const uint8_t kOtosRegLsmAccXL = 0x4A;
const uint8_t kOtosRegLsmAccXH = 0x4B;
const uint8_t kOtosRegLsmAccYL = 0x4C;
const uint8_t kOtosRegLsmAccYH = 0x4D;
const uint8_t kOtosRegLsmAccZL = 0x4E;
const uint8_t kOtosRegLsmAccZH = 0x4F;

const uint8_t kOtosRegPaaBurst00 = 0x50;
const uint8_t kOtosRegPaaBurst01 = 0x51;
const uint8_t kOtosRegPaaBurst02 = 0x52;
const uint8_t kOtosRegPaaBurst03 = 0x53;
const uint8_t kOtosRegPaaBurst04 = 0x54;
const uint8_t kOtosRegPaaBurst05 = 0x55;
const uint8_t kOtosRegPaaBurst06 = 0x56;
const uint8_t kOtosRegPaaBurst07 = 0x57;
const uint8_t kOtosRegPaaBurst08 = 0x58;
const uint8_t kOtosRegPaaBurst09 = 0x59;
const uint8_t kOtosRegPaaBurst10 = 0x5A;
const uint8_t kOtosRegPaaBurst11 = 0x5B;

// Version register bit fields
typedef union {
    struct
    {
        uint8_t minor : 4;
        uint8_t major : 4;
    };
    uint8_t value;
} sfe_otos_version_t;

// Reset register bit fields
typedef union {
    struct
    {
        uint8_t resetTracking : 1;
        uint8_t reserved : 7;
    };
    uint8_t value;
} sfe_otos_config_reset_t;

// Signal process register bit fields
typedef union {
    struct
    {
        uint8_t enLut : 1;
        uint8_t enAcc : 1;
        uint8_t enRot : 1;
        uint8_t enVar : 1;
        uint8_t reserved : 4;
    };
    uint8_t value;
} sfe_otos_config_signal_process_t;

// Self test register bit fields
typedef union {
    struct
    {
        uint8_t start : 1;
        uint8_t inProgress : 1;
        uint8_t pass : 1;
        uint8_t fail : 1;
        uint8_t reserved : 4;
    };
    uint8_t value;
} sfe_otos_config_self_test_t;

// Status register bit fields
typedef union {
    struct
    {
        uint8_t warnTiltAngle : 1;
        uint8_t warnOpticalTracking : 1;
        uint8_t reserved : 4;
        uint8_t errorPaa : 1;
        uint8_t errorLsm : 1;
    };
    uint8_t value;
} sfe_otos_status_t;

// Fixed product ID
const uint8_t kOtosProdID = 0x5F;

// Hardware and firmware versions
const sfe_otos_version_t kOtosHwVersion =
{
    .minor = 1,
    .major = 1
};
const sfe_otos_version_t kOtosFwVersion =
{
    .minor = 0,
    .major = 1
};

// I2C register buffers. There is 1 main copy and 2 shadow copies in order to
// minimize the risk of race conditions. The OTOS device and the host write to
// their respective shadow copies, and the main copy is only updated when the
// host is not reading or writing to minimize the chance of race conditions.
#define kNumRegisters 96
extern uint8_t registerBufferMain[kNumRegisters];
extern uint8_t registerShadowHost[kNumRegisters];
extern uint8_t registerShadowSelf[kNumRegisters];

// Flags to know if the shadow registers have been written to
extern bool registerShadowHostWritten[kNumRegisters];
extern bool shadowHostWritten;
extern bool shadowSelfWritten;

// Index of the register being accessed by the host
extern volatile uint8_t registerIndex;

// Helper pointers to registers with bit fields
extern sfe_otos_config_reset_t *registerReset;
extern sfe_otos_config_self_test_t *registerSelfTest;
extern sfe_otos_config_signal_process_t *registerSignalProcess;
extern sfe_otos_status_t *registerStatus;

// Flag to know if the LSM6DSO has new data ready
extern volatile bool lsmDrdy;

// Helper function to reset all registers to their default values
void resetRegisters();
