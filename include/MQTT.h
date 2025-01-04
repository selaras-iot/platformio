#ifndef MQTT_h
#define MQTT_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <config.h>

class MQTT {
 private:
  String subTopicMode = "", subTopicBrightness = "";
  unsigned long lastReconnectAttempt = 0;

  WiFiClient wifiClient;
  PubSubClient client = PubSubClient(wifiClient);

  void initializeTopics();
  boolean reconnect();

  void (*callback)(boolean isConnected);

 public:
  void begin(void (*callback)(boolean isConnected));
  void loop();
};

#endif