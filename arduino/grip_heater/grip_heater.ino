// Grip heater control program
//
// Copyright 2016 Alexander Schaefer

// Included libraries. ////////////////////////////////////////////////
// Include Button library.
#include "Button.h"


// Constants. /////////////////////////////////////////////////////////
// Button debouncing time [ms].
const int debounceTime = 50;

// Duration of long button activation [ms].
const int longPress = 1000;

// Number of heat levels and corresponding PWM duty cycles.
const int heatLevels = 6;
const double heatDutyCycle[heatLevels] = {0.0, 0.1, 0.2, 0.28, 0.35, 0.5};

// Pin numbers.
const int leds = heatLevels - 1;
const int ledPin[leds] = {LED_BUILTIN, 12, 11, 10, 9};
const int buttonPin = 1;
const int heaterPin = 0;

// PWM period [ms].
const int heatingPeriod = 5000;


// Global variables. //////////////////////////////////////////////////
// Heat level.
int heat = 0;

// Create Button object.
Button button = Button(buttonPin, false, false, debounceTime);


// Functions. /////////////////////////////////////////////////////////
// Compute the heating duration in each PWM period in [ms].
int heatingDuration()
{
    return heatDutyCycle[heat] * heatingPeriod;
}


// Set up the controller after boot.
void setup() 
{
    // Initialize the LED pins.
    for (int i = 0; i < nLeds; ++i)
        pinMode(ledPin[i], OUTPUT);
  
    // Initialize the button pin.
    pinMode(buttonPin, INPUT);
}


// Infinite worker loop.
void loop()
{
    // Update the button state.
    button.read();
    
    // Adjust the heat level according to how long the button was pressed.
    heat = ((heat + button.wasPressed()) % heatLevels) * button.pressedFor(longPress);

    // Visualize the heat level using the LEDs.
    for (int i = 0; i < leds; ++i)
        digitalWrite(ledPin[i], i<heat);

    // If the heating duration is not yet over, heat the grips.
    digitalWrite(heaterPin, (milli() % heatingPeriod) < heatingDuration());
}
