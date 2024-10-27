#include "DeviceLoop.h"
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "WiFiConnection.h"

// Define DHT sensor type and pin (ensure consistency with your existing setup)
#define BUTTON_PIN D1  // Ensure this is the same as in the main setup

extern DHT dht;  // External reference to the sensor object
extern WiFiConnection wifiConnection;  // External WiFi connection
extern String url;
extern String data1;
extern String data2;
extern String data3;
extern String timeZone;

// Function declarations
extern String getCurrentTimestamp();
extern void sendData(String url);

void executeLoop() {
    // Check if the button is pressed
    if (digitalRead(BUTTON_PIN) == LOW) {
        // Delay for button debounce
        delay(50);
        if (digitalRead(BUTTON_PIN) == LOW) { // Check again to confirm
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("WiFi not connected! Trying to reconnect...");
                wifiConnection.connect();
                delay(1000);
                return;
            }

            float temperature = dht.readTemperature();
            float humidity = dht.readHumidity();

            if (isnan(temperature) || isnan(humidity)) {
                Serial.println("Failed to read from DHT sensor!");
                return;
            }

            // Fetch the current timestamp
            String timestamp = getCurrentTimestamp();

            // Construct the full URL using the fetched timestamp
            String fullUrl = url + data1 + String(temperature) + data2 + timestamp + data3 + String(humidity);
            Serial.println("Requesting: --> " + fullUrl);

            sendData(fullUrl);
            Serial.println("Data sent. Press the button for the next reading.");
        }

        // Wait for button release to avoid multiple readings
        while (digitalRead(BUTTON_PIN) == LOW) {
            delay(10);
        }
    }

    // Add a small delay to avoid busy-waiting
    delay(100);
}
