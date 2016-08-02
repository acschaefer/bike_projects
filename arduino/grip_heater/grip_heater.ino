// Grip heater control program
//
// Hardware:
// The grip heater consists of an Arduino Trinket Pro, a button, five LEDs, 
// and heated grips.
// 
// Behavior:
// When the button is first pressed, all LEDs are lit and the grips are heated
// with maximum power. Each time the user presses the button again, one LED 
// after the other is switched off and the heating power is reduced. When the 
// user presses the button for at least one second, the LEDs and the heating 
// are switched off.
//
// Copyright 2016 Alexander Schaefer

// Included libraries. /////////////////////////////////////////////////////////
// Button handling and debouncing.
#include "Button.h"

// Constants. //////////////////////////////////////////////////////////////////
// Button debouncing time [ms].
const int debounceTime = 50;

// Duration of long button activation [ms].
const int longPress = 1000;

// LED brightness. Interval: [0; 255].
const byte brightness = 0.3 * 255;

// Duration of LED flash during startup.
const unsigned long flashDuration = 1000;

// Number of heat levels and corresponding PWM duty cycles.
const int heatLevels = 6;
const double heatDutyCycle[heatLevels] = {0.0, 0.10, 0.18, 0.25, 0.30, 0.40};

// Pin numbers.
const int leds = heatLevels - 1;
const int ledPin[leds] = {11, 3, 9, 10, 6};
const int buttonPin = 8;
const int heaterPin = 0;

// PWM period [ms].
const unsigned long heatingPeriod = 5000;

// Global variables. ///////////////////////////////////////////////////////////
// Current heat level.
int heat = 0;

// Button object.
Button button(buttonPin, true, true, debounceTime);

// Functions. //////////////////////////////////////////////////////////////////
// Set given number of LEDs to given brightness.
void shine(byte brightness, int n = leds)
{
    for (int i = 0; i < leds; ++i)
        analogWrite(ledPin[i], brightness * (i<n));
}

// Flash all LEDs.
void flash(unsigned long duration)
{    
    shine(brightness);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(duration);
    shine(0);
    digitalWrite(LED_BUILTIN, LOW);
}

// Set up the controller after boot.
void setup()
{
    // Initialize pins.
    for (int i = 0; i < leds; ++i)
        pinMode(ledPin[i], OUTPUT);
    pinMode(heaterPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Flash all LEDs to show device is ready.
    flash(flashDuration);
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

    if (button.pressedFor(longPress) && heat < heatLevels-1)
        heat = 0;
    
    // Visualize the heat level using the LEDs.
    shine(brightness, heat);
    
    // If the heating duration is not yet over, heat the grips 
    // and light the built-in LED.
    unsigned long millisSincePeriod = millis() % heatingPeriod;
    unsigned long millisToHeat = heatDutyCycle[heat] * heatingPeriod;
    boolean heatOn =  millisToHeat > millisSincePeriod;
    digitalWrite(heaterPin, heatOn);
    digitalWrite(LED_BUILTIN, heatOn);
}

