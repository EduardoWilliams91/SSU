#ifndef DEVICESETUP_H
#define DEVICESETUP_H

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "WiFiConnection.h"

// Function to initialize the device (replaces the setup() function)
void initializeDevice();

#endif // DEVICESETUP_H
