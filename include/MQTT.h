#ifndef MQTT_h
#define MQTT_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <config.h>

class MQTT {
 private:
  unsigned long lastReconnectAttempt = 0;
  void (*callback)(boolean isConnected);

  WiFiClient wifiClient;
  PubSubClient client = PubSubClient(wifiClient);

  boolean reconnect();

 public:
  void begin(void (*callback)(boolean isConnected));
  void loop();
};

#endif