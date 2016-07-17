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
const byte brightness = 0.6 * 255;

// Duration of LED flash during startup.
const int flashDuration = 200;

// Number of heat levels and corresponding PWM duty cycles.
const byte heatLevels = 6;
const float heatDutyCycle[heatLevels] = {0f, 0.10f, 0.20f, 0.28f, 0.35f, 0.50f};

// Pin numbers.
const byte leds = heatLevels - 1;
const byte ledPin[leds] = {5, 6, 9, 10, 11};
const byte buttonPin = 1;
const byte heaterPin = 0;

// PWM period [ms].
const int heatingPeriod = 2000;

// Global variables. ///////////////////////////////////////////////////////////
// Current heat level.
byte heat = 0;

// Button struct.
Button button = Button(buttonPin, false, false, debounceTime);

// Functions. //////////////////////////////////////////////////////////////////
// Set given number of LEDs to given brightness.
void shine(byte brightness, byte n = leds)
{
    for (byte i = 0; i < leds; ++i)
        analogWrite(ledPin[i], brightness * (i<n));
}

// Set up the controller after boot.
void setup() 
{
    // Initialize pins.
    for (byte i = 0; i < leds; ++i)
        pinMode(ledPin[i], OUTPUT);
    pinMode(buttonPin, INPUT);
    pinMode(heaterPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Flash all LEDs to show device is ready.
    shine(brightness);
    analogWrite(LED_BUILTIN, brightness);
    delay(flashDuration);
    shine(0);
    analogWrite(LED_BUILTIN, 0);
}

// Infinite worker loop.
void loop()
{
    // Adjust the heat level according to how long the button was pressed.
    button.read();
    if (button.wasPressed())
    {
        if (heat <= 0)
            heat = heatLevels - 1;
        else
            heat = (heat - 1) * !button.pressedFor(longPress);
    }

    // Visualize the heat level using the LEDs.
    shine(brightness, heat);
    
    // If the heating duration is not yet over, heat the grips 
    // and light the built-in LED.
    boolean heatOn = (millis()%heatingPeriod) < heatDutyCycle[heat]*heatingPeriod;
    digitalWrite(heaterPin, heatOn);
    analogWrite(LED_BUILTIN, brightness * heatOn);
}
