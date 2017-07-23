// Headlight trim program
//
// Hardware:
// The headlight trimmer consists of an Arduino Trinket, a stepper motor 
// attached to the headlight trim adjustment skrew via a worm drive, the 
// stepper motor driver, and a button.
// 
// Behavior:
// When the up button is pressed, the motor turns the headlight trim adjustment 
// skrew in one direction. When the down button is pressed, it turns it in the 
// other direction.
//
// Copyright 2017 Alexander Schaefer

// Included libraries. /////////////////////////////////////////////////////////
// Library necessary to enable 16 MHz clock frequency.
#include <avr/power.h>

// Button handling and debouncing.
// See https://github.com/JChristensen/Button.
#include "Button.h"

// Stepper motor library.
// See http://www.airspayce.com/mikem/arduino/AccelStepper/index.html.
#include <AccelStepper.h>

// Global constants and variables. /////////////////////////////////////////////
// Pin numbers.
const byte buttonUpPin = 3;
const byte buttonDownPin = 2;
const byte motorWakePin = 0;
const byte dirPin = 4;
const byte stepPin = 1;

// Skrew speed [rps].
const float skrewSpeed = 0.25f;

// Ratio of the worm drive.
const float gearRatio = 23.0f;

// Resolution of the stepper motor.
const byte motorResolution = 200;

// Maximum admissible motor position.
const long maxPosition = (long)(1.5 * motorResolution * gearRatio);

// Button debouncing time [ms].
const byte debounceTime = 50;

// Buttons.
const boolean pullup = true;
const boolean invert = true;
Button buttonUp(buttonUpPin, pullup, invert, debounceTime);
Button buttonDown(buttonDownPin, pullup, invert, debounceTime);

// Stepper motor.
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

// Time to full speed [s].
float accelerationTime = 1.0f;

// Functions. //////////////////////////////////////////////////////////////////
// Set up the controller after boot.
void setup()
{
    // Set processor clock to 16 MHz.
    if (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);

    // Initialize motor wake pin.
    pinMode(motorWakePin, OUTPUT);

    // Set maximum speed of stepper motor.
    stepper.setMaxSpeed(motorResolution * gearRatio * skrewSpeed);

    // Set acceleration of stepper motor.
    stepper.setAcceleration(stepper.maxSpeed() / accelerationTime);
}

// Infinite worker loop.
void loop()
{
    // Read the states of the buttons.
    buttonUp.read();
    buttonDown.read();
    
    // If a button was pressed, move the skrew in the respective direction.
    // If the button was released, stop the motor.
    if (buttonUp.wasPressed() || buttonUp.wasPressed())
    {
        digitalWrite(motorWakePin, HIGH);
        delay(2ul);
        stepper.moveTo((buttonUp.wasPressed() - buttonDown.wasPressed()) 
                       * maxPosition);
    }
    else if (buttonUp.wasReleased() || buttonDown.wasReleased())
        stepper.stop();

    // Turn the motor, if required. If the motor does not run, let the 
    // motor driver sleep.
    digitalWrite(motorWakePin, stepper.run());
}
