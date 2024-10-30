#include "LedIndicator.h"

// Constructor to initialize pin numbers
LedIndicator::LedIndicator(int led0Pin, int led1Pin, int errorLedPin)
    : _led0Pin(led0Pin), _led1Pin(led1Pin), _errorLedPin(errorLedPin),
      _blinkCount(0), _isBlinking(false), _blinkTaskHandle(nullptr) {
    // Set LED pins as output
    pinMode(_led0Pin, OUTPUT);
    pinMode(_led1Pin, OUTPUT);
    pinMode(_errorLedPin, OUTPUT);

    // Ensure all LEDs are off initially
    turnOffAll();
}

// Method to update state and control LED behavior
void LedIndicator::setState(const String &state, int blinkCount) {
    // Stop any running blink task
    if (_blinkTaskHandle) {
        vTaskDelete(_blinkTaskHandle);
        _blinkTaskHandle = nullptr;
    }

    turnOffAll();  // Reset all LEDs

    // Set LEDs based on the state
    if (state == "initializing") {
        digitalWrite(_led0Pin, HIGH);  // LED 0 on
    } else if (state == "idle") {
        digitalWrite(_led1Pin, HIGH);  // LED 1 on
    } else if (state == "busy") {
        digitalWrite(_led0Pin, HIGH);  // LED 0 on
        digitalWrite(_led1Pin, HIGH);  // LED 1 on
    } else if (state == "error") {
        _blinkCount = blinkCount;
        _isBlinking = true;
        // Create a FreeRTOS task for error blinking
        xTaskCreatePinnedToCore(blinkTask, "BlinkTask", 2048, this, 1, &_blinkTaskHandle, 1);
    }
}

// Helper to turn off all LEDs
void LedIndicator::turnOffAll() {
    digitalWrite(_led0Pin, LOW);
    digitalWrite(_led1Pin, LOW);
    digitalWrite(_led2Pin, LOW);
    digitalWrite(_errorLedPin, LOW);
}

// FreeRTOS task to handle blinking with infinite loop
void LedIndicator::blinkTask(void *parameter) {
    LedIndicator *ledIndicator = static_cast<LedIndicator *>(parameter);

    while (true) {
        // Blink error LED _blinkCount times
        for (int i = 0; i < ledIndicator->_blinkCount; ++i) {
            digitalWrite(ledIndicator->_errorLedPin, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);  // LED on for 500ms

            digitalWrite(ledIndicator->_errorLedPin, LOW);
            vTaskDelay(500 / portTICK_PERIOD_MS);  // LED off for 500ms
        }

        // Wait for 2 seconds before repeating the blink cycle
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}


