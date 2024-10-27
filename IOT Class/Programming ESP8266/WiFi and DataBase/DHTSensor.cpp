#include "DHTSensor.h"
#include <Arduino.h>

volatile bool DHTSensor::buttonPressed = false; // Initialize the static variable
unsigned long lastDebounceTime = 0; // Last time the button press was registered
const unsigned long debounceDelay = 20; // Delay to debounce the button (in milliseconds)

DHTSensor::DHTSensor(int dhtPin, int dhtType, int buttonPin) 
    : dht(dhtPin, dhtType), buttonPin(buttonPin) {}

void IRAM_ATTR DHTSensor::handleButtonPress() {
    // Only set buttonPressed if enough time has passed since the last press
    if (millis() - lastDebounceTime > debounceDelay) {
        buttonPressed = true; // Set flag when button is pressed
        lastDebounceTime = millis(); // Update the last debounce time
    }
}

void DHTSensor::begin() {
    dht.begin(); // Initialize DHT sensor
    pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with pull-up resistor
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING); // Attach the ISR
}

void DHTSensor::read() {
    if (buttonPressed) { // Check if the button has been pressed
        buttonPressed = false; // Reset the flag

        float temperature = dht.readTemperature(); // Read temperature
        float humidity = dht.readHumidity(); // Read humidity

        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!"); // Handle error
        } else {
            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.println(" °C");
            Serial.print("Humidity: ");
            Serial.print(humidity);
            Serial.println(" %");
            Serial.println("  ");
        }
    }
}
