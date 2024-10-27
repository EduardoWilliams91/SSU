#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include <DHT.h>


class DHTSensor {
public:
    DHTSensor(int dhtPin, int dhtType, int buttonPin);
    void begin();
    void read();

private:
    DHT dht;
    int buttonPin;
    static volatile bool buttonPressed; // Static variable to track button press
    static void IRAM_ATTR handleButtonPress(); // Use IRAM_ATTR for ISR
};

#endif
