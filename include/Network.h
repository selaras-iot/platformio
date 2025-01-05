#ifndef Network_h
#define Network_h

#include <Arduino.h>
#include <ESP8266WiFi.h>

enum class NetworkMode { STA, AP };

class Network {
 private:
  unsigned long previousMillis = 0;
  boolean isConnected = false;
  NetworkMode networkMode;

 public:
  void begin(String ssid, String password, NetworkMode networkMode);
  void loop();
};

#endif