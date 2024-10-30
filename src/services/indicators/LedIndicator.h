#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include <Arduino.h>

class LedIndicator {
public:
    // Constructor with LED pin mappings
    LedIndicator(int led0Pin, int led1Pin, int errorLedPin);

    // Method to update the state and blink pattern
    void setState(const String &state, int blinkCount = 0);

    // FreeRTOS Task that handles the blinking in a separate thread
    static void blinkTask(void *parameter);

private:
    // Helper to turn all LEDs off
    void turnOffAll();

    // Store the LED pins
    int _led0Pin, _led1Pin, _led2Pin, _errorLedPin;

    // Parameters for blinking logic
    int _blinkCount;
    bool _isBlinking;

    // Task handle for the blink task
    TaskHandle_t _blinkTaskHandle;
};

#endif
