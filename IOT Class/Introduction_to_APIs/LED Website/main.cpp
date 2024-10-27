#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials
const char* ssid = "EDUARDO";     
const char* password = "123456789"; 

// Server and file path
const char* host = "eduardowilliams.net";
const int httpsPort = 443;
const char* url = "/LED/results1.txt";

// Pin definitions
const int ledPin = D5;       // GPIO 14 (D5 on NodeMCU) for the LED
const int buttonPin = D1;    // GPIO 5 (D1 on NodeMCU) for the push button

// Variable to store button state
int buttonState = 0;
int lastButtonState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
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

void checkFileAndControlLED() {
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

  // Wait for server response and print full headers for debugging
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line); // Print each line of the header
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }

  // Read and print the full response body
  String result = client.readString();
  Serial.println("Full Response: " + result);

  // Control the LED based on the file contents
  if (result.indexOf("ON") >= 0) {
    digitalWrite(ledPin, HIGH);  // Turn LED ON
    Serial.println("LED ON");
  } else if (result.indexOf("OFF") >= 0) {
    digitalWrite(ledPin, LOW);   // Turn LED OFF
    Serial.println("LED OFF");
  } else {
    Serial.println("Invalid response");
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
    checkFileAndControlLED();  // Call the function to check the file
  }

  // Update the last button state
  lastButtonState = buttonState;
}
