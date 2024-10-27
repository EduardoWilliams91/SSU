#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "WiFiConnection.h"
#include <ArduinoJson.h>
#include "DeviceSetup.h"
#include "DeviceLoop.h"

// Define DHT sensor type and pin
#define DHTPIN D2        // Pin where the DHT11 data pin is connected
#define DHTTYPE DHT11    // DHT 11
#define BUTTON_PIN D1    // Define the button pin

const char* ssid = "EDUARDO";     
const char* password = "123456789"; 

String url = "https://eduardowilliams.net/DatabaseFolder/Database_1.php?Node_Name=Node+1";
String data1 = "&Temperature=";
String data2 = "&Time_received="; // Placeholder for time
String data3 = "&Humidity=";

DHT dht(DHTPIN, DHTTYPE);
WiFiConnection wifiConnection(ssid, password);

String timeZone; // Variable to hold the selected time zone


void setup() {
    initializeDevice();
}

void loop() {
    executeLoop();
}

void sendData(String url) {
    WiFiClientSecure client;
    client.setInsecure(); // For production, use a proper certificate

    HTTPClient https;
    if (https.begin(client, url)) {
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
        Serial.printf("[HTTPS] Unable to connect.\n");
    }
}

String getCurrentTimestamp() {
    String timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/" + timeZone; 
    Serial.println("Time API URL: " + timeApiUrl); 

    WiFiClientSecure client;
    client.setInsecure(); // For testing

    HTTPClient http;
    http.begin(client, timeApiUrl); 

    int httpCode = http.GET(); 
    String currentTime = "";

    if (httpCode == 200) {
        String payload = http.getString(); 

        DynamicJsonDocument doc(1024); 

        DeserializationError error = deserializeJson(doc, payload);
        
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return ""; 
        }

        // Access the dateTime field
        currentTime = doc["dateTime"].as<String>();

        // Replace 'T' with '+' and remove milliseconds if present
        currentTime.replace('T', '+');
        int msIndex = currentTime.indexOf('.');
        if (msIndex != -1) {
            currentTime = currentTime.substring(0, msIndex); // Keep only the part before milliseconds
        }

        // Optional: Print out the modified timestamp for debugging
        Serial.println("Formatted Timestamp: " + currentTime);
    } else {
        Serial.printf("Error getting time: %s\n", http.errorToString(httpCode).c_str());
        Serial.println("HTTP Code: " + String(httpCode)); 
    }

    http.end(); 
    return currentTime; 
}
