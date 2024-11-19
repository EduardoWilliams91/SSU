#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEScan.h>

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// I2C address for the OLED display
#define OLED_ADDRESS 0x3C

// GPIO for the LED
#define LED_PIN 2 // Adjust to your LED pin

// BLE scan settings
#define SCAN_TIME 1 // Duration of BLE scan in seconds

// Create an SSD1306 display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);

    // Initialize the LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW); // Turn off LED initially

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Stop execution if OLED fails
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Display initialization message
    display.setCursor(0, 0);
    display.println("Initializing BLE...");
    display.display();

    // Initialize BLE
    BLEDevice::init("");
    Serial.println("Starting BLE scan...");
    delay(1000);
}

void loop() {
    // Create a BLE scan object
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true); // Active scanning for more details
    pBLEScan->setInterval(50);
    pBLEScan->setWindow(49);

    // Start scanning for a short duration
    BLEScanResults scanResults = pBLEScan->start(SCAN_TIME, false);

    bool deviceFound = false;

    // Loop through the scanned devices
    for (int i = 0; i < scanResults.getCount(); i++) {
        BLEAdvertisedDevice device = scanResults.getDevice(i);

        // Debug the entire device data
        Serial.print("Found Device: ");
        Serial.println(device.toString().c_str());

        // Check if the device name or address matches "Push_Button"
        if (device.getName() == "Push_Button" || device.toString().find("Push_Button") != std::string::npos) {
            deviceFound = true;
            Serial.println("Device 'Push_Button' Found!");

            // Display confirmation with name and public address on OLED
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("BLE Connected:");
            display.println("Name: Push_Button");
            display.print("Address: ");
            display.println(device.getAddress().toString().c_str());
            display.display();

            // Turn on LED to indicate connection
            digitalWrite(LED_PIN, HIGH);
            break; // Stop searching further
        }
    }

    if (!deviceFound) {
        // If no device named "Eduardo" is found
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Scanning...");
        display.display();

        // Turn off LED if not connected
        digitalWrite(LED_PIN, LOW);
    }

    // End scan
    pBLEScan->clearResults(); // Free memory
    delay(100); // Short delay before the next scan
}
