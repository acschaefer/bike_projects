// Heated sock control program
//
// Hardware:
// The sock heater consists of an Arduino Trinket, a button, three LEDs, 
// a MOSFET, and heated socks.
// 
// Behavior:
// When the button is first pressed, all LEDs are lit and the socks are heated
// with maximum power. Each time the user presses the button again, one LED 
// after the other is switched off and the heating power is reduced. When the 
// user presses the button for at least one second, the LEDs and the heating 
// are switched off.
//
// Copyright 2017 Alexander Schaefer

// Included libraries. /////////////////////////////////////////////////////////
// Button handling and debouncing.
// See https://github.com/JChristensen/Button.
#include "Button.h"

// Global constants. ///////////////////////////////////////////////////////////
// Button debouncing time [ms].
const int debounceTime = 50;

// Duration of LED flash during startup.
const unsigned long flashDuration = 1000ul;

// Number of heat levels and corresponding PWM duty cycles.
const int heatLevels = 4;
const double heatDutyCycle[heatLevels] = {0.00, 0.33, 0.67, 1.00};

// Pin numbers.
const int leds = heatLevels - 1;
const int ledPin[leds] = {1, 0, 4};
const int buttonPin = 2;
const int heaterPin = 3;

// PWM period [ms].
const unsigned long heatingPeriod = 5000ul;

// Global variables. ///////////////////////////////////////////////////////////
// Current heat level.
int heat = 0;

// Button sensor.
Button button(buttonPin, true, true, debounceTime);

// Functions. //////////////////////////////////////////////////////////////////
// Switch on given number of LEDs.
void glow(int n)
{
    for (int i = 0; i < leds; ++i)
        digitalWrite(ledPin[i], i<n);
}

// Flash all LEDs.
void flash()
{
    glow(leds);
    delay(flashDuration);
    glow(0);
}

// Set up the controller after boot.
void setup()
{
    // Initialize pins.
    for (int i = 0; i < leds; ++i)
        pinMode(ledPin[i], OUTPUT);
    pinMode(heaterPin, OUTPUT);
    
    // Flash all LEDs to show device is ready.
    flash();
}

// Infinite worker loop.
void loop()
{
    // Adjust the heat level according to how long the button was pressed.
    button.read();
    if (button.wasPressed())
    {
        if (heat > 0)
            heat--;
        else
            heat = heatLevels - 1;
    }
    
    // Visualize the heat level using the LEDs.
    glow(heat);
    
    // If the heating duration is not yet over, heat the socks.
    unsigned long millisSincePeriod = millis() % heatingPeriod;
    unsigned long millisToHeat = heatDutyCycle[heat] * heatingPeriod;
    boolean heatOn =  millisToHeat > millisSincePeriod;
    digitalWrite(heaterPin, heatOn);
}

