// Headlight trim program
//
// Hardware:
// The headlight trimmer consists of an Arduino Pro Trinket, a stepper motor 
// attached to the headlight trim adjustment skrew via a worm drive, the 
// stepper motor driver, and an up-neutral-down button.
// 
// Behavior:
// When the up button is pressed, the motor turns the headlight trim adjustment 
// skrew in one direction. When the down button is pressed, it turns it in the 
// other direction.
//
// Copyright 2017 Alexander Schaefer

// Include libraries. //////////////////////////////////////////////////////////
// Button handling and debouncing.
// See https://github.com/JChristensen/Button.
#include "Button.h"

// Stepper motor driver with acceleration and deceleration.
#include "AccelStepper.h"

// Global constants and variables. /////////////////////////////////////////////
// Pin numbers.
const byte buttonUpPin = 3;
const byte buttonDownPin = 2;
const byte wakePin = 0;
const byte stepPin = 1;
const byte dirPin = 4;

// Step rate at maximum speed [Hz].
const unsigned long maxSpeed = 23 * 200 / 3;

// Acceleration duration to maximum speed.
const double accelDuration = 1.0 / 3.0;

// Duration needed to wake the motor driver and to set it asleep [us].
const unsigned int wakeDuration = 1500u;
const unsigned int sleepDuration = 100u;

// End stop of the stepper motor.
const long maxPosition = 1.5 * 200 * 23;
const long upperPosition = +maxPosition;
const long lowerPosition = -maxPosition;

// Stepper motor.
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

// Buttons.
const boolean pullup = true;
const boolean invert = true;
const int debounceTime = 50; // [ms]
Button buttonUp(buttonUpPin, pullup, invert, debounceTime);
Button buttonDown(buttonDownPin, pullup, invert, debounceTime);

// Functions. //////////////////////////////////////////////////////////////////
// Set up the controller after boot.
void setup()
{
    // Initialize the output pins.
    pinMode(wakePin, OUTPUT);
    pinMode(stepPin, OUTPUT);

    // Initialize the stepper motor.
    stepper.setMaxSpeed(maxSpeed);
    stepper.setAcceleration(maxSpeed / accelDuration);
}

// Infinite worker loop.
void loop()
{
    // Read the states of the buttons.
    buttonUp.read();
    buttonDown.read();
    
    // Activate the motor.
    if (buttonUp.wasPressed() || buttonDown.wasPressed())
    {
        // Wake the motor driver.
        digitalWrite(wakePin, HIGH);
        delayMicroseconds(wakeDuration);
    }

    // Set the desired motor position.
    if (buttonUp.wasPressed())
        stepper.moveTo(upperPosition);
    else if (buttonDown.isPressed())
        stepper.moveTo(lowerPosition);

    // Turn the motor.
    stepper.run();

    // Deactivate the motor driver.
    if (buttonUp.wasReleased() || buttonDown.wasReleased())
    {
        // Send the motor driver to sleep.
        stepper.stop();
        digitalWrite(wakePin, LOW);
        delayMicroseconds(sleepDuration);
    }
}

