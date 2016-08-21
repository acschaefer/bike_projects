// Grip heater control program
//
// Hardware:
// The grip heater consists of an Arduino Trinket Pro, a button, five LEDs, 
// a MOSFET, and heated grips.
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
// See https://github.com/JChristensen/Button.
#include "Button.h"

// Input voltage measuring.
// See https://github.com/Yveaux/arduino_vcc.
#include "Vcc.h"

// Global constants. ///////////////////////////////////////////////////////////
// Input voltage sensor.
Vcc vcc();

// Reference input voltage.
const float vccRef = 14.75f;

// Button debouncing time [ms].
const int debounceTime = 50;

// Duration of long button activation [ms].
const int longPress = 1000;

// LED brightness. Interval: [0; 255].
const byte brightness = 0.3 * 255;

// Duration of LED flash during startup.
const unsigned long flashDuration = 1500ul;

// Number of heat levels and corresponding PWM duty cycles.
const int heatLevels = 6;
const double heatDutyCycle[heatLevels] = {0.00, 0.30, 0.45, 0.60, 0.80, 1.00};

// Pin numbers.
const int leds = heatLevels - 1;
const int ledPin[leds] = {3, 10, 11, 9, 6};
const int buttonPin = 8;
const int heaterPin = 0;

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
        analogWrite(ledPin[i], brightness * (i<n));
}

// Flash all LEDs.
void flash()
{
    // Switch on all LEDs, one after the other.
    const unsigned long stepDuration = flashDuration / 3;
    for (int i = 1; i < leds; ++i)
    {
        glow(i);
        delay(stepDuration / (leds-1));
    }
    glow(leds);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(flashDuration - stepDuration);
    
    // Switch off LEDs.
    glow(0);
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

    if (button.pressedFor(longPress) && heat < heatLevels-1)
        heat = 0;
    
    // Visualize the heat level using the LEDs.
    glow(heat);
    
    // If the heating duration is not yet over, heat the grips 
    // and light the built-in LED.
    unsigned long millisSincePeriod = millis() % heatingPeriod;
    vccCorrection = sq(vccRef / vcc.Read_Volts());
    unsigned long millisToHeat 
        = heatDutyCycle[heat] * heatingPeriod * vccCorrection;
    boolean heatOn =  millisToHeat > millisSincePeriod;
    digitalWrite(heaterPin, heatOn);
    digitalWrite(LED_BUILTIN, heatOn);
}
