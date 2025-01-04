#include <MQTT.h>

boolean MQTT::reconnect() {
  Serial.print("Attempting MQTT connection...");
  String clientId = "Selaras-ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  Serial.println(clientId);

  if (MQTT::client.connect(clientId.c_str())) {
    String org = "com.rizalanggoro.selaras";
    String topic = org + "/" + DEVICE_UUID + "/#";

    MQTT::client.subscribe(topic.c_str());
  }

  return MQTT::client.connected();
}

void MQTT::begin(void (*onConnectionChanged)(boolean isConnected),
                 void (*onMessageReceived)(MQTT_TOPIC topic, String payload)) {
  MQTT::onConnectionChanged = onConnectionChanged;

  MQTT::client.setServer(MQTT_BROKER, 1883);
  MQTT::client.setCallback(
      [onMessageReceived](char* topic, byte* payload, unsigned int length) {
        String strTopic = String(topic);
        String strPayload = "";

        for (unsigned int a = 0; a < length; a++) {
          strPayload += (char)payload[a];
        }

        Serial.printf("Message arrived [%s] %s\n", strTopic.c_str(),
                      strPayload.c_str());

        if (strTopic.endsWith("mode"))
          onMessageReceived(MQTT_TOPIC::MODE, strPayload);
        else if (strTopic.endsWith("brightness"))
          onMessageReceived(MQTT_TOPIC::BRIGHTNESS, strPayload);
      });
}

void MQTT::loop() {
  if (!MQTT::client.connected()) {
    unsigned long currentMillis = millis();
    if (currentMillis - MQTT::lastReconnectAttempt > 5000) {
      MQTT::lastReconnectAttempt = currentMillis;
      if (reconnect()) {
        MQTT::lastReconnectAttempt = 0;
        MQTT::onConnectionChanged(true);
        Serial.println("Connected to broker!");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        MQTT::onConnectionChanged(false);
      }
    }
  } else {
    client.loop();
  }
}