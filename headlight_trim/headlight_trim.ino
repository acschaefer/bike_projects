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
const byte buttonUpPin = 15;
const byte buttonDownPin = 16;
const byte wakePin = 5;
const byte stepPin = 4;
const byte dirPin = 3;
const byte ledRunPin = 17;
const byte ledErrorPin = 18;
const byte driverStatusPin = 19;

// Step rate at maximum speed [Hz].
const float maxSpeed = 23.0f * 200.0f / 3.0f;

// Acceleration duration to maximum speed.
const float accelDuration = 1.0f / 3.0f;

// Duration needed to wake the motor driver and to set it asleep [us].
const unsigned long wakeDuration = 1500ul;
const unsigned long sleepDuration = 100ul;

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
    // Initialize the motor driver pins.
    pinMode(wakePin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    pinMode(driverStatusPin, INPUT);
    
    // Initialize the status LEDs.
    pinMode(ledRunPin, OUTPUT);
    pinMode(ledErrorPin, OUTPUT);

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
    else if (buttonDown.wasPressed())
        stepper.moveTo(lowerPosition);

    // Turn the motor and activate the motor LED.
    digitalWrite(ledRunPin, stepper.run());

    // Light the error LED.
    digitalWrite(ledErrorPin, !digitalRead(driverStatusPin));

    // Deactivate the motor driver.
    if (buttonUp.wasReleased() || buttonDown.wasReleased())
    {
        // Send the motor driver to sleep.
        stepper.stop();
        digitalWrite(wakePin, LOW);
        delayMicroseconds(sleepDuration);
    }
}

