/*
    SPDX-License-Identifier: MIT

    Copyright (c) 2024 SparkFun Electronics
*/
#include "stepper.h"
#include "SparkFun_Qwiic_OTOS_Arduino_Library.h"
#include "Wire.h"

// List of available tests
typedef enum
{
    OTOS_ANGLE,
    DISC_RAMP,
    ZERO_ANGLE,
    MOUNT_CENTERING,
    DATA_COLLECTION,
} otos_test_t;

// Which test to run
otos_test_t currentTest = MOUNT_CENTERING;

// Number of samples to average for velocity measurements
int numSamples = 8;

// You may need to adjust this value to get the OTOS angle to zero out properly
float correctionAngle = 0;

// Maximum speed for the mounting test. Default is 100 in/sec, though this could
// be reduced to shorten the test time.
int mountTestMaxSpeed = 100;

// Maximum speed for the lookup table test. Recommend 120 in/sec, since the max
// speed of the optical sensor is 100 in/sec, but the resolution of the sensor
// changes at high speed, so at least 120 is needed to get the measured velocity
// up to 100 in/sec.
int lutMaxSpeed = 120;

// Disable signal processing to get just raw optical sensor data
sfe_otos_signal_process_config_t signalProcessConfig =
{
    .enLut = 0, // Disable lookup table
    .enAcc = 0, // Disable acceleration data
    .enRot = 0, // Disable rotating the linear data by the angle
    .enVar = 0, // Disable measurement variance to get raw data
    .reserved = 0 // Reserved bits, leave as zero
};

// Create an Optical Tracking Odometry Sensor object
QwiicOTOS myOtos;

// Pinout for disc stepper
int enablePinDiscStepper = 25;
int standbyPinDiscStepper = 4;
int mode0PinDiscStepper = 17;
int mode1PinDiscStepper = 16;
int mode2PinDiscStepper = 14;
int mode3PinDiscStepper = 13;

// Pinout for OTOS stepper
int enablePinOtosStepper = 19;
int standbyPinOtosStepper = 18;
int mode0PinOtosStepper = 23;
int mode1PinOtosStepper = 5;
int mode2PinOtosStepper = 27;
int mode3PinOtosStepper = 26;

// Distance between the two steppers
float radius = 1.75f;

// Stepper configurations
stepperConfig discStepperConfig = {enablePinDiscStepper,
                                   standbyPinDiscStepper,
                                   mode0PinDiscStepper,
                                   mode1PinDiscStepper,
                                   mode2PinDiscStepper,
                                   mode3PinDiscStepper,
                                   radius};
stepperConfig otosStepperConfig = {enablePinOtosStepper,
                                   standbyPinOtosStepper,
                                   mode0PinOtosStepper,
                                   mode1PinOtosStepper,
                                   mode2PinOtosStepper,
                                   mode3PinOtosStepper,
                                   radius};

// Create the stepper motors
Stepper discStepper(discStepperConfig);
Stepper otosStepper(otosStepperConfig);

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("Qwiic OTOS LUT Calibration");

    // Start I2C
    Wire.begin();

    // Set the I2C clock to 1MHz for faster data transfer
    Wire.setClock(1000000);

    // Attempt to begin the sensor
    while (myOtos.begin() == false)
    {
        Serial.println("OTOS not connected, check your wiring and I2C address!");
        delay(1000);
    }

    Serial.println("OTOS connected!");

    // Set the signal processing configuration
    myOtos.setSignalProcessConfig(signalProcessConfig);

    // Reset the tracking algorithm
    myOtos.resetTracking();

    // Initialize the steppers
    discStepper.init();
    otosStepper.init();

    // Run the specified test
    switch (currentTest)
    {
        case OTOS_ANGLE:
            // Rotate the OTOS to 90 degrees
            otosStepper.moveToAngle(90);
            delay(500);
            break;
        case DISC_RAMP:
            // Ramp the disc stepper to the max speed expected during LUT test
            otosStepper.rampToSpeed(lutMaxSpeed * 1.414);
            break;
        case ZERO_ANGLE:
            // Find the zero angle for the OTOS
            findZeroAngle();
            break;
        case MOUNT_CENTERING:
            // Find the zero angle for the OTOS, then run the mounting test
            findZeroAngle();
            mountingTest();
            break;
        case DATA_COLLECTION:
            // Find the zero angle for the OTOS, then run the resolution test
            findZeroAngle();
            lutTest();
            break;
        default:
            break;
    }

    // Print done message to indicate the test is complete
    Serial.println("Done!");

    // Stop the disc stepper, and reset the OTOS stepper angle
    discStepper.rampToSpeed(0);
    otosStepper.moveToAngle(0);

    // Deinitialize the steppers
    discStepper.deinit();
    otosStepper.deinit();
}

