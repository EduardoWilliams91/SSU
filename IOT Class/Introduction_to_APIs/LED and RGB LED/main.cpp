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

// RGB LED pin definitions for LED3
const int led3RedPin = D4;    // GPIO 4 (D2 on NodeMCU) for RED
const int led3GreenPin = D3;  // GPIO 0 (D3 on NodeMCU) for GREEN
const int led3BluePin = D7;   // GPIO 2 (D7 on NodeMCU) for BLUE

// Timer variables
unsigned long previousMillis = 0;
const long interval = 30000; // 30 seconds

void setup() {
  Serial.begin(9600);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  // Set RGB LED pins as outputs
  pinMode(led3RedPin, OUTPUT);
  pinMode(led3GreenPin, OUTPUT);
  pinMode(led3BluePin, OUTPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Display the Wi-Fi network name and IP address
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("Wi-Fi Network: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
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
  StaticJsonDocument<300> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print("JSON Parsing Error: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract LED1 and LED2 statuses from JSON
  const char* led1Status = doc["Node1"]["LED1"];
  const char* led2Status = doc["Node1"]["LED2"];
  int led3Red = doc["Node2"]["LED3"]["red"];   // Extract red component for LED3
  int led3Green = doc["Node2"]["LED3"]["green"]; // Extract green component for LED3
  int led3Blue = doc["Node2"]["LED3"]["blue"];   // Extract blue component for LED3

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

  // Control LED3 color based on RGB values
  analogWrite(led3RedPin, led3Red);     // Set red component
  analogWrite(led3GreenPin, led3Green); // Set green component
  analogWrite(led3BluePin, led3Blue);   // Set blue component
  Serial.print("LED3 RGB Values - Red: ");
  Serial.print(led3Red);
  Serial.print(", Green: ");
  Serial.print(led3Green);
  Serial.print(", Blue: ");
  Serial.println(led3Blue);

  // Disconnect
  client.stop();
}

void loop() {
  unsigned long currentMillis = millis();

  // Check results1.txt every 30 seconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println("Checking status from results1.txt...");
    checkFileAndControlLEDs();
  }
}
