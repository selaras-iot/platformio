#include <MQTT.h>

void MQTT::initializeTopics() {
  String org = "com.rizalanggoro.selaras";
  String prefix = org + "/" + DEVICE_UUID + "/";

  MQTT::subTopicMode = prefix + "mode";
  MQTT::subTopicBrightness = prefix + "brightness";
}

boolean MQTT::reconnect() {
  Serial.print("Attempting MQTT connection...");
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  Serial.println(clientId);

  if (client.connect(clientId.c_str())) {
    client.subscribe(MQTT::subTopicMode.c_str());
    client.subscribe(MQTT::subTopicBrightness.c_str());
  }

  return client.connected();
}

void MQTT::begin(void (*callback)(boolean isConnected)) {
  MQTT::callback = callback;
  MQTT::initializeTopics();

  MQTT::client.setServer(MQTT_BROKER, 1883);
  MQTT::client.setCallback([](char* topic, byte* payload, unsigned int length) {
    String strTopic = String(topic);
    String strPayload = "";

    for (unsigned int a = 0; a < length; a++) {
      strPayload += (char)payload[a];
    }

    Serial.printf("Message arrived [%s] %s\n", strTopic.c_str(),
                  strPayload.c_str());
  });
}

void MQTT::loop() {
  if (!MQTT::client.connected()) {
    unsigned long currentMillis = millis();
    if (currentMillis - MQTT::lastReconnectAttempt > 5000) {
      MQTT::lastReconnectAttempt = currentMillis;
      if (reconnect()) {
        MQTT::lastReconnectAttempt = 0;
        MQTT::callback(true);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        MQTT::callback(false);
      }
    }
  } else {
    client.loop();
  }
}