void loop()
{
    // Nothing to do here
}

void getAveragedVelocity(float &vx, float &vy, int n)
{
    // Reset the velocity values
    vx = 0;
    vy = 0;

    // Loop through n samples
    for (int i = 0; i < n; i++)
    {
        // Reading data from the OTOS can actually cause a slight delay in the
        // main loop of the firmware, meaning the following velocity measurement
        // could be slightly off. So we'll wait 2 measurement periods (2.4ms
        // each) to ensure the velocity is accurate.
        delay(5);

        // Get the velocity from the OTOS
        sfe_otos_pose2d_t vel;
        myOtos.getVelocity(vel);

        // Add the velocity to the total
        vx += vel.x;
        vy += vel.y;
    }

    // Compute the average velocity
    vx /= n;
    vy /= n;
}

void findZeroAngle()
{
    // Ramp the disc stepper up to some speed and wait for it to stabilize
    discStepper.rampToSpeed(90);
    delay(200);

    // Initialize measurement variables
    float vx = 0;
    float vy = 0;
    float angleError = 0;

    // Measure the angle error and move the OTOS to correct it
    getAveragedVelocity(vx, vy, 8);
    angleError = atan2(vy, vx) * 180 / 3.1415926;
    otosStepper.moveToAngle(-angleError);
    otosStepper.zeroAngle();

    // Wait for the OTOS to stabilize
    delay(200);

    // Measure the angle error again and move the OTOS to correct it
    getAveragedVelocity(vx, vy, 128);
    angleError = atan2(vy, vx) * 180 / 3.1415926;
    otosStepper.moveToAngle(-angleError);
    otosStepper.zeroAngle();

    // Move to correction angle
    otosStepper.moveToAngle(correctionAngle);
    otosStepper.zeroAngle();

    // Ramp the disc stepper back to zero speed
    discStepper.rampToSpeed(0);
}

void mountingTest()
{
    // Loop through orthogonal angles
    for (int i = 0; i <= 270; i += 90)
    {
        // Print the new angle
        Serial.printf("New angle: %i\n", i);

        // Move the OTOS to the new angle
        otosStepper.moveToAngle(i);

        // Loop through speeds
        for (int j = -mountTestMaxSpeed; j <= mountTestMaxSpeed; j+=1)
        {
            // Ramp the disc stepper to the new speed
            discStepper.rampToSpeed(j);

            // Measure the velocity at this speed
            float vx = 0;
            float vy = 0;
            getAveragedVelocity(vx, vy, numSamples);

            // Print results
            Serial.printf("%.3f\n", i % 180 == 0 ? vx : vy);
        }
    }
}

void lutTest()
{
    // Loop through grid of points
    for (int j = -lutMaxSpeed; j <= lutMaxSpeed; j += 1)
    {
        for (int i = -lutMaxSpeed; i <= lutMaxSpeed; i += 1)
        {
            // Set speed and angle
            float newSpeed = sqrt(i * i + j * j);
            float newAngle = atan2(j, i) * 180 / 3.1415926;
            otosStepper.moveToAngle(newAngle);
            discStepper.rampToSpeed(newSpeed);

            // Measure the velocity at this speed
            float vx = 0;
            float vy = 0;
            getAveragedVelocity(vx, vy, numSamples);

            // Print results
            Serial.printf("%i\t%i\t%.3f\t%.3f\n", i, j, vx, vy);
        }
    }
}
