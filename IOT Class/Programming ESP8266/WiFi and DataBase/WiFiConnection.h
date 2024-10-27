#ifndef WiFiConnection_h
#define WiFiConnection_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

class WiFiConnection {
public:
    WiFiConnection(const char* ssid, const char* password);
    void connect();
    void printConnectionDetails();
    
private:
    const char* _ssid;
    const char* _password;
};

#endif
