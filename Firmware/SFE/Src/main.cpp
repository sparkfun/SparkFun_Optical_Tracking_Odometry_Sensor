/*
    SPDX-License-Identifier: MIT
    
    Copyright (c) 2024 SparkFun Electronics
*/

#include "main.h"
#include "otosLsm6dso.h"
#include "otosPaa5160.h"
#include "registers.h"
#include "otosConstants.h"
#include "time.h"
#include "pose2d.h"
#include "adc.h"
#include "otosLut.h"
#include "otosKalmanFilter.h"
#include "arm_math.h"

// Helper function to calibrate the IMU. Must be stationary during calibration!
void calibrateImu(uint8_t numSamples);

// Helper function to get the latest sensor data and update the pose
void sensorUpdate();

// Helper function to update the main register copy from the self shadow
void copyRegsFromSelfShadow();

// Helper function to update the main register copy from the host shadow
void copyRegsFromHostShadow();

// Instances of the sensors
PAA5160 paa;
LSM6DSO lsm;

// Offset between OTOS and host, and its inverse
pose2d posHost;
pose2d posOffset;
pose2d posOffsetInv;

// IMU offset values
float accOffsetX = 0;
float accOffsetY = 0;
float gyrOffsetX = 0;
float gyrOffsetY = 0;
float gyrOffsetZ = 0;

// Approximate roll and pitch angles, used to determine whether accelerometer
// data can be trusted due to too much tilt of the sensor
float roll = 0;
float pitch = 0;

// Previous loop time in order to calculate delta time
uint32_t lastLoopTime = 0;

// Variable to track errors by function calls
sfeTkError_t err = 0;

// Scaler correction values
float userScalarLinear = 1.0f;
float userScalarAngular = 1.0f;

// Kalmam filters for x and y axes
otosKalmanFilter xKf(1e-8, 1e-6, 1e-4);
otosKalmanFilter yKf(1e-8, 1e-6, 1e-4);
otosKalmanFilter hKf(1e-10, 1e-7, 1e-1);

