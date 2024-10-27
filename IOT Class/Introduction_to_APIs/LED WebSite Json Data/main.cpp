#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>  // Include ArduinoJson library for JSON parsing

// Wi-Fi credentials
const char* ssid = "EDUARDO";     
const char* password = "123456789"; 

// Server and file path
const char* host = "eduardowilliams.net";
const int httpsPort = 443;
const char* url = "/LED/results1.txt";

// Pin definitions
const int led1Pin = D5;       // GPIO 14 (D5 on NodeMCU) for LED1
const int led2Pin = D6;       // GPIO 12 (D6 on NodeMCU) for LED2
const int buttonPin = D1;     // GPIO 5 (D1 on NodeMCU) for the push button

// Variable to store button state
int buttonState = 0;
int lastButtonState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Using internal pull-up resistor

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void checkFileAndControlLEDs() {
  // Fetch the file from the server
  WiFiClientSecure client;
  client.setInsecure();  // Use setInsecure to ignore SSL certificate verification
  
  Serial.print("Connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  // Send the HTTP GET request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  // Skip HTTP headers and capture only the JSON body
  String json = "";
  bool jsonStarted = false;
  while (client.connected() || client.available()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      jsonStarted = true;  // The JSON starts right after headers
      continue;
    }
    if (jsonStarted) {
      json += line + "\n";
    }
  }

  // Print the JSON data for debugging
  Serial.println("JSON Response:");
  Serial.println(json);

  // Parse JSON data
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print("JSON Parsing Error: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract LED1 and LED2 statuses from JSON
  const char* led1Status = doc["Node1"]["LED1"];
  const char* led2Status = doc["Node1"]["LED2"];

  // Control LED1 based on status
  if (strcmp(led1Status, "ON") == 0) {
    digitalWrite(led1Pin, HIGH);  // Turn LED1 ON
    Serial.println("LED1 ON");
  } else if (strcmp(led1Status, "OFF") == 0) {
    digitalWrite(led1Pin, LOW);   // Turn LED1 OFF
    Serial.println("LED1 OFF");
  }

  // Control LED2 based on status
  if (strcmp(led2Status, "ON") == 0) {
    digitalWrite(led2Pin, HIGH);  // Turn LED2 ON
    Serial.println("LED2 ON");
  } else if (strcmp(led2Status, "OFF") == 0) {
    digitalWrite(led2Pin, LOW);   // Turn LED2 OFF
    Serial.println("LED2 OFF");
  }

  // Disconnect
  client.stop();
}

void loop() {
  // Read the state of the push button
  buttonState = digitalRead(buttonPin);

  // Check if the button was pressed (change from HIGH to LOW)
  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.println("Button pressed, checking status...");
    checkFileAndControlLEDs();  // Call the function to check the file and control LEDs
  }

  // Update the last button state
  lastButtonState = buttonState;
}
