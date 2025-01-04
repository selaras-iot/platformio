#ifndef MQTT_h
#define MQTT_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <config.h>

enum class MQTT_TOPIC {
  MODE,
  BRIGHTNESS,
  SPEED,
  COLOR,
};

class MQTT {
 private:
  unsigned long lastReconnectAttempt = 0;
  void (*onConnectionChanged)(boolean isConnected);
  void (*onMessageReceived)(MQTT_TOPIC topic, String payload);

  WiFiClient wifiClient;
  PubSubClient client = PubSubClient(wifiClient);

  boolean reconnect();

 public:
  void begin(void (*onConnectionChanged)(boolean isConnected),
             void (*onMessageReceived)(MQTT_TOPIC topic, String payload));
  void loop();
};

#endif