void setup(void)
{
    // Enable SysTick interrupt, which fires every millisecond and is used to
    // track the system time (see SysTick_Handler() in stm32c0xx_it.c)
    LL_SYSTICK_EnableIT();

    // Enable SPI. For some reason this isn't done by the code generator?
    LL_SPI_Enable(SPI1);

    // Reset I2C registers to ensure they are in a known state
    resetRegisters();

    // Enable the address match, NACK, and STOP interrupts for I2C. The TX and
    // RX interrupts are enabled dynamically when these 3 interrupts are
    // triggered (see I2C1_IRQHandler() in stm32c0xx_it.c)
    LL_I2C_EnableIT_ADDR(I2C1);
    LL_I2C_EnableIT_NACK(I2C1);
    LL_I2C_EnableIT_STOP(I2C1);

    // Calibrate and enable the ADC, which is used to check that the PAA5160 is
    // actually receiving power
    calibrateAndEnableADC();

    // Begin the PAA5160. This should be done before the LSM6DS0, because the
    // ESD diodes on the PAA5160's SPI pins will back-power it, which we don't
    // want. The begin() will power up the PAA5160, after which we can begin
    // the LSM6DS0 safely
    err = paa.begin();
    if(err != kSTkErrOk)
        registerStatus->errorPaa = 1;

    // Now we can begin the LSM6DS0
    err = lsm.begin();
    if(err != kSTkErrOk)
        registerStatus->errorLsm = 1;

    // Ensure the orientation of the PAA5160 is correct, it defaults to one axis
    // being inverted
    sfe_paa5160_orientation_t orientation;
    orientation.invertX = 0;
    orientation.invertY = 0;
    orientation.swapXY = 0;
    err = paa.setOrientation(orientation);
    if(err != kSTkErrOk)
        registerStatus->errorPaa = 1;

    // Set PAA5160 resolution to max (20k dpi)
    err = paa.setResolution(kPaa5160MaxResolution, kPaa5160MaxResolution);
    if(err != kSTkErrOk)
        registerStatus->errorPaa = 1;

    // Set range and data rate for accelerometer and gyro as high as we can, to
    // avoid clipping, and to see short data spikes. 16g and 2000dps is the max
    // of the LSM6DS0, and 416Hz is the max data rate that this application on
    // this MCU can reasonably handle
    err = lsm.setAccelRange(kLsm6dsoAccelRange16g);
    if(err != kSTkErrOk)
        registerStatus->errorLsm = 1;
    err = lsm.setGyroRange(kLsm6dsoGyroRange2000Dps);
    if(err != kSTkErrOk)
        registerStatus->errorLsm = 1;
    err = lsm.setAccelDataRate(kLsm6dsoOdr416Hz);
    if(err != kSTkErrOk)
        registerStatus->errorLsm = 1;
    err = lsm.setGyroDataRate(kLsm6dsoOdr416Hz);
    if(err != kSTkErrOk)
        registerStatus->errorLsm = 1;

    // Enable gyro data ready interrupt, which we will use to synchronize the
    // data from the PAA5160 and LSM6DS0
    err = lsm.setInterrupts(kLsm6dsoInt1Pin, kLsm6dsoInt1SrcDrdyG);
    if(err != kSTkErrOk)
        registerStatus->errorLsm = 1;

    // Testing has shown that the initial data from the LSM6DS0 seems to
    // "oscillate" just after powering on for some reason, despite the sensor
    // being motionless. This typically settles within 100ms, so we'll wait for
    // that to happen before we start calibrating the gyro
    delay(100);

    // Perform a quick calibration of the gyro. This should really only be done
    // when motionless, and we don't know if we are or not. But we'll do it
    // anyway since there's a decent chance that we are motionless if we just
    // reset. Hopefully the user will trigger a proper calibration later!
    calibrateImu(64);
}

void loop()
{
    // Update registers from self shadow registers if needed
    copyRegsFromSelfShadow();

    // Update registers from host shadow registers if needed
    copyRegsFromHostShadow();

    // Update sensor data if needed
    sensorUpdate();
}

void calibrateImu(uint8_t numSamples)
{
    // Variables to store the running sum of raw IMU data, which we'll use to
    // compute the average
    int32_t accSumX = 0;
    int32_t accSumY = 0;
    int32_t gyrSumX = 0;
    int32_t gyrSumY = 0;
    int32_t gyrSumZ = 0;

    // Loop until we get the requested number of samples
    uint8_t counter = numSamples;
    uint32_t timeoutStart = millis();
    while (counter > 0)
    {
        // Check if we've waited for too long (should only be 2.4ms at 416Hz)
        if(millis() - timeoutStart > 10)
        {
            // Timeout occurred, set flag and break out of loop
            registerStatus->errorLsm = 1;
            break;
        }

        // Wait for next data to become available from IMU
        if (!lsmDrdy)
            continue;

        // Reset the data ready flag for the next measurement
        lsmDrdy = false;

        // Reset timeout
        timeoutStart = millis();

        // Grab the IMU data. We use the raw data and convert it later for
        // better efficiency and numerical accuracy
        uint8_t lsmData[12];
        err = lsm.readRawAccelGyroAll(lsmData, 12);
        if(err != kSTkErrOk)
            registerStatus->errorLsm = 1;
        
        // Combine raw bytes into 16-bit signed integers
        int16_t gyrXRaw = (lsmData[1] << 8) | lsmData[0];
        int16_t gyrYRaw = (lsmData[3] << 8) | lsmData[2];
        int16_t gyrZRaw = (lsmData[5] << 8) | lsmData[4];
        int16_t accXRaw = (lsmData[7] << 8) | lsmData[6];
        int16_t accYRaw = (lsmData[9] << 8) | lsmData[8];

        // Add the IMU data to the running sums
        accSumX += accXRaw;
        accSumY += accYRaw;
        gyrSumX += gyrXRaw;
        gyrSumY += gyrYRaw;
        gyrSumZ += gyrZRaw;

        // Decrement the sample counter
        counter--;

        // Update register to inform host of progress
        registerBufferMain[kOtosRegImuCalib] = counter;
    }

    // Reset the last loop time to avoid a large delta time in the main loop
    lastLoopTime = micros();

    // Compute the averages in m/s^2 and radians/second, and store them as the
    // offsets
    accOffsetX = (accSumX * lsm.rawToMps2()) / numSamples;
    accOffsetY = (accSumY * lsm.rawToMps2()) / numSamples;
    gyrOffsetX = (gyrSumX * lsm.rawToRps()) / numSamples;
    gyrOffsetY = (gyrSumY * lsm.rawToRps()) / numSamples;
    gyrOffsetZ = (gyrSumZ * lsm.rawToRps()) / numSamples;
}

