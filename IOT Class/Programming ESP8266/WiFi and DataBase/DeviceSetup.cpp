#include "DeviceSetup.h"
#include <ArduinoJson.h>

// Define DHT sensor type and pin
#define DHTPIN D2        // Pin where the DHT11 data pin is connected
#define DHTTYPE DHT11    // DHT 11
#define BUTTON_PIN D1    // Define the button pin

extern const char* ssid;      // WiFi SSID declared elsewhere
extern const char* password;  // WiFi password declared elsewhere

extern DHT dht;
extern WiFiConnection wifiConnection;

extern String timeZone;  // Time zone variable declared elsewhere

void initializeDevice() {
    Serial.begin(9600);
    wifiConnection.connect();
    dht.begin();
    
    // Set up the button pin
    pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up resistor

    // Prompt user for time zone
    Serial.println("Select your time zone:");
    Serial.println("1: Eastern Time Zone (ET) – New York City, NY");
    Serial.println("2: Central Time Zone (CT) – Chicago, IL");
    Serial.println("3: Mountain Time Zone (MT) – Denver, CO");
    Serial.println("4: Pacific Time Zone (PT) – Los Angeles, CA");
    Serial.println("5: Alaska Time Zone (AKT) – Anchorage, AK");
    Serial.println("6: Hawaii-Aleutian Time Zone (HAT) – Honolulu, HI");
    Serial.println("7: Atlantic Time Zone (AT) – San Juan, Puerto Rico");
    
    // Wait for user input
    while (Serial.available() == 0) {} // Wait for input
    int choice = Serial.parseInt();

    switch (choice) {
        case 1: timeZone = "New_York"; break;
        case 2: timeZone = "Chicago"; break;
        case 3: timeZone = "Denver"; break;
        case 4: timeZone = "Los_Angeles"; break;
        case 5: timeZone = "Anchorage"; break;
        case 6: timeZone = "Honolulu"; break;
        case 7: timeZone = "San_Juan"; break;
        default: timeZone = "Los_Angeles"; // Default
    }

    Serial.println("Setup complete. Press the button to take a reading.");
}
