// Headlight trim program
//
// Hardware:
// The headlight trimmer consists of an Arduino Trinket, a stepper motor 
// attached to the headlight trim adjustment skrew via a worm drive, the 
// stepper motor driver, and a button.
// 
// Behavior:
// When the button is pressed, the motor turns the headlight trim adjustment 
// skrew.
//
// Copyright 2017 Alexander Schaefer

// Included libraries. /////////////////////////////////////////////////////////
// Button handling and debouncing.
// See https://github.com/JChristensen/Button.
#include "Button.h"

// Global constants. ///////////////////////////////////////////////////////////
// Button debouncing time [ms].
const int debounceTime = 50;

// Pin numbers.
const int buttonUpPin = 3;
const int buttonDownPin = 4;
const int dirPin = 2;
const int stepPin = 1;
const int wakePin = 0;

// Skrew angular velocity [rpm].
const double skrewRpm = 15.0;

// Ratio of the worm drive.
const double gearRatio = 23.0;

// Resolution of the stepper motor.
const double motorRes = 200.0;

// Counter frequency.
const double clockFrequency = 8.0e6;

// Global variables. ///////////////////////////////////////////////////////////
// Button.
const boolean pullup = true;
const boolean invert = true;
Button buttonUp(buttonUpPin, pullup, invert, debounceTime);
Button buttonDown(buttonDownPin, pullup, invert, debounceTime);

// Functions. //////////////////////////////////////////////////////////////////
// Set up the controller after boot.
void setup()
{
    // Initialize pins.
    pinMode(wakePin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    //pinMode(stepPin, OUTPUT);

  DDRB  = 0x02;
  TCCR1 = 0x9F;
  OCR1C = 1;

    // Configure the counter prescaler.
    //TCCR1 = B10010001;

    // Set the value of the counter compare register.
    //OCR1C = constrain(skrewRpm*gearRatio*motorRes/clockFrequency*2.0 - 1,
    //                  1, 255);
}

// Infinite worker loop.
void loop()
{
    // Set the direction.
    //digitalWrite(dirPin, buttonUp.isPressed());
    
    // Wake the motor driver up if a button is pressed.
    //digitalWrite(wakePin, buttonUp.read() || buttonDown.read());
digitalWrite(dirPin, HIGH);
digitalWrite(wakePin, HIGH);
while (true)
{
  //digitalWrite(stepPin,HIGH);
    delay(10);
    //digitalWrite(stepPin, LOW);
}
}

