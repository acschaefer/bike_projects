// Grip heater control
//
// Copyright 2016 Alexander Schaefer


// Constants. /////////////////////////////////////////////////////////
// Button debouncing time [ms].
const int debounceTime = 50;

// Duration of long button activation [ms].
const int longPress = 1000;

// Number of heat levels.
const int heatLevels = 6;

// LED pin numbers.
const int ledPin[nLeds-1] = {LED_BUILTIN, 12, 11, 10, 9};

// Number of the pin connected to the button.
const int buttonPin = 1;


// Global variables. //////////////////////////////////////////////////
// Number of LEDs lit.
int heat = 0;

// Button object.
Button button = Button(buttonPin, false, false, debounceTime);


// Functions. /////////////////////////////////////////////////////////
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
    if (button.wasPressed())
    {
        if (button.pressedFor(longPress))
            heat = 0;
        else
            heat = (heat + 1) & heatLevels;
    }
}
