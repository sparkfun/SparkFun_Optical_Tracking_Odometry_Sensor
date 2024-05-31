#pragma once

#include "Arduino.h"

// Configuration for the stepper motor
struct stepperConfig
{
    // Pinout for the stepper motor
    int enablePin;
    int standbyPin;
    int mode0Pin;
    int mode1Pin;
    int mode2Pin;
    int mode3Pin;

    // Distance between the two steppers, used for velocity calculations
    float radius;
};

// Class for the stepper motor
class Stepper
{
public:
    // Constructor
    Stepper(stepperConfig &config)
    {
        // Store the configuration
        enablePin = config.enablePin;
        standbyPin = config.standbyPin;
        mode0Pin = config.mode0Pin;
        mode1Pin = config.mode1Pin;
        mode2Pin = config.mode2Pin;
        mode3Pin = config.mode3Pin;
        radius = config.radius;

        // Set initial values
        speed = 0;
        angle = 0;
        freqToIPS = 1.0f / 200.0f * 2.0f * 3.1415926 * radius;
        ipsToFreq = 1.0f / freqToIPS;
    }

    // Initialize the stepper motor
    void init()
    {
        // Set the pins as outputs
        pinMode(enablePin, OUTPUT);
        pinMode(standbyPin, OUTPUT);
        pinMode(mode0Pin, OUTPUT);
        pinMode(mode1Pin, OUTPUT);
        pinMode(mode2Pin, OUTPUT);
        pinMode(mode3Pin, OUTPUT);

        // Ensure the stepper is disabled and in standby
        digitalWrite(enablePin, LOW);
        digitalWrite(standbyPin, LOW);

        // Set microstepping to 32
        digitalWrite(mode0Pin, LOW);
        digitalWrite(mode1Pin, LOW);
        digitalWrite(mode2Pin, HIGH);
        digitalWrite(mode3Pin, HIGH);
        delayMicroseconds(100);
        digitalWrite(standbyPin, HIGH);
        delayMicroseconds(100);

        // Enable the stepper
        digitalWrite(enablePin, HIGH);
    }

    // Deinitialize the stepper motor
    void deinit()
    {
        // Just disable the stepper
        digitalWrite(enablePin, LOW);
        digitalWrite(standbyPin, LOW);
    }

    // Set the zero angle
    void zeroAngle()
    {
        // Reset the angle to zero
        angle = 0;
    }

    // Ramp the speed to a new value
    void rampToSpeed(float newSpeed)
    {
        // Check which direction to ramp the speed
        if (newSpeed > speed)
        {
            // Ramp the speed up
            speed += 1;
            for (; speed <= newSpeed; speed += 1)
            {
                // Set the direction pin, in case we pass through zero
                digitalWrite(mode3Pin, speed >= 0 ? HIGH : LOW);

                // Set the new PWM frequency for this speed
                tone(mode2Pin, abs(speed * 16 * ipsToFreq));

                // Wait for the motor to reach the new speed
                delay(10);
            }
        }
        else
        {
            // Ramp the speed down
            speed -= 1;
            for (; speed >= newSpeed; speed -= 1)
            {
                // Set the direction pin, in case we pass through zero
                digitalWrite(mode3Pin, speed >= 0 ? HIGH : LOW);
                
                // Set the new PWM frequency for this speed
                tone(mode2Pin, abs(speed * 16 * ipsToFreq));

                // Wait for the motor to reach the new speed
                delay(10);
            }
        }

        // Ensure the final speed is set correctly
        tone(mode2Pin, abs(newSpeed * 16 * ipsToFreq));

        // Wait for the motor to reach the final speed
        delay(20);
    }

    // Move the stepper to a new angle
    void moveToAngle(float newAngle)
    {
        // Calculate the number of steps to move
        int stepsToMove = (newAngle / 1.8 * 16) - angle;

        // Set the direction pin based on which way we are moving
        digitalWrite(mode3Pin, stepsToMove >= 0 ? LOW : HIGH);

        // Move this number of steps
        for (int i = 0; i < abs(stepsToMove); i++)
        {
            // Pulse the step pin high then low
            digitalWrite(mode2Pin, HIGH);
            delayMicroseconds(100);
            digitalWrite(mode2Pin, LOW);
            delayMicroseconds(100);
        }

        // Update the angle
        angle += stepsToMove;
    }

private:
    int enablePin;
    int standbyPin;
    int mode0Pin;
    int mode1Pin;
    int mode2Pin;
    int mode3Pin;
    
    float speed;
    int angle;

    float radius;
    float freqToIPS;
    float ipsToFreq;
};