void sensorUpdate()
{
    // Check to see if new data is available from the IMU
    if (!lsmDrdy)
    {
        // No new data, nothing to do
        return;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Sensor measurement (time critical section!)
    ////////////////////////////////////////////////////////////////////////////

    // We have new data! Set the output pin to indicate that we're now measuring
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9);

    // Immediately update the loop timer to get accurate time
    uint32_t loopTime = micros();

    // Reset the data ready flag before we read from the LSM6DSO, because
    // that clears this interrupt, and we need to be ready for the next one
    lsmDrdy = false;

    // Read data from sensors as soon as possible
    uint8_t paaData[12];
    err = paa.readBurstData(paaData, 12);
    if(err != kSTkErrOk)
        registerStatus->errorPaa = 1;
    uint8_t lsmData[12];
    err = lsm.readRawAccelGyroAll(lsmData, 12);
    if(err != kSTkErrOk)
        registerStatus->errorLsm = 1;

    ////////////////////////////////////////////////////////////////////////////
    // Measurement conversion
    ////////////////////////////////////////////////////////////////////////////
    
    // All the time-critical things are done, so we can do some math now. First
    // let's compute the delta time, and save the last loop time
    float dt = (loopTime - lastLoopTime) * 1e-6f;
    float dtInv = 1.0f / dt;
    lastLoopTime = loopTime;
    
    // Combine raw bytes into 16-bit signed integers
    int16_t deltaXRaw = (paaData[3] << 8) | paaData[2];
    int16_t deltaYRaw = (paaData[5] << 8) | paaData[4];
    int16_t deltaRRaw = (lsmData[1] << 8) | lsmData[0];
    int16_t deltaPRaw = (lsmData[3] << 8) | lsmData[2];
    int16_t deltaHRaw = (lsmData[5] << 8) | lsmData[4];
    int16_t accelXRaw = (lsmData[7] << 8) | lsmData[6];
    int16_t accelYRaw = (lsmData[9] << 8) | lsmData[8];

    // Convert the PAA5160 data to meters
    float deltaX = deltaXRaw * paa.rawToMetersX();
    float deltaY = deltaYRaw * paa.rawToMetersY();

    // Convert the gyro data to radians/second, apply the gyro offset
    float vR = (deltaRRaw * lsm.rawToRps() - gyrOffsetX);
    float vP = (deltaPRaw * lsm.rawToRps() - gyrOffsetY);
    float vH = (deltaHRaw * lsm.rawToRps() - gyrOffsetZ);

    // Convert the accelerometer data to m/s^2, and apply the acceleration
    // offset. The IMU is mounted 90 degrees from the PAA5160, so we need to
    // swap the X and Y axes, and negate Y    
    float accelXMps2 = (accelYRaw * lsm.rawToMps2()) - accOffsetY;
    float accelYMps2 = -((accelXRaw * lsm.rawToMps2()) - accOffsetX);

    ////////////////////////////////////////////////////////////////////////////
    // Lookup table calibration
    ////////////////////////////////////////////////////////////////////////////

    // The resolution of the PAA5160 varies with the surface velocity. A lookup
    // table has been implemented that provides a scaling factor to compensate
    // based on the measured velocities, so compute the measured velocities
    float vx = deltaX * dtInv;
    float vy = deltaY * dtInv;

    // Using the measured velocities, determine the scaling factors for each
    // axis using the lookup table. The lookup table requires inches per second
    float xScalar = 1.0f;
    float yScalar = 1.0f;
    if(registerSignalProcess->enLut)
        bilinearInterp(vx * 39.37f, vy * 39.37f, xScalar, yScalar);

    // Scale the velocities by the lookup table scalars, and apply the user
    // defined scalars
    vx *= xScalar * userScalarLinear;
    vy *= yScalar * userScalarLinear;

    ////////////////////////////////////////////////////////////////////////////
    // Tilt detection
    ////////////////////////////////////////////////////////////////////////////

    // Compute roll and pitch angles approximately. Simple integration like this
    // is not technically the right way to do this, but it's not being used for
    // true orientation detection, just to determine if the sensor has been
    // tiled significantly
    roll += vR * dt;
    pitch += vP * dt;

    // As described below, the optical tracking sensor can't always be trusted,
    // such as when the surface properties change suddenly. The solution there
    // is to ignore the optical tracking data if that's detected, and only use
    // the accelerometer data. This works fine under normal conditions, but if 
    // the user tilts the sensor more than ~20 degrees, the accelerometer data
    // causes a false positive detection of tracking issues, which can cause the
    // estimated velocity and position to explode. So if the tilt angles are
    // beyond a threshold (arbitrarily set to 0.2 radians, or 11.5 degrees),
    // we'll set the status flag to indicate that the tilt angle is too high,
    // and the accelerometer data will be ignored later
    if(abs(roll) > 0.2f || abs(pitch) > 0.2f)
        registerStatus->warnTiltAngle = 1;
    else
        registerStatus->warnTiltAngle = 0;

    ////////////////////////////////////////////////////////////////////////////
    // Kalmam filter update (angular)
    ////////////////////////////////////////////////////////////////////////////

    // Apply user-defined scalar for heading
    vH *= userScalarAngular;

    // First run the prediction step
    hKf.predict(dt);

    // Now run the update step with the latest measurement
    hKf.updateVel(vH, 1e-3);

    // Wrap heading to +/- pi
    if (hKf.xData[0] >= kPi)
        hKf.xData[0] -= k2Pi;
    if (hKf.xData[0] < -kPi)
        hKf.xData[0] += k2Pi;

    ////////////////////////////////////////////////////////////////////////////
    // Convert measurements to world frame
    ////////////////////////////////////////////////////////////////////////////

    // Pre-compute sin and cos so we don't have to do it multiple times
    float cosHeading = arm_cos_f32(hKf.xData[0]);
    float sinHeading = arm_sin_f32(hKf.xData[0]);

    // Rotate the measured velocity and acceleration by the heading
    if(registerSignalProcess->enRot)
    {
        float vxRot = vx * cosHeading - vy * sinHeading;
        float vyRot = vx * sinHeading + vy * cosHeading;
        float accelXMps2Rot = accelXMps2 * cosHeading - accelYMps2 * sinHeading;
        float accelYMps2Rot = accelXMps2 * sinHeading + accelYMps2 * cosHeading;

        vx = vxRot;
        vy = vyRot;
        accelXMps2 = accelXMps2Rot;
        accelYMps2 = accelYMps2Rot;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Kalman filter update (linear)
    ////////////////////////////////////////////////////////////////////////////

    // First run the prediction step
    xKf.predict(dt);
    yKf.predict(dt);

    // Now run the update steps with the latest measurements. If the tilt angle
    // is sufficiently low, we'll use the data from both the accelerometer and
    // optical sensor, otherwise we'll only use the optical sensor data
    if(registerSignalProcess->enAcc && registerStatus->warnTiltAngle == 0)
    {
        // Acceleration update
        xKf.updateAcc(accelXMps2, 9e-4 * registerSignalProcess->enVar);
        yKf.updateAcc(accelYMps2, 9e-4 * registerSignalProcess->enVar);

        // Sometimes the velocity measured by the PAA5160 drops very low, which
        // is usually caused by tracking issues (eg. debris or a sudden change
        // in surface properties). We can detect this if the measured velocity
        // is significantly lower than the estimated velocity, in which case we
        // can ignore the measurement
        if(!((abs(vx) + abs(vy)) < .25 && (abs(xKf.xData[1]) + abs(yKf.xData[1])) > .5))
        {
            // Velocity update
            xKf.updateVel(vx, 6e-3f * registerSignalProcess->enVar);
            yKf.updateVel(vy, 6e-3f * registerSignalProcess->enVar);

            // Reset the warning flag
            registerStatus->warnOpticalTracking = 0;
        }
        else
        {
            // Set the warning flag
            registerStatus->warnOpticalTracking = 1;
        }
    }
    else
    {
        // Not using the accelerometer data, just do the velocity update
        xKf.updateVel(vx, 6e-3 * registerSignalProcess->enVar);
        yKf.updateVel(vy, 6e-3 * registerSignalProcess->enVar);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Compute host position, velocity, and acceleration
    ////////////////////////////////////////////////////////////////////////////

    // Here we compute the host position, velocity, and acceleration in the
    // world frame. The equations below are very similar to what's used in
    // transformPose(), but we're doing them explicitly here to avoid computing
    // sine and cosine more than needed. Additionally, the velocity and
    // acceleration have to be computed slightly differently

    // Host position
    posHost.x = xKf.xData[0] + (posOffsetInv.x * cosHeading - posOffsetInv.y * sinHeading);
    posHost.y = yKf.xData[0] + (posOffsetInv.x * sinHeading + posOffsetInv.y * cosHeading);
    posHost.h = hKf.xData[0] + posOffsetInv.h;

    // Host velocity
    pose2d velHost;
    velHost.x = xKf.xData[1] + (-posOffsetInv.x * sinHeading - posOffsetInv.y * cosHeading) * hKf.xData[1];
    velHost.y = yKf.xData[1] + (posOffsetInv.x * cosHeading - posOffsetInv.y * sinHeading) * hKf.xData[1];
    velHost.h = hKf.xData[1];

    // Host acceleration
    pose2d accHost;
    float centripetalAcceleration = sqrtf(posOffsetInv.x * posOffsetInv.x + posOffsetInv.y * posOffsetInv.y) * hKf.xData[1] * hKf.xData[1];
    accHost.x = xKf.xData[2] + (-posOffsetInv.x * sinHeading - posOffsetInv.y * cosHeading) * hKf.xData[2]
                - (centripetalAcceleration * cosHeading - centripetalAcceleration * sinHeading);
    accHost.y = yKf.xData[2] + (posOffsetInv.x * cosHeading - posOffsetInv.y * sinHeading) * hKf.xData[2]
                - (centripetalAcceleration * sinHeading + centripetalAcceleration * cosHeading);
    accHost.h = hKf.xData[2];

    ////////////////////////////////////////////////////////////////////////////
    // Update shadow registers
    ////////////////////////////////////////////////////////////////////////////

    // Convert host position, velocity, and acceleration to integers in
    // preparation for storing in the shadow registers
    int16_t pxInt = posHost.x * kMeterToInt16;
    int16_t pyInt = posHost.y * kMeterToInt16;
    int16_t phInt = posHost.h * kRadToInt16;
    int16_t vxInt = velHost.x * kMpsToInt16;
    int16_t vyInt = velHost.y * kMpsToInt16;
    int16_t vhInt = velHost.h * kRpsToInt16;
    int16_t axInt = accHost.x * kMpssToInt16;
    int16_t ayInt = accHost.y * kMpssToInt16;
    int16_t ahInt = accHost.h * kRpssToInt16;

    // Compute standard deviation of the Kalman filter covariance matrices
    int16_t pxStdInt = sqrtf(xKf.pData[kOtosKfIdx00]) * kMeterToInt16;
    int16_t pyStdInt = sqrtf(yKf.pData[kOtosKfIdx00]) * kMeterToInt16;
    int16_t phStdInt = sqrtf(hKf.pData[kOtosKfIdx00]) * kRadToInt16;
    int16_t vxStdInt = sqrtf(xKf.pData[kOtosKfIdx11]) * kMpsToInt16;
    int16_t vyStdInt = sqrtf(yKf.pData[kOtosKfIdx11]) * kMpsToInt16;
    int16_t vhStdInt = sqrtf(hKf.pData[kOtosKfIdx11]) * kRpsToInt16;
    int16_t axStdInt = sqrtf(xKf.pData[kOtosKfIdx22]) * kMpssToInt16;
    int16_t ayStdInt = sqrtf(yKf.pData[kOtosKfIdx22]) * kMpssToInt16;
    int16_t ahStdInt = sqrtf(hKf.pData[kOtosKfIdx22]) * kRpssToInt16;

    // Now store the raw register data into the shadow buffer
    registerShadowSelf[kOtosRegPosXL] = pxInt & 0xFF;
    registerShadowSelf[kOtosRegPosXH] = (pxInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegPosYL] = pyInt & 0xFF;
    registerShadowSelf[kOtosRegPosYH] = (pyInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegPosHL] = phInt & 0xFF;
    registerShadowSelf[kOtosRegPosHH] = (phInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegVelXL] = vxInt & 0xFF;
    registerShadowSelf[kOtosRegVelXH] = (vxInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegVelYL] = vyInt & 0xFF;
    registerShadowSelf[kOtosRegVelYH] = (vyInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegVelHL] = vhInt & 0xFF;
    registerShadowSelf[kOtosRegVelHH] = (vhInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegAccXL] = axInt & 0xFF;
    registerShadowSelf[kOtosRegAccXH] = (axInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegAccYL] = ayInt & 0xFF;
    registerShadowSelf[kOtosRegAccYH] = (ayInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegAccHL] = ahInt & 0xFF;
    registerShadowSelf[kOtosRegAccHH] = (ahInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegPosStdXL] = pxStdInt & 0xFF;
    registerShadowSelf[kOtosRegPosStdXH] = (pxStdInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegPosStdYL] = pyStdInt & 0xFF;
    registerShadowSelf[kOtosRegPosStdYH] = (pyStdInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegPosStdHL] = phStdInt & 0xFF;
    registerShadowSelf[kOtosRegPosStdHH] = (phStdInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegVelStdXL] = vxStdInt & 0xFF;
    registerShadowSelf[kOtosRegVelStdXH] = (vxStdInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegVelStdYL] = vyStdInt & 0xFF;
    registerShadowSelf[kOtosRegVelStdYH] = (vyStdInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegVelStdHL] = vhStdInt & 0xFF;
    registerShadowSelf[kOtosRegVelStdHH] = (vhStdInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegAccStdXL] = axStdInt & 0xFF;
    registerShadowSelf[kOtosRegAccStdXH] = (axStdInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegAccStdYL] = ayStdInt & 0xFF;
    registerShadowSelf[kOtosRegAccStdYH] = (ayStdInt >> 8) & 0xFF;
    registerShadowSelf[kOtosRegAccStdHL] = ahStdInt & 0xFF;
    registerShadowSelf[kOtosRegAccStdHH] = (ahStdInt >> 8) & 0xFF;

    // Also store the raw LSM6DSO and PAA5160 data into the shadow buffer
    memcpy(registerShadowSelf + kOtosRegLsmGyrXL, lsmData, 12);
    memcpy(registerShadowSelf + kOtosRegPaaBurst00, paaData, 12);

    // Update flag to indicate the shadow registers have been written to
    shadowSelfWritten = true;
}

void copyRegsFromSelfShadow()
{
    // Check if shadow registers have been written to
    if(!shadowSelfWritten)
    {
        // Shadow registers have not been written to, nothing to do
        return;
    }

    // Shadow registers have been written to, check to see if host is currently
    // reading from the registers
    if (LL_I2C_IsActiveFlag_BUSY(I2C1) && (LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_READ))
    {
        // Host is current reading, so avoid a race condition by doing nothing
        return;
    }

    // Ok, we're safe to quickly copy the data over!
    memcpy(registerBufferMain + kOtosRegPosXL, registerShadowSelf + kOtosRegPosXL, kOtosRegPaaBurst11 - kOtosRegPosXL + 1);

    // Set the output pin to indicate that new data is available
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);

    // Now reset the flag
    shadowSelfWritten = false;
}

bool updatePoseFromHostShadow(uint8_t startReg, pose2d &pose, float linearResolution, float angularResolution)
{
    // Loop through each pose register to see whether they were all written
    bool allRegsWritten = true;
    for (uint8_t i = startReg; i < startReg + 6; i++)
    {
        // Check if the register was written to
        allRegsWritten &= registerShadowHostWritten[i];

        // Reset the register written flag
        registerShadowHostWritten[i] = false;
    }

    // Check whether all registers were written
    if(allRegsWritten == false)
    {
        // We don't want to update the value with only some of the data, so we
        // won't do anything more
        return false;
    }

    // All the registers were written, copy the raw values
    int16_t xRaw = (registerShadowHost[startReg + 1] << 8) | registerShadowHost[startReg];
    int16_t yRaw = (registerShadowHost[startReg + 3] << 8) | registerShadowHost[startReg + 2];
    int16_t hRaw = (registerShadowHost[startReg + 5] << 8) | registerShadowHost[startReg + 4];

    // Convert to specified units and store in the pose
    pose.x = xRaw * linearResolution;
    pose.y = yRaw * linearResolution;
    pose.h = hRaw * angularResolution;

    // Done!
    return true;
}

void copyRegsFromHostShadow()
{
    // Check if shadow registers have been written to
    if (!shadowHostWritten)
    {
        // Shadow registers have not been written to, nothing to do
        return;
    }
    
    // Shadow registers have been written to, check to see if host is currently
    // writing to the registers
    if (LL_I2C_IsActiveFlag_BUSY(I2C1) && (LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_WRITE))
    {
        // Host is current writing, so avoid a race condition by doing nothing
        return;
    }

    // Ok, we're safe to update stuff! First reset the global host written flag
    shadowHostWritten = false;

    // Check if the reset register got changed by the host
    if (registerShadowHostWritten[kOtosRegReset])
    {
        // Copy the value to the main buffer and reset the host written flag
        registerBufferMain[kOtosRegReset] = registerShadowHost[kOtosRegReset];
        registerShadowHostWritten[kOtosRegReset] = false;

        // Check if the reset trackgin bit is set
        if(registerReset->resetTracking)
        {
            // Reset the Kalman filters
            xKf.reset();
            yKf.reset();
            hKf.reset();
            
            // Compute new self position, assuming the host is at (0, 0, 0)
            pose2d posSelf;
            posHost.x = 0;
            posHost.y = 0;
            posHost.h = 0;
            posSelf = transformPose(posHost, posOffset);

            // Update the Kalman filters with the new self position
            xKf.xData[0] = posSelf.x;
            yKf.xData[0] = posSelf.y;
            hKf.xData[0] = posSelf.h;

            // Reset the roll and pitch angles
            roll = 0;
            pitch = 0;
        }

        // Reset the register to indicate that the reset has been completed
        registerBufferMain[kOtosRegReset] = 0;
    }

    // Check if the signal process register got changed by the host
    if (registerShadowHostWritten[kOtosRegSignalProcess])
    {
        // Copy the value to the main buffer and reset the host written flag
        registerBufferMain[kOtosRegSignalProcess] = registerShadowHost[kOtosRegSignalProcess];
        registerShadowHostWritten[kOtosRegSignalProcess] = false;
    }

    // Check if the self test register got changed by the host
    if (registerShadowHostWritten[kOtosRegSelfTest])
    {
        // Copy the value to a struct and reset the host written flag
        sfe_otos_config_self_test_t selfTest;
        selfTest.value = registerShadowHost[kOtosRegSelfTest];
        registerShadowHostWritten[kOtosRegSelfTest] = false;

        // Check if the start bit was set
        if(selfTest.start)
        {
            // Set the register to indicate that the self test is in progress
            selfTest.value = 0;
            selfTest.inProgress = 1;
            registerBufferMain[kOtosRegSelfTest] = selfTest.value;

            // Perform the self test
            sfeTkError_t result = lsm.selfTest();

            // Update the self test register with the result
            if(result == kSTkErrOk)
                selfTest.pass = 1;
            else
                selfTest.fail = 1;
            selfTest.inProgress = 0;
            registerBufferMain[kOtosRegSelfTest] = selfTest.value;
            
            // Reset the last loop time to avoid a large delta time in main loop
            lastLoopTime = micros();
        }
    }

    // Check if the IMU calibration register got changed by the host
    if (registerShadowHostWritten[kOtosRegImuCalib])
    {
        // Copy the value to the main buffer and reset the host written flag
        registerBufferMain[kOtosRegImuCalib] = registerShadowHost[kOtosRegImuCalib];
        registerShadowHostWritten[kOtosRegImuCalib] = false;

        // Perform the IMU calibration with the requested number of samples
        calibrateImu(registerBufferMain[kOtosRegImuCalib]);
    }

    // Check if the linear scalar register got changed by the host
    if (registerShadowHostWritten[kOtosRegScalarLinear])
    {
        // Copy the value to the main buffer and reset the host written flag
        registerBufferMain[kOtosRegScalarLinear] = registerShadowHost[kOtosRegScalarLinear];
        registerShadowHostWritten[kOtosRegScalarLinear] = false;

        // Convert the scalar to a float
        userScalarLinear = ((int8_t)registerShadowHost[kOtosRegScalarLinear]) * 0.001f + 1.0f;
    }

    // Check if the angular scalar register got changed by the host
    if (registerShadowHostWritten[kOtosRegScalarAngular])
    {
        // Copy the value to the main buffer and reset the host written flag
        registerBufferMain[kOtosRegScalarAngular] = registerShadowHost[kOtosRegScalarAngular];
        registerShadowHostWritten[kOtosRegScalarAngular] = false;

        // Convert the scalar to a float
        userScalarAngular = ((int8_t)registerShadowHost[kOtosRegScalarAngular]) * 0.001f + 1.0f;
    }

    // Check if the position or offset registers got changed by the host
    bool poseUpdated = false;
    poseUpdated |= updatePoseFromHostShadow(kOtosRegPosXL, posHost, kInt16ToMeter, kInt16ToRad);
    poseUpdated |= updatePoseFromHostShadow(kOtosRegOffXL, posOffset, kInt16ToMeter, kInt16ToRad);
    if (poseUpdated)
    {
        // Either the host position or offset was changed, so we need to update
        // the Kalman filters with the new self position to reflect the new
        // host position and/or offset
        pose2d poseSelf;
        poseSelf = transformPose(posHost, posOffset);
        xKf.xData[0] = poseSelf.x;
        yKf.xData[0] = poseSelf.y;
        hKf.xData[0] = poseSelf.h;

        // Also store a copy of the inverse offset for later use
        posOffsetInv = invertPose(posOffset);
    }
}
