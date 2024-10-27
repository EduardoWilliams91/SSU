#include "WiFiConnection.h"
#include <ESP8266WiFi.h> 
#include <DHT.h>

WiFiConnection::WiFiConnection(const char* ssid, const char* password) {
    _ssid = ssid;
    _password = password;
}

void WiFiConnection::connect() {
    Serial.begin(9600); // Initialize Serial Monitor (use 115200 for better readability)
    
    WiFi.begin(_ssid, _password); // Start WiFi connection

    // Wait for connection
    int attempts = 0; // Initialize a counter for connection attempts
    while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Added a timeout
        delay(500);
        Serial.print("."); // Print dots while connecting
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(); // New line after connection
        printConnectionDetails(); // Print connection details
    } else {
        Serial.println("Failed to connect to WiFi.");
    }
}

void WiFiConnection::printConnectionDetails() {
    Serial.println("  ");
    Serial.print("Connected to: "); // Inform the user about the connection
    Serial.println(WiFi.SSID());    // Print the SSID of the connected network
    Serial.print("IP Address: ");    // Print the IP address assigned to the ESP8266
    Serial.println(WiFi.localIP());  // Print the local IP address
    Serial.println("  ");
}
