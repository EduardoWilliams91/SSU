#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>        // Include ArduinoJson library for JSON parsing
#include <Adafruit_GFX.h>       // Include Adafruit GFX library
#include <Adafruit_SSD1306.h>   // Include Adafruit SSD1306 library

// OLED display settings
#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define OLED_RESET    -1        // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
const int led3BluePin = D7;   // GPIO 2 (D4 on NodeMCU) for BLUE

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

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for most OLED displays
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connecting to Wi-Fi:");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }

  // Display the Wi-Fi network name and IP address
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("Wi-Fi Network: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Network: ");
  display.print(WiFi.SSID());
  display.setCursor(0, 20);
  display.print("IP: ");
  display.print(WiFi.localIP());
  display.display();
}

void checkFileAndControlLEDs() {
  // Fetch the file from the server
  WiFiClientSecure client;
  client.setInsecure();  // Use setInsecure to ignore SSL certificate verification
  
  Serial.print("Connecting to ");
  Serial.println(host);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connecting to server...");
  display.display();

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");

    // Display connection failed status
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Connection failed");
    display.setCursor(0, 30);
    display.print("Host: ");
    display.print(host);
    display.display();
    return;
  }

  // Connection succeeded, update OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connected to server");
  display.setCursor(0, 30);
  display.print("Host: ");
  display.print(host);
  display.display();

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

  // Extract LED3 statuses from JSON
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

  // Display LED information on OLED
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("LED1: ");
  display.println(led1Status);
  display.print("LED2: ");
  display.println(led2Status);
  display.setCursor(0, 45);
  display.print("LED3 RGB:");
  display.print("R:");
  display.print(led3Red);
  display.print(" G:");
  display.print(led3Green);
  display.print(" B:");
  display.print(led3Blue);
  display.display();

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
