#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <PubSubClient.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// WiFi credentials
const char* ssid = "*********";  // Replace with your WiFi SSID
const char* password = "*******";  // Replace with your WiFi password

// MQTT Broker settings
const char* mqtt_server = "broker.hivemq.com"; // Use an alternative if needed
const int mqtt_port = 8883; // Use 1883 for non-TLS
WiFiClientSecure espClient;
PubSubClient client(espClient);

// MQTT topics
const char* subscribeTopic = "testtopic/LED/ON_OFF";
const char* switchPublishTopic = "testtopic/switch/Press"; // Topic for switch events
const char* potPublishTopic = "testtopic/Pot/Value"; // Topic for potentiometer values

// Pin definitions
#define LED_PIN D4 // Use D4 for LED (maps to GPIO2)
#define SWITCH_PIN D5 // Use D5 for the switch (maps to GPIO14)

// WiFi setup
void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP); // Disable power-saving mode
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Delay for WiFi connection attempts
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Display MQTT message on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("MQTT Message:");
  display.println("Topic:");
  display.println(topic);
  display.print("Message: ");
  display.println(message);
  display.display();

  // Print on serial monitor
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  // Control the LED based on the message
  if (String(topic) == subscribeTopic) {
    if (message == "1") {
      digitalWrite(LED_PIN, HIGH); // Turn the LED on
      Serial.println("LED ON");
    } else if (message == "0") {
      digitalWrite(LED_PIN, LOW); // Turn the LED off
      Serial.println("LED OFF");
    }
  }
}

// Reconnect MQTT if disconnected
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
//Haive User and password Credentials ACCESS MANAGEMENT 
    if (client.connect(clientId.c_str(), "********", "******")) {
      Serial.println("connected");
      client.subscribe(subscribeTopic);
      Serial.println("Subscribed to topic: testtopic/LED/ON_OFF");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 10 seconds...");
      delay(10000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); // Initialize D4 as output
  pinMode(SWITCH_PIN, INPUT_PULLUP); // Initialize D5 as input with internal pull-up
  digitalWrite(LED_PIN, LOW); // Turn off LED initially

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Use the correct I2C address
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Halt execution if OLED initialization fails
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  // Connect to WiFi and MQTT
  setup_wifi();

  espClient.setInsecure(); // Skip certificate validation for testing
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setKeepAlive(120);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected!");
  display.display();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(100);

  static bool lastSwitchState = HIGH;
  bool currentSwitchState = digitalRead(SWITCH_PIN);

  if (lastSwitchState == HIGH && currentSwitchState == LOW) {
    Serial.println("Switch pressed!");
    client.publish(switchPublishTopic, "1");

    // Display switch state on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Switch State:");
    display.println("Pressed");
    display.display();

    delay(100);
  }
  lastSwitchState = currentSwitchState;

  static unsigned long lastPotMsg = 0;
  static int lastPotValue = 0;
  int potValue = analogRead(A0);
  int threshold = 10;

  if (millis() - lastPotMsg > 2000 || abs(potValue - lastPotValue) >= threshold) {
    lastPotMsg = millis();
    lastPotValue = potValue;

    char potStr[8];
    snprintf(potStr, sizeof(potStr), "%d", potValue);
    client.publish(potPublishTopic, potStr);

    // Display potentiometer value on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Potentiometer:");
    display.print("Value: ");
    display.println(potValue);
    display.display();

    Serial.print("Potentiometer value: ");
    Serial.println(potStr);
    delay(100);
  }
}
