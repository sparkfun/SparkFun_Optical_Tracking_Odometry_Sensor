/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "registers.h"

// Buffers to simulate I2C registers
uint8_t registerBufferMain[kNumRegisters];
uint8_t registerShadowHost[kNumRegisters];
uint8_t registerShadowSelf[kNumRegisters];
bool registerShadowHostWritten[kNumRegisters];
bool shadowHostWritten = false;
bool shadowSelfWritten = false;

// Index of I2C register buffer
volatile uint8_t registerIndex = 0;

// Helper pointers to registers with bit fields
sfe_otos_config_reset_t *registerReset = (sfe_otos_config_reset_t *)registerBufferMain + kOtosRegReset;
sfe_otos_config_self_test_t *registerSelfTest = (sfe_otos_config_self_test_t *)registerBufferMain + kOtosRegSelfTest;
sfe_otos_config_signal_process_t *registerSignalProcess = (sfe_otos_config_signal_process_t *)registerBufferMain + kOtosRegSignalProcess;
sfe_otos_status_t *registerStatus = (sfe_otos_status_t *)registerBufferMain + kOtosRegStatus;

void resetRegisters()
{
    // Reset register index
    registerIndex = 0;

    // Reset shadow register flags
    shadowHostWritten = false;
    shadowSelfWritten = false;

    // Reset all registers to zero
    memset(registerBufferMain, 0, kNumRegisters);
    memset(registerShadowHost, 0, kNumRegisters);
    memset(registerShadowSelf, 0, kNumRegisters);
    memset(registerShadowHostWritten, 0, kNumRegisters);

    // Set constant register values
    registerBufferMain[kOtosRegProdID] = kOtosProdID;
    registerBufferMain[kOtosRegHwVersion] = kOtosHwVersion.value;
    registerBufferMain[kOtosRegFwVersion] = kOtosFwVersion.value;

    // Set default register values
    registerSignalProcess->enLut = 1;
    registerSignalProcess->enAcc = 1;
    registerSignalProcess->enRot = 1;
    registerSignalProcess->enVar = 1;
}
