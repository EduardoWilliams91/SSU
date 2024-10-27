#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecureBearSSL.h>

const char* ssid = "EDUARDO";     
const char* password = "123456789"; 

const int buttonPin = D1; // GPIO Pin where the button is connected
bool buttonState = HIGH;  // Button state (assumes pull-up resistor)
bool lastButtonState = HIGH;

void setup() {
    Serial.begin(9600);
    
    // Set button pin as input with internal pull-up
    pinMode(buttonPin, INPUT_PULLUP);
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void sendIFTTTRequest() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient https;
        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setInsecure(); // For testing purposes. In production, use the right certificate
        
        String url = "https://maker.ifttt.com/trigger/button_pressed /json/with/key/bj0oXQsO4unCpz8lDQf1Bx";

        if (https.begin(*client, url)) {
            int httpCode = https.GET();
            Serial.println("Response Code <--: " + String(httpCode));

            if (httpCode > 0) {
                String response = https.getString();
                Serial.println(response);
            } else {
                Serial.printf("[HTTPS] Unable to connect. Error: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
        } else {
            Serial.println("[HTTPS] Unable to connect.");
        }
    } else {
        Serial.println("WiFi not connected.");
    }
}

void loop() {
    buttonState = digitalRead(buttonPin);

    // Check if the button is pressed (LOW signal)
    if (buttonState == LOW && lastButtonState == HIGH) {
        Serial.println("Button pressed!");
        sendIFTTTRequest();
        delay(1000);  // Debounce delay
    }

    lastButtonState = buttonState;
}
