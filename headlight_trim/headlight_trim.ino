// Headlight trim program
//
// Hardware:
// The headlight trimmer consists of an Arduino Trinket, a stepper motor 
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
// Library necessary to enable 16 MHz clock frequency.
#include <avr/power.h>

// Button handling and debouncing.
// See https://github.com/JChristensen/Button.
#include "Button.h"

// Global constants and variables. /////////////////////////////////////////////
// Pin numbers.
const byte buttonUpPin = 3;
const byte buttonDownPin = 2;
const byte wakePin = 0;
const byte dirPin = 4;
const byte stepPin = 1;

// Step rate at maximum speed [Hz].
const unsigned long maxStepRate = 23 * 200 / 3;

// Duration needed to wake the motor driver and to set it asleep [us].
const unsigned int wakeDuration = 1500u;
const unsigned int sleepDuration = 100u;

// Time the motor driver needs to recognize a step signal [us].
const unsigned int stepDuration = 4u;

// Point in time when the last step was triggered [us]. The time is 
// specified relative to the time when the program was started.
unsigned long lastStep = 0ul;

// Position of the motor in steps.
int position = 0;

// Maximum admissible number of steps.
const int maxPosition = 1.5 * 200 * 23;
const int upperPosition = +maxPosition;
const int lowerPosition = -maxPosition;

// Motor acceleration duration to full speed [us].
double accelerationDuration = 500000.0;

// Point in time when the motor started accelerating [us].
unsigned long accelerationStart = 0ul;

// True if the motor turns.
boolean turn = false;

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
    // Set processor clock to 16 MHz.
    if (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);

    // Initialize the output pins.
    pinMode(wakePin, OUTPUT);
    pinMode(stepPin, OUTPUT);
}

// Infinite worker loop.
void loop()
{
    // Read the states of the buttons.
    buttonUp.read();
    buttonDown.read();
    
    // Activate the motor.
    if ((buttonUp.wasPressed() && position < upperPosition)
        || (buttonDown.wasPressed() && position > -lowerPosition))
    {
        // Wake the motor driver.
        digitalWrite(wakePin, HIGH);
        delayMicroseconds(wakeDuration);

        // Memorize this time step as the start of motion.
        lastStep = micros();
        accelerationStart = lastStep;
    }
    
    // Step the motor.
    if ((position < upperPosition && buttonUp.isPressed())
        || (position > lowerPosition && buttonDown.isPressed()))
    {   
        // Compute the current step rate [us].
        unsigned long elapsedAcceleration = micros() - accelerationStart;
        unsigned long currentStepRate = constrain(
            maxStepRate * elapsedAcceleration / accelerationDuration,
            0ul, maxStepRate);
        unsigned long dt = 1000000.0 / currentStepRate;
        
        // Step the motor, if a step is due.
        if (elapsedAcceleration > 0ul)
            if (micros() >= lastStep + dt)
            {
                // Step the motor.
                digitalWrite(stepPin, HIGH);
                delayMicroseconds(stepDuration);
                digitalWrite(stepPin, LOW);
    
                // Increment the position counter.
                position += buttonUp.isPressed();
                position -= buttonDown.isPressed();
    
                // Update the time of the last step command.
                lastStep += dt;
            }
    }

    // Deactivate the motor.
    if (buttonUp.wasReleased() || buttonDown.wasReleased())
    {
        // Send the motor driver to sleep.
        digitalWrite(wakePin, LOW);
        delayMicroseconds(sleepDuration);
    }
}
