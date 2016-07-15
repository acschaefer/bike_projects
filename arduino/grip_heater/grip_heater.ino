// Grip heater control
//
// Copyright 2016 Alexander Schaefer


// Constants. /////////////////////////////////////////////////////////
// Button debouncing time [ms].
const int debounceTime = 100;

// Duration of long button activation [ms].
const int longActivation = 1000;

// Number of heat levels.
const int heatLevels = 6;

// LED pin numbers.
const int ledPin[nLeds-1] = {LED_BUILTIN, 12, 11, 10, 9};

// Number of the pin connected to the button.
const int buttonPin = 1;


// Global variables. //////////////////////////////////////////////////
// Number of LEDs lit.
int heat = 0;


// Functions. /////////////////////////////////////////////////////////
// Determines the heat level depending on the duration of button activation.
// Returns false if the button is not yet debounced, otherwise true.
boolean determine_heat(int duration)
{
    // If the button is not yet debounced, return -1.
    if (duration <= debounceTime)
        return false;
    
    // If the user presses the button for a short time, increase the heat.
    if (duration < longActivation)
    {
        heat = (heat + 1) % heatLevels;
        return true;
    }
    
    // If the user presses the button for a long time, reset the heat level.
    heat = 0;
    return true;
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
    // Sample the button state.
    if (digitalRead(buttonPin) == HIGH)
        heat = 5;
  
    delay();
    milli();
}
