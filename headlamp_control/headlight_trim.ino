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

// Include libraries. //////////////////////////////////////////////////////////
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

// Step frequency at maximum speed.
const float stepFrequency = 1.0f / (0.25f * 23f * 200f);

// Duration needed to wake the motor driver and set it asleep [us].
unsigned int wakeDuration = 1500u;
unsigned int sleepDuration = 100u;

// Time the motor driver needs to recognize a step signal [us].
unsigned int stepSignalDuration = 2u;

// Point in time when the next step is triggered relative to the time then the 
// program was started [us].
unsigned long nextStep = 0ul;

// Position of the motor in steps.
int position = 0;

// Duration to full speed [us].
unsigned long accelerationDuration = 1000000ul;

// Point in time when the motor started accelerating [us].
unsigned long accelerationStartTime = 0ul;

// Tells if the motor turns.
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
    pinMode(motorWakePin, OUTPUT);
    pinMode(stepPin, OUTPUT);
}

// Infinite worker loop.
void loop()
{
    // Read the states of the buttons.
    buttonUp.read();
    buttonDown.read();
        
    // Start or stop the motor.
    if (buttonUp.wasPressed() || buttonUp.wasPressed())
    {
        digitalWrite(wakePin, HIGH);
        delayMicroseconds(wakeTime);
        turn = true;
        nextStep = micros();
        accelerationStartTime = nextStep;
    }
    else if (buttonUp.wasReleased() || buttonDown.wasReleased())
    {
        digitalWrite(wakePin, LOW);
        delayMicroseconds(sleepTime);
        turn = false;
    }
    
    // Step the motor.
    if (turn)
    {
        // Measure the time to the next step.
        int dt = nextStep - micros();
        
        // Step the motor.
        if (dt <= 0)
        {
            // Step the motor.
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(stepTime);
            digitalWrite(stepPin, LOW);
        
            // Compute when the next step is due.
            int elapsedAccelerationTime = constrain(micros() - accelerationStartTime, 
                                                    1, accelerationTime);
            nextStep += accelerationTime 
                        / (float)(elapsedAccelerationTime * stepFrequency);
        }
    }    
}
